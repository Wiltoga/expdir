#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include "fileUtils.h"
#include "conManagement/consoleManagement.h"
#include "conManagement/stringAnsiManagement.h"

#define MAX_LINES_PER_PAGE __max_lines__

typedef struct dirent dirent;

int main(int argc, char **argv)
{
    char base_buffer[256];
    char __dir__[256];
    char *dir = __dir__;
    int __max_lines__;
    {
        struct winsize w;
        ioctl(0, TIOCGWINSZ, &w);
        __max_lines__ = w.ws_row - 3;
    }
    getcwd(dir, 256);
    {
        size_t dirLen = strlen(dir);
        dir[dirLen] = '/';
        dir[dirLen + 1] = '\0';
    }
    bool displayHidden = false;
    bool displayFiles = false;
    for (int i = 1; i < argc; ++i)
        if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--all"))
            displayHidden = true;
        else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--files"))
            displayFiles = true;
        else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--count"))
            __max_lines__ = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
        {
            printf(
                "Usage :\n\
    expdir [<options>]\n\
options :\n\
    -a, --all           displays hidden entries\n\
    -f, --files         displays files\n\
    -c, --count <n>     change the number of lines displayed per page (max available by default)\n\
    <path>      start the browser in this directory\n");
            return 0;
        }
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
    file_dirname(*argv, base_buffer);
    chdir(base_buffer);
    bool fullRefresh = true;
    bool validate = false;
    char *folders[1024];
    folders[0] = NULL;
    size_t foldersCount = 0;
    char *files[1024];
    files[0] = NULL;
    size_t filesCount = 0;
    int selection;
    int page;
    int pagesCount;
    char parentBuffer[256];
    char _history[512];
    char *letterHistory;
    char *currParent = NULL;
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
        char *__consoleBuffer = malloc(1024 * 512);
        void *console_buffer = __consoleBuffer;
        console_buffer += string_resetFormatting(console_buffer);
        console_buffer += string_clearScreen(console_buffer);
        console_buffer += string_write(console_buffer, "Current directory : ");
        console_buffer += string_formatSystemForeground(console_buffer, CONSOLE_COLOR_BRIGHT_BLUE);
        console_buffer += string_write(console_buffer, dir);
        console_buffer += string_resetFormatting(console_buffer);
        console_buffer += string_setCursorPosition(console_buffer, 1, 2);
        console_buffer += string_formatMode(console_buffer, CONSOLE_FLAG_UNDERLINE);
        console_buffer += snprintf(console_buffer, 200, "Page %d/%d            ", page + 1, pagesCount);
        console_buffer += string_resetFormatting(console_buffer);
        int displayedCount = (page + 1 == pagesCount) ? (foldersCount + filesCount) % MAX_LINES_PER_PAGE : MAX_LINES_PER_PAGE;
        for (int i = page * MAX_LINES_PER_PAGE; i < page * MAX_LINES_PER_PAGE + displayedCount; ++i)
        {
            console_buffer += string_setCursorPosition(console_buffer, 1, 3 + i - page * MAX_LINES_PER_PAGE);
            if (i < foldersCount)
            {
                console_buffer += string_formatSystemForeground(console_buffer, CONSOLE_COLOR_WHITE);
                console_buffer += string_write(console_buffer, folders[i]);
                console_buffer += string_resetFormatting(console_buffer);
            }
            else
            {
                console_buffer += string_formatSystemForeground(console_buffer, CONSOLE_COLOR_BRIGHT_YELLOW);
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
        console_buffer += string_write(console_buffer, "^C");
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
                console_buffer += string_formatSystemForegroundMode(console_buffer, CONSOLE_COLOR_WHITE, CONSOLE_FLAG_REVERSE_COLOR);
                console_buffer += string_write(console_buffer, folders[selection]);
                console_buffer += string_resetFormatting(console_buffer);
            }
            else
            {
                console_buffer += string_formatSystemForegroundMode(console_buffer, CONSOLE_COLOR_BRIGHT_YELLOW, CONSOLE_FLAG_REVERSE_COLOR);
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
                console_buffer += string_formatSystemForeground(console_buffer, CONSOLE_COLOR_WHITE);
                console_buffer += string_write(console_buffer, folders[selection]);
                console_buffer += string_resetFormatting(console_buffer);
            }
            else
            {
                console_buffer += string_formatSystemForeground(console_buffer, CONSOLE_COLOR_BRIGHT_YELLOW);
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
            else if (key == 32)
            {

                refresh = true;
                validate = true;
                FILE *f = fopen("/var/cache/expdir/location", "w");
                fwrite(dir, sizeof(char) * strlen(dir), 1, f);
                fflush(f);
                fclose(f);
            }
            else if (key == 127)
            {
                refresh = true;
                fullRefresh = true;
            }
            else
            {
                *letterHistory = key;
                letterHistory++;
                *letterHistory = '\0';
                selection = listScore(folders, foldersCount, _history);
                if (page != selection / MAX_LINES_PER_PAGE)
                    refresh = true;
            }
        }
        free(__consoleBuffer);
    }
    return 0;
}
