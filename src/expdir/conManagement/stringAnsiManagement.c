#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "stringAnsiManagement.h"

void *begin(void *buffer)
{
    memcpy(buffer, "\x1b[", sizeof(char) * 2);
    return buffer + sizeof(char) * 2;
}

void *end(void *buffer)
{
    *(char *)buffer = 'm';
    return buffer + sizeof(char);
}

void *mode(void *buffer, uint8_t flags)
{
    bool firstAdded = false;
    if (flags & CONSOLE_FLAG_BOLD)
    {
        *(char *)buffer = '1';
        buffer += sizeof(char);
        firstAdded = true;
    }
    if (flags & CONSOLE_FLAG_UNDERLINE)
    {
        if (firstAdded)
        {
            *(char *)buffer = ';';
            buffer += sizeof(char);
        }
        else
            firstAdded = true;
        *(char *)buffer = '4';
        buffer += sizeof(char);
    }
    if (flags & CONSOLE_FLAG_BLINK)
    {
        if (firstAdded)
        {
            *(char *)buffer = ';';
            buffer += sizeof(char);
        }
        else
            firstAdded = true;
        *(char *)buffer = '5';
        buffer += sizeof(char);
    }
    if (flags & CONSOLE_FLAG_REVERSE_COLOR)
    {
        if (firstAdded)
        {
            *(char *)buffer = ';';
            buffer += sizeof(char);
        }
        else
            firstAdded = true;
        *(char *)buffer = '7';
        buffer += sizeof(char);
    }
    return buffer;
}
void *sysColor(void *buffer, uint8_t c)
{
    char b[5];
    snprintf(b, 5 * sizeof(char), "%d", c);
    size_t len = strlen(b);
    memcpy(buffer, b, len * sizeof(char));
    return buffer + sizeof(char) * len;
}
void *rgbForeground(void *buffer, color c)
{
    memcpy(buffer, "38;2;", sizeof(char) * 5);
    buffer += sizeof(char) * 5;
    char b[15];
    snprintf(b, 15 * sizeof(char), "%d;%d;%d", c.r, c.g, c.b);
    size_t len = strlen(b);
    memcpy(buffer, b, len * sizeof(char));
    return buffer + sizeof(char) * len;
}
void *rgbBackground(void *buffer, color c)
{
    memcpy(buffer, "48;2;", sizeof(char) * 5);
    buffer += sizeof(char) * 5;
    char b[15];
    snprintf(b, 15 * sizeof(char), "%d;%d;%d", c.r, c.g, c.b);
    size_t len = strlen(b);
    memcpy(buffer, b, len * sizeof(char));
    return buffer + sizeof(char) * len;
}

size_t string_formatMode(void *buffer, uint8_t flags)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    buffer = mode(buffer, flags);
    buffer = end(buffer);
    return (size_t)buffer - init;
}

size_t string_write(void *buffer, char *content)
{
    size_t len = strlen(content);
    memcpy(buffer, content, len * sizeof(char));
    return len * sizeof(char);
}

size_t string_formatRGBBackground(void *buffer, color background)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    buffer = rgbBackground(buffer, background);
    buffer = end(buffer);
    return (size_t)buffer - init;
}

size_t string_formatRGBBackgroundMode(void *buffer, color background, uint8_t flags)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    buffer = mode(buffer, flags);
    *(char *)buffer = ';';
    buffer = rgbBackground(buffer + sizeof(char), background);
    buffer = end(buffer);
    return (size_t)buffer - init;
}

size_t string_formatSystemBackground(void *buffer, int background)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    buffer = sysColor(buffer, background + 10);
    buffer = end(buffer);
    return (size_t)buffer - init;
}

size_t string_formatSystemBackgroundMode(void *buffer, int background, uint8_t flags)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    buffer = mode(buffer, flags);
    *(char *)buffer = ';';
    buffer = sysColor(buffer + sizeof(char), background + 10);
    buffer = end(buffer);
    return (size_t)buffer - init;
}

size_t string_formatRGBForeground(void *buffer, color foreground)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    buffer = rgbForeground(buffer, foreground);
    buffer = end(buffer);
    return (size_t)buffer - init;
}

size_t string_formatRGBForegroundMode(void *buffer, color foreground, uint8_t flags)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    buffer = mode(buffer, flags);
    *(char *)buffer = ';';
    buffer = rgbForeground(buffer + sizeof(char), foreground);
    buffer = end(buffer);
    return (size_t)buffer - init;
}

size_t string_formatSystemForeground(void *buffer, int foreground)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    buffer = sysColor(buffer, foreground);
    buffer = end(buffer);
    return (size_t)buffer - init;
}

size_t string_formatSystemForegroundMode(void *buffer, int foreground, uint8_t flags)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    buffer = mode(buffer, flags);
    *(char *)buffer = ';';
    buffer = sysColor(buffer + sizeof(char), foreground);
    buffer = end(buffer);
    return (size_t)buffer - init;
}

size_t string_formatRGBColor(void *buffer, color foreground, color background)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    buffer = rgbForeground(buffer, foreground);
    *(char *)buffer = ';';
    buffer = rgbForeground(buffer + sizeof(char), background);
    buffer = end(buffer);
    return (size_t)buffer - init;
}

size_t string_formatRGBColorMode(void *buffer, color foreground, color background, uint8_t flags)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    buffer = mode(buffer, flags);
    *(char *)buffer = ';';
    buffer = rgbForeground(buffer, foreground);
    *(char *)buffer = ';';
    buffer = rgbForeground(buffer + sizeof(char), background);
    buffer = end(buffer);
    return (size_t)buffer - init;
}

size_t string_formatSystemColor(void *buffer, int foreground, int background)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    buffer = sysColor(buffer, foreground);
    *(char *)buffer = ';';
    buffer = sysColor(buffer + sizeof(char), background + 10);
    buffer = end(buffer);
    return (size_t)buffer - init;
}

size_t string_formatSystemColorMode(void *buffer, int foreground, int background, uint8_t flags)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    buffer = mode(buffer, flags);
    *(char *)buffer = ';';
    buffer = sysColor(buffer, foreground);
    *(char *)buffer = ';';
    buffer = sysColor(buffer + sizeof(char), background + 10);
    buffer = end(buffer);
    return (size_t)buffer - init;
}

size_t string_saveCursorPosition(void *buffer)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    *(char *)buffer = 's';
    return (size_t)buffer - init + sizeof(char);
}

size_t string_resetFormatting(void *buffer)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    *(char *)buffer = '0';
    buffer = end(buffer + sizeof(char));
    return (size_t)buffer - init;
}

size_t string_restoreCursorPosition(void *buffer)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    *(char *)buffer = 'u';
    return (size_t)buffer - init + sizeof(char);
}

size_t string_eraseEndOfLine(void *buffer)
{
    size_t init = (size_t)buffer;
    buffer = begin(buffer);
    *(char *)buffer = 'K';
    return (size_t)buffer - init + sizeof(char);
}

size_t string_setCursorPosition(void *buffer, int x, int y)
{
    char b[13];
    snprintf(b, 13 * sizeof(char), "\x1b[%d;%dH", y, x);
    size_t len = strlen(b);
    memcpy(buffer, b, len * sizeof(char));
    return len * sizeof(char);
}

size_t string_clearScreen(void *buffer)
{
    memcpy(buffer, "\x1b[2J\x1b[1;1H", 10 * sizeof(char));
    return 10 * sizeof(char);
}
