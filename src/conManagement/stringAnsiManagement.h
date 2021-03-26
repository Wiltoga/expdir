#ifndef DEF_STRING_ANSI_MANAGEMENT_H
#define DEF_STRING_ANSI_MANAGEMENT_H
#include <stdlib.h>
#include <stdint.h>
#include "color.h"
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param  flags flags to apply
 * @return size of the added characters to the buffer
 */
size_t string_formatMode(void *buffer, uint8_t flags);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param background background color of the text
 * @return size of the added characters to the buffer
 */
size_t string_formatBackground(void *buffer, color_t background);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param background background color of the text
 * @param  flags flags to apply
 * @return size of the added characters to the buffer
 */
size_t string_formatBackgroundMode(void *buffer, color_t background, uint8_t flags);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param foreground foreground color of the text
 * @return size of the added characters to the buffer
 */
size_t string_formatForeground(void *buffer, color_t foreground);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param foreground foreground color of the text
 * @param  flags flags to apply
 * @return size of the added characters to the buffer
 */
size_t string_formatForegroundMode(void *buffer, color_t foreground, uint8_t flags);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param foreground foreground color of the text
 * @param background background color of the text
 * @return size of the added characters to the buffer
 */
size_t string_formatColor(void *buffer, color_t foreground, color_t background);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param foreground foreground color of the text
 * @param background background color of the text
 * @param  flags flags to apply
 * @return size of the added characters to the buffer
 */
size_t string_formatColorMode(void *buffer, color_t foreground, color_t background, uint8_t flags);
/**
 * @brief  Saves the position of the cursor, to be restored later
 * @param  buffer buffer to write into 
 * @return size of the added characters to the buffer
 */
size_t string_saveCursorPosition(void *buffer);
/**
 * @brief  Resets the formatting done using ANSI characters
 * @param  buffer buffer to write into 
 * @return size of the added characters to the buffer
 */
size_t string_resetFormatting(void *buffer);
/**
 * @brief  Retores the position of the cursor to where it was saved
 * @param  buffer buffer to write into 
 * @return size of the added characters to the buffer
 */
size_t string_restoreCursorPosition(void *buffer);
/**
 * @brief  Clears the remaining characters of the current line, including the current position
 * @param  buffer buffer to write into 
 * @return size of the added characters to the buffer
 */
size_t string_eraseEndOfLine(void *buffer);
/**
 * @brief  Moves the cursor to the specified position
 * @param  buffer buffer to write into 
 * @param x horizontal position
 * @param y vertical position
 * @return size of the added characters to the buffer
 */
size_t string_setCursorPosition(void *buffer, int x, int y);
/**
 * @brief  Clears the current screen and set the cursor at the default position (1,1)
 * @param  buffer buffer to write into 
 * @return size of the added characters to the buffer
 */
size_t string_clearScreen(void *buffer);

#endif