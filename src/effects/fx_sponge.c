#include "fx.h"

#include "pd_api.h"
#include "../mathlib.h"
#include "../util/pixel_ops.h"
#include <stdbool.h>

// somewhat based on https://www.shadertoy.com/view/ldyGWm

#define MAX_TRACE_STEPS 8
#define FAR_DIST 5.0f

typedef struct TraceState
{
	float t;
	float rotmx, rotmy;
	float3 pos;
} TraceState;

static float scene_sdf(float3 q)
{
	// Layer one. The ".05" on the end varies the hole size.
	// p = abs(fract(q / 3) * 3 - 1.5)
	float3 p = v3_fract(v3_mulfl(q, 0.333333f));
	p = v3_abs(v3_subfl(v3_mulfl(p, 3.0f), 1.5f));
	float d = MIN(MAX(p.x, p.y), MIN(MAX(p.y, p.z), MAX(p.x, p.z))) - 1.0f + 0.05f;

	// Layer two
	p = v3_abs(v3_subfl(v3_fract(q), 0.5f));
	d = MAX(d, MIN(MAX(p.x, p.y), MIN(MAX(p.y, p.z), MAX(p.x, p.z))) - (1.0f / 3.0f) + 0.05f);

	return d;
}

static int trace_ray(const TraceState* st, float x, float y)
{
	float3 pos = st->pos;
	float3 dir = { x, y, 1.0f }; // do not normalize on purpose lol

	// Rotate the ray
	// dir.xy = mat2(m.y, -m.x, m)*dir.xy
	float nx = st->rotmy * dir.x + st->rotmx * dir.y;
	float ny = st->rotmx * dir.x - st->rotmy * dir.y;
	dir.x = nx;
	dir.y = ny;
	// dir.xz = mat2(m.y, -m.x, m)*dir.xz
	nx = st->rotmy * dir.x + st->rotmx * dir.z;
	ny = st->rotmx * dir.x - st->rotmy * dir.z;
	dir.x = nx;
	dir.z = ny;

	float t = 0.0f;
	int i;
	for (i = 0; i < MAX_TRACE_STEPS; ++i)
	{
		float3 q = v3_add(pos, v3_mulfl(dir, t));
		float d = scene_sdf(q);
		if (d < t * 0.05f || d > FAR_DIST)
			break;
		t += d;
	}
	//return MIN((int)(t * 0.3f * 255.0f), 255);
	return 255 - i * 31;
}

static int s_frame_count = 0;

static void do_render(float crank_angle, float time, uint8_t* framebuffer, int framebuffer_stride)
{
	TraceState st;
	st.t = time;
	st.pos.x = 0.0f;
	st.pos.y = 0.0f;
	st.pos.z = time;
	st.rotmx = sinf(st.t / 4.0f);
	st.rotmy = sinf(st.t / 4.0f + M_PIf * 0.5f);

	// trace one ray per 2x2 pixel block
	// x: -1.67 .. +1.67
	// y: -1.0 .. +1.0
	float xext = 1.6667f;
	float yext = 1.0f;
	float dx = xext * 4 / LCD_COLUMNS;
	float dy = yext * 4 / LCD_ROWS;

	float y = yext - dy * 0.5f;
	for (int py = 0; py < LCD_ROWS / 2; ++py, y -= dy)
	{
		// Temporal: every frame update just one out of every 2x2 pixel blocks.
		// Which means every other frame we skip every other row.
		if ((s_frame_count & 1) == (py & 1))
			continue;

		float x = -xext + dx * 0.5f;
		int pix_idx = py * LCD_COLUMNS / 2;
		// And for each row we step at 2 pixels, but shift location by one every
		// other frame.
		if ((s_frame_count & 2)) {
			x += dx;
			pix_idx++;
		}
		for (int px = 0; px < LCD_COLUMNS / 2; px += 2, x += dx * 2, pix_idx += 2)
		{
			int val = trace_ray(&st, x, y);
			g_screen_buffer_2x2sml[pix_idx] = val;
		}
	}

	memcpy(g_screen_buffer, g_screen_buffer_2x2sml, LCD_COLUMNS/2*LCD_ROWS/2);
	draw_dithered_screen_2x2(framebuffer, 1);
	++s_frame_count;
}

static int fx_sponge_update(uint32_t buttons_cur, uint32_t buttons_pressed, float crank_angle, float time, uint8_t* framebuffer, int framebuffer_stride)
{
	do_render(crank_angle, time, framebuffer, framebuffer_stride);
	return MAX_TRACE_STEPS;
}

Effect fx_sponge_init(void* pd_api)
{
	return (Effect) { fx_sponge_update };
}