#include <string.h>
#include <sys/stat.h>
#include "fileUtils.h"

#define NORMAL_BUFFER __buffer__

static char __buffer__[256];

bool file_hidden(char *path)
{
    file_filename(path, NORMAL_BUFFER);
    return NORMAL_BUFFER[0] == '.';
}
void file_filename(char *path, char *buffer)
{
    size_t len = strlen(path);
    int i = len - 1;
    for (; i >= 0; --i)
        if (path[i] == '/')
            break;
    if (i != -1)
        memcpy(buffer, path + i + 1, len - i);
    else
        memcpy(buffer, path, len + 1);
}
void file_dirname(char *path, char *buffer)
{
    size_t len = strlen(path) - 1;
    int lastS = -1;
    for (int i = 0; i < len; i++)
        if (path[i] == '/')
            lastS = i;
    if (lastS == -1)
        *buffer = '\0';
    else
    {
        memcpy(buffer, path, lastS + 1);
        buffer[lastS + 1] = '\0';
    }
}
bool file_isDir(char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}
bool file_isLink(char *path)
{
    struct stat path_stat;
    lstat(path, &path_stat);
    return S_ISLNK(path_stat.st_mode);
}
int64_t hashSpecialChar(char *c)
{
    return ((int64_t)c[0] << 8) | (int64_t)c[1];
}
void simplifyString(char *str, char *buffer)
{
    while (true)
    {
        if (*str == '\0')
        {
            *buffer = '\0';
            break;
        }
        else if (*str >= 'A' && *str <= 'Z')
        {
            *buffer = *str - 'A' + 'a';
            buffer++;
            str++;
        }
        else if ((*str >= '0' && *str <= '9') || (*str >= 'a' && *str <= 'z'))
        {
            *buffer = *str;
            buffer++;
            str++;
        }
        else
        {
            int64_t hash = hashSpecialChar(str);
            if (hash == hashSpecialChar("é") ||
                hash == hashSpecialChar("è") ||
                hash == hashSpecialChar("ê") ||
                hash == hashSpecialChar("ë") ||
                hash == hashSpecialChar("É") ||
                hash == hashSpecialChar("Ê") ||
                hash == hashSpecialChar("Ë") ||
                hash == hashSpecialChar("È"))
            {
                *buffer = 'e';
                buffer++;
                str += 2;
            }
            else if (hash == hashSpecialChar("à") ||
                     hash == hashSpecialChar("ä") ||
                     hash == hashSpecialChar("â") ||
                     hash == hashSpecialChar("À") ||
                     hash == hashSpecialChar("Ä") ||
                     hash == hashSpecialChar("Â"))
            {
                *buffer = 'a';
                buffer++;
                str += 2;
            }
            else if (hash == hashSpecialChar("ò") ||
                     hash == hashSpecialChar("ö") ||
                     hash == hashSpecialChar("ô") ||
                     hash == hashSpecialChar("Ò") ||
                     hash == hashSpecialChar("Ö") ||
                     hash == hashSpecialChar("Ô"))
            {
                *buffer = 'o';
                buffer++;
                str += 2;
            }
            else if (hash == hashSpecialChar("ì") ||
                     hash == hashSpecialChar("ï") ||
                     hash == hashSpecialChar("î") ||
                     hash == hashSpecialChar("Ì") ||
                     hash == hashSpecialChar("Ï") ||
                     hash == hashSpecialChar("Î"))
            {
                *buffer = 'i';
                buffer++;
                str += 2;
            }
            else if (hash == hashSpecialChar("ù") ||
                     hash == hashSpecialChar("ü") ||
                     hash == hashSpecialChar("û") ||
                     hash == hashSpecialChar("Ù") ||
                     hash == hashSpecialChar("Ü") ||
                     hash == hashSpecialChar("Û"))
            {
                *buffer = 'u';
                buffer++;
                str += 2;
            }
            else
                str++;
        }
    }
}
bool stringStartWith(char *str, char *start)
{
    while (*str == *start)
    {
        if (*str == '\0')
            return true;
        ++str;
        ++start;
    }
    return *start == '\0';
}
void file_combine(char *dest, char *src)
{
    while (stringStartWith(src, "./"))
        src += 2;
    while (stringStartWith(src, "../"))
    {
        src += 3;
        file_dirname(dest, dest);
    }

    if (!strcmp(src, ".."))
    {
        file_dirname(dest, dest);
        return;
    }
    if (!strcmp(src, "."))
        return;
    strcat(dest, src);
}
void file_sort(char **list, size_t listSize)
{
    bool swapped;
    int max = listSize - 1;
    do
    {
        swapped = false;
        for (int i = 0; i < max; ++i)
        {
            char __buffer1[256], __buffer2[256];
            char *buffer1 = __buffer1, *buffer2 = __buffer2;
            if (list[i][0] == '.')
            {
                *buffer1 = '.';
                ++buffer1;
            }
            if (list[i + 1][0] == '.')
            {
                *buffer2 = '.';
                ++buffer2;
            }
            simplifyString(list[i], buffer1);
            simplifyString(list[i + 1], buffer2);
            if (strcmp(__buffer1, __buffer2) > 0)
            {
                swapped = true;
                char *tmp = list[i];
                list[i] = list[i + 1];
                list[i + 1] = tmp;
            }
        }
        --max;
    } while (swapped);
}
bool match(char *str, char *filter)
{
    char buffer[128];
    simplifyString(str, buffer);
    size_t index = 0;
    while (*filter != '\0')
    {
        if (buffer[index] == '\0')
            return false;
        else if (buffer[index] == *filter)
            ++filter;
        ++index;
    }
    return true;
}
size_t filterList(char **input, size_t n, char **output, char *filter)
{
    size_t count = 0;
    for (size_t i = 0; i < n; ++i)
        if (match(input[i], filter) || !strcmp(input[i], ".."))
            output[count++] = input[i];
    return count;
}
bool anyEntry(char **list, size_t n)
{
    for (int i = 0; i < n; ++i)
        if (strcmp(list[i], ".."))
            return true;
    return false;
}
