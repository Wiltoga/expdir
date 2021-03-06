#include "aliases.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void trim(char *dest, char *src);

int parseAliases(char **files, int nbFiles, char **patterns, char **overrides)
{
    char __buffer[512];
    char __buffer2[512];
    char *line = __buffer;
    size_t n = 512;
    int count = 0;
    for (int currFile = 0; currFile < nbFiles; ++currFile)
    {
        FILE *f = fopen(files[currFile], "r");
        if (f == NULL)
            continue;
        while (getline(&line, &n, f) != -1)
        {
            char *tmp = __buffer2;
            trim(tmp, line);
            if (strlen(tmp) > 0 && tmp[0] != '#')
            {
                char __left[256], __right[256];
                char *left = __left, *right = __right;
                int i = 0;
                while (tmp[i] != '=')
                    if (tmp[i] == '\0')
                    {
                        fclose(f);
                        return -1;
                    }
                    else
                    {
                        *left = tmp[i++];
                        ++left;
                    }
                ++i;
                *left = '\0';
                while (tmp[i] != '\0')
                    if (tmp[i] == '=')
                    {
                        fclose(f);
                        return -2;
                    }
                    else
                    {
                        *right = tmp[i++];
                        ++right;
                    }
                *right = '\0';
                left = __left;
                right = __right;
                trim(tmp, left);
                strcpy(left, tmp);
                trim(tmp, right);
                strcpy(right, tmp);
                tmp = __buffer2;
                while (*left != '\0')
                {
                    if (*left == '%')
                    {
                        char __env[64];
                        char *env = __env;
                        ++left;
                        while (*left != '%')
                        {
                            if (*left == '\0')
                            {
                                fclose(f);
                                return -3;
                            }
                            *env = *left;
                            ++env;
                            ++left;
                        }
                        *env = '\0';
                        env = getenv(__env);
                        if (env == NULL)
                        {
                            fclose(f);
                            return -4;
                        }
                        while (*env != '\0')
                        {
                            *tmp = *env;
                            ++tmp;
                            ++env;
                        }
                    }
                    else
                    {
                        *tmp = *left;
                        ++tmp;
                    }
                    ++left;
                }
                *tmp = '\0';
                strcpy(__left, __buffer2);
                left = __left;
                tmp = __buffer2;
                while (*right != '\0')
                {
                    if (*right == '%')
                    {
                        char __env[64];
                        char *env = __env;
                        ++right;
                        while (*right != '%')
                        {
                            if (*right == '\0')
                            {
                                fclose(f);
                                return -3;
                            }
                            *env = *right;
                            ++env;
                            ++right;
                        }
                        *env = '\0';
                        env = getenv(__env);
                        if (env == NULL)
                        {
                            fclose(f);
                            return -4;
                        }
                        while (*env != '\0')
                        {
                            *tmp = *env;
                            ++tmp;
                            ++env;
                        }
                    }
                    else
                    {
                        *tmp = *right;
                        ++tmp;
                    }
                    ++right;
                }
                *tmp = '\0';
                strcpy(__right, __buffer2);
                right = __right;

                patterns[count] = (char *)malloc((strlen(left) + 1) * sizeof(char));
                strcpy(patterns[count], left);
                overrides[count] = (char *)malloc((strlen(right) + 1) * sizeof(char));
                strcpy(overrides[count], right);
                ++count;
            }
        }
        fclose(f);
    }
    return count;
}

void trim(char *dest, char *src)
{
    size_t beg = 0, end = strlen(src) - 1;
    while (src[beg] == ' ' ||
           src[beg] == '\t' ||
           src[beg] == '\r' ||
           src[beg] == '\n')
        ++beg;
    while ((src[end] == ' ' ||
            src[end] == '\t' ||
            src[end] == '\r' ||
            src[end] == '\n') &&
           end > 0)
        --end;
    memcpy(dest, src + beg, (end - beg + 1) * sizeof(char));
    dest[end - beg + 1] = '\0';
}