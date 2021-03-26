#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include "fileUtils.h"
#include "conManagement/consoleManagement.h"
#include "conManagement/stringAnsiManagement.h"
#include "aliases.h"

#define MAX_LINES_PER_PAGE __max_lines__

#define FOLDER_ICON "📁"
#define FILE_ICON "📄"
#define PARENT_ICON "↩️"

typedef struct dirent dirent;

void replaceStartingString(void *dest, char *src, char *pattern, char *override);
void applyAliases(void *dest, char *src, char **patterns, char **overrides, size_t count);

int main(int argc, char **argv)
{
    char *patterns[64];
    char *overrides[64];
    char *paths[2] = {
        "/etc/expdir/aliases"};
    {
        paths[1] = (char *)malloc(256 * sizeof(char));
        void *ptr = paths[1];
        ptr += string_write(ptr, getenv("HOME"));
        ptr += string_write(ptr, "/.config/expdir/aliases");
        *(char *)ptr = '\0';
    }
    int patternCount = parseAliases(paths, 2, patterns, overrides);
    switch (patternCount)
    {
    case -1:
        printf("ERROR:Missing '=' in the same line in aliases.\n");
        return -1;
    case -2:
        printf("ERROR:Too many '=' in the same line in aliases.\n");
        return -2;
    case -3:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat" //ignores the warning caused by %
        printf("ERROR:Missing closing '%' in the same line in aliases.\n");
#pragma GCC diagnostic pop
        return -3;
    case -4:
        printf("ERROR:Unknown environment variable.\n");
        return -4;
    }
    char *base_buffer = (char *)malloc(256 * sizeof(char));
    char *__dir__ = (char *)malloc(256 * sizeof(char));
    char *dir = __dir__;
    int __max_lines__;
    {
        struct winsize w;
        ioctl(0, TIOCGWINSZ, &w);
        __max_lines__ = w.ws_row - 3;
    }
    strcpy(dir, getenv("PWD"));
    {
        size_t dirLen = strlen(dir);
        dir[dirLen] = '/';
        dir[dirLen + 1] = '\0';
    }
    bool displayHidden = false;
    bool displayFiles = false;
    bool useEmojis = false;
    bool showHelp = false;
    int counter = -1;
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-' && argv[i][1] != '-')
        {
            size_t s = strlen(argv[i]);
            for (int j = 1; j < s; ++j)
            {
                if (argv[i][j] == 'a')
                    displayHidden = true;
                else if (argv[i][j] == 'f')
                    displayFiles = true;
                else if (argv[i][j] == 'c')
                    counter = i + 1;
                else if (argv[i][j] == 'e')
                    useEmojis = true;
                else if (argv[i][j] == 'h')
                    showHelp = true;
            }
        }
        else if (!strcmp(argv[i], "--all"))
            displayHidden = true;
        else if (!strcmp(argv[i], "--files"))
            displayFiles = true;
        else if (!strcmp(argv[i], "--count"))
            counter = i + 1;
        else if (!strcmp(argv[i], "--emojis"))
            useEmojis = true;
        else if (!strcmp(argv[i], "--help"))
            showHelp = true;
        else
        {
            if (*argv[i] != '/')
                file_combine(dir, argv[i]);
            else
                dir = argv[i];
            {
                size_t dirLen = strlen(dir);
                if (dir[dirLen - 1] != '/')
                {
                    dir[dirLen] = '/';
                    dir[dirLen + 1] = '\0';
                }
            }
        }
        if (counter == i + 1)
            ++i;
        if (showHelp)
        {
            printf(
                "Usage :\n\
    expdir [<options>]\n\
options :\n\
    -h, --help          displays this help panel\n\
    -a, --all           displays hidden entries\n\
    -f, --files         displays files\n\
    -e, --emojis        displays emojis icons\n\
    -c, --count <n>     change the number of lines displayed per page (max available by default)\n\
    <path>      start the browser in this directory\n");
            return 0;
        }
    }
    if (counter != -1)
        __max_lines__ = atoi(argv[counter]);
    file_dirname(*argv, base_buffer);
    chdir(base_buffer);
    bool fullRefresh = true;
    bool validate = false;
    char **folders = (char **)malloc(1024 * sizeof(char));
    folders[0] = NULL;
    size_t foldersCount = 0;
    char **files = (char **)malloc(1024 * sizeof(char));
    files[0] = NULL;
    size_t filesCount = 0;
    int selection;
    int page;
    int pagesCount;
    char *parentBuffer = (char *)malloc(256 * sizeof(char));
    char *_history = (char *)malloc(512 * sizeof(char));
    char *letterHistory;
    char *currParent = NULL;
    char *__consoleBuffer = (char *)malloc(1024 * 1024 * 2 * sizeof(char));
    while (!validate)
    {
        if (fullRefresh)
        {
            letterHistory = _history;
            fullRefresh = false;
            for (int i = 0; i < filesCount; ++i)
                free(files[i]);
            for (int i = 0; i < foldersCount; ++i)
                free(folders[i]);
            DIR *currentDir = opendir(dir);
            dirent *entry;
            foldersCount = 0;
            filesCount = 0;
            selection = -1;
            while ((entry = readdir(currentDir)))
            {
                if (entry->d_name[0] == '.' && entry->d_name[1] == '\0')
                    continue;
                if (!strcmp(entry->d_name, "..") && !strcmp(dir, "/"))
                    continue;
                strcpy(base_buffer, dir);
                file_combine(base_buffer, entry->d_name);
                if ((displayHidden || !file_hidden(base_buffer)) && file_isDir(base_buffer))
                {
                    if (currParent != NULL)
                        if (!strcmp(currParent, entry->d_name))
                            selection = foldersCount;
                    folders[foldersCount] = (char *)malloc(sizeof(char) * (strlen(entry->d_name) + 1));
                    strcpy(folders[foldersCount], entry->d_name);
                    ++foldersCount;
                }
                else if (displayFiles && (displayHidden || !file_hidden(base_buffer)) && !file_isDir(base_buffer))
                {
                    files[filesCount] = (char *)malloc(sizeof(char) * (strlen(entry->d_name) + 1));
                    strcpy(files[filesCount], entry->d_name);
                    ++filesCount;
                }
            }
            closedir(currentDir);
            file_sort(folders, foldersCount);
            file_sort(files, filesCount);
            pagesCount = (foldersCount + filesCount) / MAX_LINES_PER_PAGE;
            if ((foldersCount + filesCount) % MAX_LINES_PER_PAGE)
                pagesCount++;
            if (selection == -1)
                selection = !strcmp(folders[0], "..") && foldersCount > 1 ? 1 : 0;
        }
        page = selection / MAX_LINES_PER_PAGE;
        void *console_buffer = __consoleBuffer;
        console_buffer += string_resetFormatting(console_buffer);
        console_buffer += string_clearScreen(console_buffer);
        console_buffer += string_write(console_buffer, "Current directory : ");
        console_buffer += string_formatSystemForeground(console_buffer, CONSOLE_COLOR_BRIGHT_BLUE);
        {
            char __tmp[256];
            applyAliases(__tmp, dir, patterns, overrides, patternCount);
            console_buffer += string_write(console_buffer, __tmp);
        }
        console_buffer += string_resetFormatting(console_buffer);
        console_buffer += string_setCursorPosition(console_buffer, 1, 2);
        console_buffer += string_formatMode(console_buffer, CONSOLE_FLAG_UNDERLINE);
        console_buffer += snprintf(console_buffer, 200, "Page %d/%d            ", page + 1, pagesCount);
        console_buffer += string_resetFormatting(console_buffer);
        int displayedCount = (page + 1 == pagesCount) ? (foldersCount + filesCount) : MAX_LINES_PER_PAGE;
        if (displayedCount > MAX_LINES_PER_PAGE)
            displayedCount %= MAX_LINES_PER_PAGE;
        for (int i = page * MAX_LINES_PER_PAGE; i < page * MAX_LINES_PER_PAGE + displayedCount; ++i)
        {
            console_buffer += string_setCursorPosition(console_buffer, 1, 3 + i - page * MAX_LINES_PER_PAGE);
            if (i < foldersCount)
            {
                char __fullDir[256];
                strcpy(__fullDir, dir);
                file_combine(__fullDir, folders[i]);
                if (file_isLink(__fullDir))
                {
                    console_buffer += string_formatSystemForeground(console_buffer, CONSOLE_COLOR_BRIGHT_CYAN);
                    if (useEmojis)
                        console_buffer += string_write(console_buffer, FOLDER_ICON);
                    console_buffer += string_write(console_buffer, folders[i]);
                    console_buffer += string_resetFormatting(console_buffer);
                    console_buffer += string_formatSystemForeground(console_buffer, CONSOLE_COLOR_YELLOW);
                    console_buffer += string_write(console_buffer, " -> ");
                    console_buffer += string_formatSystemForeground(console_buffer, !access(__fullDir, R_OK) ? CONSOLE_COLOR_BRIGHT_GREEN : CONSOLE_COLOR_RED);
                    char __tmp1[256];
                    char __tmp2[256];
                    __tmp1[readlink(__fullDir, __tmp1, 256)] = '\0';
                    applyAliases(__tmp2, __tmp1, patterns, overrides, patternCount);
                    console_buffer += string_write(console_buffer, __tmp2);
                    console_buffer += string_resetFormatting(console_buffer);
                }
                else
                {
                    console_buffer += string_formatSystemForeground(console_buffer, !access(__fullDir, R_OK) ? CONSOLE_COLOR_WHITE : CONSOLE_COLOR_RED);
                    if (useEmojis)
                        console_buffer += string_write(console_buffer, strcmp(folders[i], "..") ? FOLDER_ICON : PARENT_ICON);
                    console_buffer += string_write(console_buffer, folders[i]);
                    console_buffer += string_resetFormatting(console_buffer);
                }
            }
            else
            {
                console_buffer += string_formatSystemForeground(console_buffer, CONSOLE_COLOR_BRIGHT_YELLOW);
                if (useEmojis)
                    console_buffer += string_write(console_buffer, FILE_ICON);
                console_buffer += string_write(console_buffer, files[i - foldersCount]);
                console_buffer += string_resetFormatting(console_buffer);
            }
        }
        console_buffer += string_setCursorPosition(console_buffer, 2, 3 + MAX_LINES_PER_PAGE);
        console_buffer += string_formatSystemForegroundMode(console_buffer, CONSOLE_COLOR_BRIGHT_GREEN, CONSOLE_FLAG_REVERSE_COLOR);
        console_buffer += string_write(console_buffer, "Space");
        console_buffer += string_resetFormatting(console_buffer);
        console_buffer += string_write(console_buffer, ":Open");
        console_buffer += string_setCursorPosition(console_buffer, 13, 3 + MAX_LINES_PER_PAGE);
        console_buffer += string_formatSystemForegroundMode(console_buffer, CONSOLE_COLOR_BRIGHT_RED, CONSOLE_FLAG_REVERSE_COLOR);
        console_buffer += string_write(console_buffer, "^X");
        console_buffer += string_resetFormatting(console_buffer);
        console_buffer += string_write(console_buffer, ":Cancel");
        console_buffer += string_setCursorPosition(console_buffer, 23, 3 + MAX_LINES_PER_PAGE);
        console_buffer += string_formatSystemForegroundMode(console_buffer, CONSOLE_COLOR_BRIGHT_YELLOW, CONSOLE_FLAG_REVERSE_COLOR);
        console_buffer += string_write(console_buffer, "Backspace");
        console_buffer += string_resetFormatting(console_buffer);
        console_buffer += string_write(console_buffer, ":Refresh");
        console_buffer += string_setCursorPosition(console_buffer, 41, 3 + MAX_LINES_PER_PAGE);
        console_buffer += string_formatSystemForegroundMode(console_buffer, CONSOLE_COLOR_BRIGHT_BLUE, CONSOLE_FLAG_REVERSE_COLOR);
        console_buffer += string_write(console_buffer, "Tab");
        console_buffer += string_resetFormatting(console_buffer);
        console_buffer += string_write(console_buffer, ":");
        if (!strcmp(dir, "/"))
            console_buffer += string_formatSystemForeground(console_buffer, CONSOLE_COLOR_BRIGHT_RED);
        console_buffer += string_write(console_buffer, "Parent");
        console_buffer += string_resetFormatting(console_buffer);
        bool refresh = false;
        while (!refresh)
        {
            console_buffer += string_setCursorPosition(console_buffer, 1, selection - page * MAX_LINES_PER_PAGE + 3);
            if (selection < foldersCount)
            {
                char __fullDir[256];
                strcpy(__fullDir, dir);
                file_combine(__fullDir, folders[selection]);
                if (file_isLink(__fullDir))
                {
                    console_buffer += string_formatSystemForegroundMode(console_buffer, CONSOLE_COLOR_BRIGHT_CYAN, CONSOLE_FLAG_REVERSE_COLOR);
                    if (useEmojis)
                        console_buffer += string_write(console_buffer, FOLDER_ICON);
                    console_buffer += string_write(console_buffer, folders[selection]);
                    console_buffer += string_resetFormatting(console_buffer);
                    console_buffer += string_formatSystemForeground(console_buffer, CONSOLE_COLOR_YELLOW);
                    console_buffer += string_write(console_buffer, " -> ");
                    console_buffer += string_formatSystemForeground(console_buffer, !access(__fullDir, R_OK) ? CONSOLE_COLOR_BRIGHT_GREEN : CONSOLE_COLOR_RED);
                    char __tmp1[256];
                    char __tmp2[256];
                    __tmp1[readlink(__fullDir, __tmp1, 256)] = '\0';
                    applyAliases(__tmp2, __tmp1, patterns, overrides, patternCount);
                    console_buffer += string_write(console_buffer, __tmp2);
                    console_buffer += string_resetFormatting(console_buffer);
                }
                else
                {
                    console_buffer += string_formatSystemForegroundMode(console_buffer, !access(__fullDir, R_OK) ? CONSOLE_COLOR_WHITE : CONSOLE_COLOR_RED, CONSOLE_FLAG_REVERSE_COLOR);
                    if (useEmojis)
                        console_buffer += string_write(console_buffer, strcmp(folders[selection], "..") ? FOLDER_ICON : PARENT_ICON);
                    console_buffer += string_write(console_buffer, folders[selection]);
                    console_buffer += string_resetFormatting(console_buffer);
                }
            }
            else
            {
                console_buffer += string_formatSystemForegroundMode(console_buffer, CONSOLE_COLOR_BRIGHT_YELLOW, CONSOLE_FLAG_REVERSE_COLOR);
                if (useEmojis)
                    console_buffer += string_write(console_buffer, FILE_ICON);
                console_buffer += string_write(console_buffer, files[selection - foldersCount]);
                console_buffer += string_resetFormatting(console_buffer);
            }
            console_buffer += string_setCursorPosition(console_buffer, 1, MAX_LINES_PER_PAGE + 4);
            *(char *)console_buffer = '\0';
            printf("%s", __consoleBuffer);
            console_buffer = __consoleBuffer;
            char key = getch();
            console_buffer += string_setCursorPosition(console_buffer, 1, selection - page * MAX_LINES_PER_PAGE + 3);
            if (selection < foldersCount)
            {
                char __fullDir[256];
                strcpy(__fullDir, dir);
                file_combine(__fullDir, folders[selection]);
                if (file_isLink(__fullDir))
                {
                    console_buffer += string_formatSystemForeground(console_buffer, CONSOLE_COLOR_BRIGHT_CYAN);
                    if (useEmojis)
                        console_buffer += string_write(console_buffer, FOLDER_ICON);
                    console_buffer += string_write(console_buffer, folders[selection]);
                    console_buffer += string_resetFormatting(console_buffer);
                    console_buffer += string_formatSystemForeground(console_buffer, CONSOLE_COLOR_YELLOW);
                    console_buffer += string_write(console_buffer, " -> ");
                    console_buffer += string_formatSystemForeground(console_buffer, !access(__fullDir, R_OK) ? CONSOLE_COLOR_BRIGHT_GREEN : CONSOLE_COLOR_RED);
                    char __tmp1[256];
                    char __tmp2[256];
                    __tmp1[readlink(__fullDir, __tmp1, 256)] = '\0';
                    applyAliases(__tmp2, __tmp1, patterns, overrides, patternCount);
                    console_buffer += string_write(console_buffer, __tmp2);
                    console_buffer += string_resetFormatting(console_buffer);
                }
                else
                {
                    console_buffer += string_formatSystemForeground(console_buffer, !access(__fullDir, R_OK) ? CONSOLE_COLOR_WHITE : CONSOLE_COLOR_RED);
                    if (useEmojis)
                        console_buffer += string_write(console_buffer, strcmp(folders[selection], "..") ? FOLDER_ICON : PARENT_ICON);
                    console_buffer += string_write(console_buffer, folders[selection]);
                    console_buffer += string_resetFormatting(console_buffer);
                }
            }
            else
            {
                console_buffer += string_formatSystemForeground(console_buffer, CONSOLE_COLOR_BRIGHT_YELLOW);
                if (useEmojis)
                    console_buffer += string_write(console_buffer, FILE_ICON);
                console_buffer += string_write(console_buffer, files[selection - foldersCount]);
                console_buffer += string_resetFormatting(console_buffer);
            }
            console_buffer += string_setCursorPosition(console_buffer, 1, MAX_LINES_PER_PAGE + 4);
            if (key == 27)
            {
                getch();
                switch (getch())
                {
                case 65:
                    if (selection > 0)
                        selection--;
                    if (selection % MAX_LINES_PER_PAGE == MAX_LINES_PER_PAGE - 1)
                        refresh = true;
                    break;
                case 66:
                    if (selection < foldersCount + filesCount - 1)
                        selection++;
                    if (selection % MAX_LINES_PER_PAGE == 0)
                        refresh = true;
                    break;
                case 68:
                    if (page > 0)
                    {
                        selection -= MAX_LINES_PER_PAGE;
                        refresh = true;
                    }
                    break;
                case 67:
                    if (page < pagesCount - 1)
                    {
                        selection += MAX_LINES_PER_PAGE;
                        if (selection > foldersCount + filesCount - 1)
                            selection = foldersCount + filesCount - 1;
                        refresh = true;
                    }
                    break;
                }
            }
            else if ((key == 10 && selection < foldersCount) || (key == 9 && !strcmp(folders[0], "..")))
            {
                char __fullDir[256];
                strcpy(__fullDir, dir);
                file_combine(__fullDir, folders[selection]);
                if (!access(__fullDir, R_OK))
                {
                    if (key == 9)
                        selection = 0;
                    fullRefresh = true;
                    refresh = true;
                    if (!strcmp(folders[0], ".."))
                    {
                        strcpy(base_buffer, dir);
                        base_buffer[strlen(base_buffer) - 1] = '\0';
                        file_filename(base_buffer, parentBuffer);
                        currParent = parentBuffer;
                    }
                    else
                        currParent = NULL;
                    snprintf(base_buffer, 256, "%s/", folders[selection]);
                    file_combine(dir, base_buffer);
                }
            }
            else if (key == 32)
            {

                refresh = true;
                validate = true;
                FILE *f = fopen("/var/cache/expdir/location", "w");
                fwrite(dir, sizeof(char) * strlen(dir), 1, f);
                fflush(f);
                fclose(f);
            }
            else if (key == 24)
            {

                refresh = true;
                validate = true;
            }
            else if (key == 127)
            {
                refresh = true;
                fullRefresh = true;
            }
            else if (key >= 'a' && key <= 'z')
            {
                *letterHistory = key;
                letterHistory++;
                *letterHistory = '\0';
                selection = listScore(folders, foldersCount, _history);
                if (page != selection / MAX_LINES_PER_PAGE)
                    refresh = true;
            }
        }
    }
    for (int i = 0; i < patternCount; ++i)
    {
        free(patterns[i]);
        free(overrides[i]);
    }
    {
        void *console_buffer = __consoleBuffer;
        console_buffer += string_clearScreen(console_buffer);
        *(char *)console_buffer = '\0';
        printf("%s", __consoleBuffer);
    }
    free(__consoleBuffer);
    free(folders);
    free(files);
    free(parentBuffer);
    free(_history);
    return 0;
}

void applyAliases(void *dest, char *src, char **patterns, char **overrides, size_t count)
{
    char __tmp1[256];
    strcpy(__tmp1, src);
    char __tmp2[256];
    for (int i = 0; i < count; ++i)
    {
        replaceStartingString(__tmp2, __tmp1, patterns[i], overrides[i]);
        strcpy(__tmp1, __tmp2);
    }
    strcpy(dest, __tmp1);
}

void replaceStartingString(void *dest, char *src, char *pattern, char *override)
{
    int patternSize = strlen(pattern);
    int srcSize = strlen(src);
    int patternCharMatch = 0;
    while (patternCharMatch < srcSize && src[patternCharMatch] == pattern[patternCharMatch])
    {
        if (patternCharMatch + 1 == patternSize)
        {
            src += patternCharMatch + 1;
            dest += string_write(dest, override);
            break;
        }
        ++patternCharMatch;
    }
    strcpy(dest, src);
}
