//
//  render.c
//  Extension
//
//  Created by Dave Hayden on 10/20/15.
//  Copyright © 2015 Panic, Inc. All rights reserved.
//

#include <stdint.h>
#include <string.h>
#include "render.h"

#define SCREEN_Y 240
#define SCREEN_X 400

static inline void
_drawMaskPattern(uint32_t* p, uint32_t mask, uint32_t color)
{
	if ( mask == 0xffffffff )
		*p = color;
	else
		*p = (*p & ~mask) | (color & mask);
}

static void
drawFragment(uint32_t* row, int x1, int x2, uint32_t color)
{
	if ( x2 < 0 || x1 >= SCREEN_X )
		return;
	
	if ( x1 < 0 )
		x1 = 0;
	
	if ( x2 > SCREEN_X )
		x2 = SCREEN_X;
	
	if ( x1 > x2 )
		return;
	
	// Operate on 32 bits at a time
	
	int startbit = x1 % 32;
	uint32_t startmask = swap((1 << (32 - startbit)) - 1);
	int endbit = x2 % 32;
	uint32_t endmask = swap(((1 << endbit) - 1) << (32 - endbit));
	
	int col = x1 / 32;
	uint32_t* p = row + col;

	if ( col == x2 / 32 )
	{
		uint32_t mask = 0;
		
		if ( startbit > 0 && endbit > 0 )
			mask = startmask & endmask;
		else if ( startbit > 0 )
			mask = startmask;
		else if ( endbit > 0 )
			mask = endmask;
		
		_drawMaskPattern(p, mask, color);
	}
	else
	{
		int x = x1;
		
		if ( startbit > 0 )
		{
			_drawMaskPattern(p++, startmask, color);
			x += (32 - startbit);
		}
		
		while ( x + 32 <= x2 )
		{
			_drawMaskPattern(p++, 0xffffffff, color);
			x += 32;
		}
		
		if ( endbit > 0 )
			_drawMaskPattern(p, endmask, color);
	}
}

static inline int32_t slope(float x1, float y1, float x2, float y2)
{
	float dx = x2-x1;
	float dy = y2-y1;
	
	if ( dy < 1 )
		return (int32_t)(dx * (1<<16));
	else
		return (int32_t)(dx / dy * (1<<16));
}

void drawLine(uint8_t* bitmap, int rowstride, const float3* p1, const float3* p2, int thick, const uint8_t pattern[8])
{
	if ( p1->y > p2->y )
	{
		const float3* tmp = p1;
		p1 = p2;
		p2 = tmp;
	}

	int y = (int)p1->y;
	int endy = (int)p2->y;
	
	if ( y >= SCREEN_Y || endy < 0 || MIN(p1->x, p2->x) >= SCREEN_X || MAX(p1->x, p2->x) < 0 )
		return;
	
	int32_t x = (int32_t)(p1->x * (1<<16));
	int32_t dx = slope(p1->x, p1->y, p2->x, p2->y);
	float py = p1->y;
	
	if ( y < 0 )
	{
		x += (int32_t)(-p1->y * dx);
		y = 0;
		py = 0;
	}

	int32_t x1 = (int32_t)(x + dx * (y+1-py));

	while ( y <= endy )
	{
		uint8_t p = pattern[y%8];
		uint32_t color = (p<<24) | (p<<16) | (p<<8) | p;
		
		if ( y == endy )
			x1 = (int32_t)(p2->x * (1<<16));
		
		if ( dx < 0 )
			drawFragment((uint32_t*)&bitmap[y*rowstride], x1>>16, (x>>16) + thick, color);
		else
			drawFragment((uint32_t*)&bitmap[y*rowstride], x>>16, (x1>>16) + thick, color);
		
		if ( ++y == SCREEN_Y )
			break;

		x = x1;
		x1 += dx;
	}
}

static void fillRange(uint8_t* bitmap, int rowstride, int y, int endy, int32_t* x1p, int32_t dx1, int32_t* x2p, int32_t dx2, const uint8_t pattern[8])
{
	int32_t x1 = *x1p, x2 = *x2p;
	
	if ( endy < 0 )
	{
		int dy = endy - y;
		*x1p = x1 + dy * dx1;
		*x2p = x2 + dy * dx2;
		return;
	}
	
	if ( y < 0 )
	{
		x1 += -y * dx1;
		x2 += -y * dx2;
		y = 0;
	}
	
	while ( y < endy )
	{
		uint8_t p = pattern[y%8];
		uint32_t color = (p<<24) | (p<<16) | (p<<8) | p;
		
		drawFragment((uint32_t*)&bitmap[y*rowstride], (x1>>16), (x2>>16)+1, color);
		
		x1 += dx1;
		x2 += dx2;
		++y;
	}
	
	*x1p = x1;
	*x2p = x2;
}

static inline void sortTri(const float3** p1, const float3** p2, const float3** p3)
{
	float y1 = (*p1)->y, y2 = (*p2)->y, y3 = (*p3)->y;
	
	if ( y1 <= y2 && y1 < y3 )
	{
		if ( y3 < y2 ) // 1,3,2
		{
			const float3* tmp = *p2;
			*p2 = *p3;
			*p3 = tmp;
		}
	}
	else if ( y2 < y1 && y2 < y3 )
	{
		const float3* tmp = *p1;
		*p1 = *p2;

		if ( y3 < y1 ) // 2,3,1
		{
			*p2 = *p3;
			*p3 = tmp;
		}
		else // 2,1,3
			*p2 = tmp;
	}
	else
	{
		const float3* tmp = *p1;
		*p1 = *p3;
		
		if ( y1 < y2 ) // 3,1,2
		{
			*p3 = *p2;
			*p2 = tmp;
		}
		else // 3,2,1
			*p3 = tmp;
	}

}

void fillTriangle(uint8_t* bitmap, int rowstride, const float3* p1, const float3* p2, const float3* p3, const uint8_t pattern[8])
{
	// sort by y coord
	
	sortTri(&p1, &p2, &p3);
	
	int endy = MIN(SCREEN_Y, (int)p3->y);
	
	if ( p1->y > SCREEN_Y || endy < 0 )
		return;

	int32_t x1 = (int32_t)(p1->x * (1<<16));
	int32_t x2 = x1;
	
	int32_t sb = slope(p1->x, p1->y, p2->x, p2->y);
	int32_t sc = slope(p1->x, p1->y, p3->x, p3->y);

	int32_t dx1 = MIN(sb, sc);
	int32_t dx2 = MAX(sb, sc);
	
	fillRange(bitmap, rowstride, (int)p1->y, MIN(SCREEN_Y, (int)p2->y), &x1, dx1, &x2, dx2, pattern);
	
	int dx = slope(p2->x, p2->y, p3->x, p3->y);
	
	if ( sb < sc )
	{
		x1 = (int32_t)(p2->x * (1<<16));
		fillRange(bitmap, rowstride, (int)p2->y, endy, &x1, dx, &x2, dx2, pattern);
	}
	else
	{
		x2 = (int32_t)(p2->x * (1<<16));
		fillRange(bitmap, rowstride, (int)p2->y, endy, &x1, dx1, &x2, dx, pattern);
	}
}
