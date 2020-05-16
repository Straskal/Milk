#include "milk.h"
#include "milk_api.h"
#include "milk_bmp.h"
#include <math.h>
#include <memory.h>
#include <stdio.h>

#define FRAMEBUFFER_POS(x, y) ((MILK_FRAMEBUF_WIDTH * y) + x)
#define FRAMEBUFFER_CAN_SET(x, y) (0 < x && x < MILK_FRAMEBUF_WIDTH && 0 < y && y < MILK_FRAMEBUF_HEIGHT)

Milk *milkInit()
{
	Milk *milk = (Milk *)calloc(1, sizeof(Milk));
	milkLoadSpritesheet(&milk->video.spritesheet, MILK_SPRSHEET_FILENAME);
	milkLoadFont(&milk->video.font, MILK_FONT_FILENAME);
	milkLoadScripts(milk);
	return milk;
}

void milkFree(Milk *milk)
{
	free(milk);
}

void milkUpdate(Milk *milk)
{
	milkInvokeUpdate(&milk->code);
}

static void _resetDrawState(Video *video)
{
	video->colorKey = 0;
}

void milkDraw(Milk *milk)
{
	_resetDrawState(&milk->video);
	milkInvokeDraw(&milk->code);
}

int milkButton(Input *input, uint8_t button)
{
	return (input->gamepad.buttonState & button) == button;
}

void milkClear(Video *video, Color32 color)
{
	for (int i = 0; i < MILK_FRAMEBUF_WIDTH * MILK_FRAMEBUF_HEIGHT; i++)
	{
		video->framebuffer[i] = color;
	}
}

void milkPixelSet(Video *video, int x, int y, Color32 color)
{
	if (FRAMEBUFFER_CAN_SET(x, y))
		video->framebuffer[FRAMEBUFFER_POS(x, y)] = color;
}

void milkRectFill(Video *video, int x, int y, int w, int h, Color32 color)
{
	for (int j = y; j < y + h; j++)
	{
		for (int i = x; i < x + w; i++)
		{
			milkPixelSet(video, i, j, color);
		}
	}
}

static void _horizontalLine(Video *video, int x, int y, int w, Color32 color)
{
	for (int i = x; i <= x + w; i++)
	{
		milkPixelSet(video, i, y, color);
	}
}

static void _verticalLine(Video *video, int x, int y, int h, Color32 color)
{
	for (int i = y; i <= y + h; i++)
	{
		milkPixelSet(video, x, i, color);
	}
}

void milkRect(Video *vram, int x, int y, int w, int h, Color32 color)
{
	_horizontalLine(vram, x, y, w, color);
	_horizontalLine(vram, x, y + h, w, color);
	_verticalLine(vram, x, y, h, color);
	_verticalLine(vram, x + w, y, h, color);
}

static void _blitRect(Video *video, Color32 *pixels, int x, int y, int w, int h, int pitch, float scale)
{
	int xPixel;
	int yPixel;
	int xFramebuffer;
	int yFramebuffer;

	if (scale == 1.0f)
	{
		for (yFramebuffer = y, yPixel = 0; yFramebuffer < y + h; yFramebuffer++, yPixel++)
		{
			for (xFramebuffer = x, xPixel = 0; xFramebuffer < x + w; xFramebuffer++, xPixel++)
			{
				Color32 col = pixels[yPixel * pitch + xPixel];

				if (col != video->colorKey)
					milkPixelSet(video, xFramebuffer, yFramebuffer, col);
			}
		}
	}
	else
	{
		float scaledWidth = w * scale;
		float scaledHeight = h * scale;
		int xRatio = (int)((w << 16) / scaledWidth) + 1;
		int yRatio = (int)((h << 16) / scaledHeight) + 1;

		for (yFramebuffer = y, yPixel = 0; yFramebuffer < y + scaledHeight; yFramebuffer++, yPixel++)
		{
			for (xFramebuffer = x, xPixel = 0; xFramebuffer < x + scaledWidth; xFramebuffer++, xPixel++)
			{
				int xNearest = (xPixel * xRatio) >> 16;
				int yNearest = (yPixel * yRatio) >> 16;

				Color32 col = pixels[yNearest * pitch + xNearest];

				if (col != video->colorKey)
					milkPixelSet(video, xFramebuffer, yFramebuffer, col);
			}
		}
	}
}

void milkSprite(Video *video, int idx, int x, int y, int w, int h, float scale)
{
	if (idx < 0 || MILK_SPRSHEET_SQRSIZE < idx)
		return;

	int row = floor(idx / 16);
	int col = floor(idx % 16);
	Color32 *pixels = &video->spritesheet[row * MILK_SPRSHEET_SQRSIZE * MILK_SPRSHEET_SPR_SQRSIZE + col * MILK_SPRSHEET_SPR_SQRSIZE];

	_blitRect(video, pixels, x, y, MILK_SPRSHEET_SPR_SQRSIZE * w, MILK_SPRSHEET_SPR_SQRSIZE * h, MILK_SPRSHEET_SQRSIZE, scale);
}

static int _isAscii(char character)
{
	return (character & 0xff80) == 0;
}

static void _drawCharacter(Video *video, char character, int x, int y)
{
	if (!_isAscii(character))
		character = '?';

	/* bitmap font starts at ASCII character 32 (SPACE) */
	int row = floor((character - 32) / 16);
	int col = floor((character - 32) % 16);
	Color32 *pixels = &video->font[(row * MILK_FONT_WIDTH * MILK_CHAR_SQRSIZE + col * MILK_CHAR_SQRSIZE)];

	_blitRect(video, pixels, x, y, MILK_CHAR_SQRSIZE, MILK_CHAR_SQRSIZE, MILK_FONT_WIDTH, 1.0f);
}

void milkSpriteFont(Video *video, int x, int y, const char *str)
{
	char *currentChar = str;

	while (*currentChar)
	{
		_drawCharacter(video, *(currentChar++), x, y);

		x += MILK_CHAR_SQRSIZE;
	}
}
