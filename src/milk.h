#ifndef __MILK_H__
#define __MILK_H__

#include <stdint.h>

#define MILK_FRAMERATE (1000 / 30) /* Fixed 30 FPS */
#define MILK_FRAMEBUF_WIDTH 256
#define MILK_FRAMEBUF_HEIGHT 224
#define MILK_FRAMEBUF_AREA (MILK_FRAMEBUF_WIDTH * MILK_FRAMEBUF_HEIGHT)
#define MILK_WINDOW_WIDTH MILK_FRAMEBUF_WIDTH * 2
#define MILK_WINDOW_HEIGHT MILK_FRAMEBUF_HEIGHT * 2
#define MILK_SPRSHEET_SQRSIZE 256
#define MILK_SPRSHEET_AREA MILK_SPRSHEET_SQRSIZE * MILK_SPRSHEET_SQRSIZE
#define MILK_SPR_SQRSIZE 16 /* Each sprite is considered to be 16x16 px */
#define MILK_SPR_FILENAME "sprsheet.bmp"
#define MILK_FONT_FILENAME "font.bmp"
#define MILK_FONT_WIDTH 128
#define MILK_FONT_HEIGHT 48
#define MILK_FONT_AREA MILK_FONT_WIDTH * MILK_FONT_HEIGHT
#define MILK_CHAR_SQRSIZE 8

#define MILK_BOOL int
#define MILK_TRUE 1
#define MILK_FALSE 0

/*
 * Milk supports 3 channel, 24 bit colors.
 */
typedef struct ColorRGB
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} ColorRGB;

/*
 * Essentially VRAM.
 * - The framebuffer is what we actively draw to, and display at the end of each frame.
 * - The sprite sheet is a single 256x256 px image buffer.
 * - The color key is the current color to consider as transparent.
 */
typedef struct Video
{
	ColorRGB framebuffer[MILK_FRAMEBUF_AREA];
	ColorRGB spritesheet[MILK_SPRSHEET_AREA];
    ColorRGB font[MILK_FONT_AREA];
	ColorRGB colorKey;
} Video;

typedef enum ButtonState
{
	UP = 1 << 0,
	DOWN = 1 << 1,
	LEFT = 1 << 2,
	RIGHT = 1 << 3,
	A = 1 << 4,
	B = 1 << 5,
	X = 1 << 6,
	Y = 1 << 7
} ButtonState;

typedef struct Gamepad
{
	ButtonState buttonState;
	ButtonState previousButtonState;
} Gamepad;

typedef struct Input
{
	uint32_t mouseX;
	uint32_t mouseY;
	MILK_BOOL mouseDown;
	MILK_BOOL mouseDownPrevious;
	Gamepad gamepad;
} Input;

/*
 * Code is the client side script(s).
 * - state holds the current state of the program. In this case, it is a Lua state.
 */
typedef struct Code
{
	void *state;
} Code;

typedef struct Milk
{
	Input input;
	Video video;
	Code code;
} Milk;

/*
 * Initialize milk, loading all of it's content.
 */
Milk *milkInit();

/*
 * Free milk and all of it's content.
 */
void milkFree(Milk *milk);

/*
 * Update milk's current state.
 */
void milkUpdate(Milk *milk);

/*
 * Draw milk's current state.
 */
void milkDraw(Milk *milk);

/*
 * Clear milk's framebuffer to the specified color.
 */
void milkClear(Video *video, int idx);

/*
 * Set the framebuffer's pixel at the given coordinates.
 */
void milkPixelSet(Video *video, int hex, int x, int y);

/*
 * Draw a solid rectangle to the framebuffer at the given coordinates.
 */
void milkRectFill(Video *video, int hex, int x, int y, int w, int h);

/*
 * Draw a rectangle to the framebuffer at the given coordinates.
 */
void milkRect(Video *video, int hex, int x, int y, int w, int h);

/*
 * Draw a sprite to the framebuffer at the given coordinates.
 *
 * - I'll have a milk sprite with my milk steak please.
 */
void milkSprite(Video *video, int idx, int x, int y);

int milkButton(Input *input, ButtonState button);

void milkSpriteFont(Video *video, const char *str, int x, int y);

#endif
