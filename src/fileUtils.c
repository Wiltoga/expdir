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
    return S_ISREG(path_stat.st_mode);
}
void lowerString(char *str, char *buffer)
{
    while (true)
    {
        if (*str >= 'A' && *str <= 'Z')
            *buffer = *str - 'A' + 'a';
        else
            *buffer = *str;
        if (*str == '\0')
            break;
        buffer++;
        str++;
    }
}
size_t listScore(char **list, size_t listSize, char *keyHistory)
{
    int bestId = -1;
    int bestVal = -1;
    size_t historyLen = strlen(keyHistory);
    for (int i = 0; i < listSize; ++i)
    {
        int score = 0;
        file_filename(list[i], NORMAL_BUFFER);
        lowerString(NORMAL_BUFFER, NORMAL_BUFFER);
        for (int j = historyLen - 1; j >= 0; --j)
            if (stringStartWith(NORMAL_BUFFER, keyHistory + j))
                score = historyLen - j;
        if (score > bestVal)
        {
            bestVal = score;
            bestId = i;
        }
    }
    return bestId;
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