#include "milk.h"
#include "milk_api.h"
#include "milk_bmp.h"
#include <memory.h>
#include <stdio.h>

#define COORD_2_FRAMEBUF_POS(x, y) ((MILK_FRAMEBUF_WIDTH * y) + x)
#define HEX_2_COLOR(h) ((ColorRGB){((h >> 16)), ((h >> 8)), ((h)) })

Milk *milk_init()
{
	Milk *milk = (Milk *)calloc(1, sizeof(Milk));
	milk_load_bmp(milk, "./sprsheet.bmp");
	milk_load_code(milk);
	return milk;
}

void milk_free(Milk *milk)
{
	free(milk);
}

void milk_update(Milk *milk)
{
	milk_invoke_update(&milk->code);
}

void __reset_draw_state(Video *video)
{
	video->colorkey = HEX_2_COLOR(0x000000);
}

void milk_draw(Milk *milk)
{
	__reset_draw_state(&milk->video);
	milk_invoke_draw(&milk->code);
}

void milk_clear(Video *video, int hex)
{
	int i;
	ColorRGB color = HEX_2_COLOR(hex);

	for (i = 0; i < MILK_FRAMEBUF_WIDTH * MILK_FRAMEBUF_HEIGHT; i++)
	{
		video->framebuffer[i] = color;
	}
}

static void __framebuf_cull_xy(int *x, int *y)
{
	if (x != NULL)
	{
		if (x < 0)
			x = 0;
		else if (x > MILK_FRAMEBUF_WIDTH)
			x = MILK_FRAMEBUF_WIDTH;
	}
	if (y != NULL)
	{
		if (y < 0)
			y = 0;
		else if (y > MILK_FRAMEBUF_HEIGHT)
			y = MILK_FRAMEBUF_HEIGHT;
	}
}

static void __framebuf_cull_xywh(int *x, int *y, int *w, int *h)
{
	__framebuf_cull_xy(x, y);

	int right = *x + *w;
	int bottom = *y + *h;
	int clamped_r = right;
	int clamped_b = bottom;

	__framebuf_cull_xy(&clamped_r, &clamped_b);

	if (clamped_r != right)
		*w -= (right - clamped_r);
	if (clamped_b != bottom)
		*h -= (bottom - clamped_b);
}

static int __color_eq(ColorRGB *col1, ColorRGB *col2)
{
	return col1->r == col2->r && col1->g == col2->g && col1->b == col2->b;
}

static void __pixel_set(Video *video, ColorRGB *color, int x, int y)
{
	video->framebuffer[COORD_2_FRAMEBUF_POS(x, y)] = *color;
}

void milk_pixel_set(Video *video, int hex, int x, int y)
{
	__pixel_set(video, hex, x, y);
}

void milk_rectfill(Video *video, int hex, int x, int y, int w, int h)
{
	int i = x, j = y;
	__framebuf_cull_xywh(&i, &j, &w, &h);

	ColorRGB color = HEX_2_COLOR(hex);

	for (i = x; i < x + w; i++)
	{
		for (j = y; j < y + h; j++)
		{
			if (i < MILK_FRAMEBUF_WIDTH && j < MILK_FRAMEBUF_HEIGHT)
				__pixel_set(video, &color, i, j);
		}
	}
}

static void __horizontal_line(Video *video, ColorRGB *color, int x, int y, int w)
{
	int i;
	for (i = x; i <= x + w; i++)
	{
		if (i < MILK_FRAMEBUF_WIDTH)
			__pixel_set(video, color, i, y);
	}
}

static void __vertical_line(Video *video, ColorRGB *color, int x, int y, int h)
{
	int i;
	for (i = y; i <= y + h; i++)
	{
		if (i < MILK_FRAMEBUF_WIDTH)
			__pixel_set(video, color, x, i);
	}
}

void milk_rect(Video *vram, int hex, int x, int y, int w, int h)
{
	ColorRGB color = HEX_2_COLOR(hex);

	__framebuf_cull_xywh(&x, &y, &w, &h);
	__horizontal_line(vram, &color, x, y, w);
	__horizontal_line(vram, &color, x, y + h, w);
	__vertical_line(vram, &color, x, y, h);
	__vertical_line(vram, &color, x + w, y, h);
}

static void __blit(Video *video, ColorRGB* pixels, int x, int y)
{
	int framebufx, framebufy;
	int spritex, spritey;

	for (framebufx = x, spritex = 0; framebufx < x + MILK_SPR_SQRSIZE; framebufx++, spritex++)
	{
		for (framebufy = y, spritey = 0; framebufy < y + MILK_SPR_SQRSIZE; framebufy++, spritey++)
		{
			if (framebufx < MILK_FRAMEBUF_WIDTH && framebufy < MILK_FRAMEBUF_HEIGHT)
			{
				ColorRGB *col = &pixels[MILK_SPRSHEET_SQRSIZE * spritey + spritex];

				if (!__color_eq(col, &video->colorkey))
					__pixel_set(video, col, framebufx, framebufy);
			}
		}
	}
}

void milk_spr(Video *video, int idx, int x, int y)
{
	if (idx < 0)
		return;
	if (idx > MILK_SPRSHEET_SQRSIZE)
		return;

	__blit(video, &video->spritesheet[idx * MILK_SPR_SQRSIZE], x, y);
}
