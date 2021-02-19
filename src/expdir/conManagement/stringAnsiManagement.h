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
 * @brief  Writes the string inside the buffer, without the '\0'
 * @param  buffer buffer to write into 
 * @param  content content of the string to write
 * @return size of the byte written
 */
size_t string_write(void *buffer, char *content);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param background background color of the text
 * @return size of the added characters to the buffer
 */
size_t string_formatRGBBackground(void *buffer, color background);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param background background color of the text
 * @param  flags flags to apply
 * @return size of the added characters to the buffer
 */
size_t string_formatRGBBackgroundMode(void *buffer, color background, uint8_t flags);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param background background color of the text
 * @return size of the added characters to the buffer
 */
size_t string_formatSystemBackground(void *buffer, int background);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param background background color of the text
 * @param  flags flags to apply
 * @return size of the added characters to the buffer
 */
size_t string_formatSystemBackgroundMode(void *buffer, int background, uint8_t flags);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param foreground foreground color of the text
 * @return size of the added characters to the buffer
 */
size_t string_formatRGBForeground(void *buffer, color foreground);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param foreground foreground color of the text
 * @param  flags flags to apply
 * @return size of the added characters to the buffer
 */
size_t string_formatRGBForegroundMode(void *buffer, color foreground, uint8_t flags);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param foreground foreground color of the text
 * @return size of the added characters to the buffer
 */
size_t string_formatSystemForeground(void *buffer, int foreground);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param foreground foreground color of the text
 * @param  flags flags to apply
 * @return size of the added characters to the buffer
 */
size_t string_formatSystemForegroundMode(void *buffer, int foreground, uint8_t flags);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param foreground foreground color of the text
 * @param background background color of the text
 * @return size of the added characters to the buffer
 */
size_t string_formatRGBColor(void *buffer, color foreground, color background);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param foreground foreground color of the text
 * @param background background color of the text
 * @param  flags flags to apply
 * @return size of the added characters to the buffer
 */
size_t string_formatRGBColorMode(void *buffer, color foreground, color background, uint8_t flags);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param foreground foreground color of the text
 * @param background background color of the text
 * @return size of the added characters to the buffer
 */
size_t string_formatSystemColor(void *buffer, int foreground, int background);
/**
 * @brief  Writes ANSI format characters
 * @param  buffer buffer to write into 
 * @param foreground foreground color of the text
 * @param background background color of the text
 * @param  flags flags to apply
 * @return size of the added characters to the buffer
 */
size_t string_formatSystemColorMode(void *buffer, int foreground, int background, uint8_t flags);
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