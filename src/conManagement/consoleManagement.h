#ifndef DEF_CONSOLE_MANAGEMENT_H
#define DEF_CONSOLE_MANAGEMENT_H
#include <stdint.h>
#include "color.h"

/**
 * @brief  Writes formatted text in the console
 * @param  content string to write in the console 
 * @param  flags flags to apply
 * @return void
 */
void console_formatMode(char *content, uint8_t flags, ...);
/**
 * @brief  Writes formatted text in the console
 * @param  content string to write in the console 
 * @param background background color of the text
 * @return void
 */
void console_formatBackground(char *content, color_t background, ...);
/**
 * @brief  Writes formatted text in the console
 * @param  content string to write in the console 
 * @param background background color of the text
 * @param  flags flags to apply
 * @return void
 */
void console_formatBackgroundMode(char *content, color_t background, uint8_t flags, ...);
/**
 * @brief  Writes formatted text in the console
 * @param  content string to write in the console 
 * @param foreground foreground color of the text
 * @return void
 */
void console_formatForeground(char *content, color_t foreground, ...);
/**
 * @brief  Writes formatted text in the console
 * @param  content string to write in the console 
 * @param foreground foreground color of the text
 * @param  flags flags to apply
 * @return void
 */
void console_formatForegroundMode(char *content, color_t foreground, uint8_t flags, ...);
/**
 * @brief  Writes formatted text in the console
 * @param  content string to write in the console 
 * @param foreground foreground color of the text
 * @param background background color of the text
 * @return void
 */
void console_formatColor(char *content, color_t foreground, color_t background, ...);
/**
 * @brief  Writes formatted text in the console
 * @param  content string to write in the console 
 * @param foreground foreground color of the text
 * @param background background color of the text
 * @param  flags flags to apply
 * @return void
 */
void console_formatColorMode(char *content, color_t foreground, color_t background, uint8_t flags, ...);
/**
 * @brief  Saves the position of the cursor, to be restored later
 * @return void
 */
void console_saveCursorPosition();
/**
 * @brief  Retores the position of the cursor to where it was saved
 * @return void
 */
void console_restoreCursorPosition();
/**
 * @brief  Clears the remaining characters of the current line, including the current position
 * @return void
 */
void console_eraseEndOfLine();
/**
 * @brief  Moves the cursor to the specified position
 * @param x horizontal position
 * @param y vertical position
 * @return void
 */
void console_setCursorPosition(int x, int y);
/**
 * @brief  Clears the current screen and set the cursor at the default position (1,1)
 * @return void
 */
void console_clearScreen();

#define CONSOLE_KEY_OTHER 0
#define CONSOLE_KEY_UP 1
#define CONSOLE_KEY_DOWN 2
#define CONSOLE_KEY_LEFT 3
#define CONSOLE_KEY_RIGHT 4
#define CONSOLE_KEY_RETURN 5
/**
 * @brief  Returns a hint of the arrow key pressed
 * @return hint of the key
 */
int console_getArrowPressed();

char getch();

char getche();

#endif