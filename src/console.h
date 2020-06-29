#ifndef __CONSOLE__H__
#define __CONSOLE__H__

#include "milk.h"

#include <stdbool.h>

#define COMMAND_MAX_LENGTH 25

typedef enum consoleInputState
{
	CONSOLE_INPUT_NONE = 0 << 0,
	CONSOLE_INPUT_CHAR = 1 << 0,
	CONSOLE_INPUT_BACK = 1 << 1,
	CONSOLE_INPUT_ENTER = 1 << 2,
	CONSOLE_INPUT_ESCAPE = 1 << 3
} ConsoleInputState;

typedef struct consoleInput
{
	void(*startTextInput)();
	void(*stopTextInput)();

	ConsoleInputState   state;
	ConsoleInputState   previousState;
	char                currentChar;
} ConsoleInput;

typedef enum consoleState
{
	GAME,
	COMMAND
} ConsoleState;

typedef struct console
{
	ConsoleInput    input;
	ConsoleState    state;
	int             lastErrorCount;
	int             commandCandidateLength;
	int             previousCommandLength;
	char            commandCandidate[COMMAND_MAX_LENGTH];
	char            previousCommand[COMMAND_MAX_LENGTH];
	bool            isGameInitialized;
} Console;

Console *createConsole();

void freeConsole(Console *console);

void updateConsole(Console *console, Milk *milk);

void drawConsole(Console *console, Milk *milk);

#endif
