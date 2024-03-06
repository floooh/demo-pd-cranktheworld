#include "image_loader.h"

#include "../allocator.h"

#include "pd_api.h"

#include <assert.h>

typedef struct TgaHeader
{
	uint8_t id_size;
	uint8_t color_map_type;
	uint8_t image_type;
	uint8_t color_map_desc[5];
	uint16_t x_origin;
	uint16_t y_origin;
	uint16_t width;
	uint16_t height;
	uint8_t bits_per_pixel;
	uint8_t image_desc;
} TgaHeader;

_Static_assert(sizeof(TgaHeader) == 18, "TgaHeader should be 18 bytes");


uint8_t* read_tga_file_grayscale(const char* path, void* pd_api, int* out_w, int* out_h)
{
	*out_w = *out_h = 0;
	PlaydateAPI* pd = (PlaydateAPI*)pd_api;
	SDFile* file = pd->file->open(path, kFileRead);
	if (!file)
		return NULL;

	uint8_t* res = NULL;
	TgaHeader header;
	int bytes = pd->file->read(file, &header, sizeof(header));
	if (bytes != sizeof(header))
		goto _exit;

	if (header.image_type != 3) // only support grayscale images
		goto _exit;
	if (header.bits_per_pixel != 8) // only support 8bpp
		goto _exit;

	if (header.width == 0 || header.width > 2048 || header.height == 0 || header.height > 2048) // out of bounds sizes
		goto _exit;

	*out_w = header.width;
	*out_h = header.height;

	int image_size = header.width * header.height;
	res = pd_malloc(image_size);

	pd->file->seek(file, header.id_size, SEEK_CUR);
	bytes = pd->file->read(file, res, image_size);
	if (bytes != image_size) {
		pd_free(res);
		res = NULL;
		goto _exit;
	}

_exit:
	pd->file->close(file);
	return res;
}
