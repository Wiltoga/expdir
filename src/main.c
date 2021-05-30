#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <time.h>
#include <ncurses.h>
#include <locale.h>
#include <errno.h>
#include <signal.h>
#include "fileUtils.h"
#include "aliases.h"

#define MAX_LINES_PER_PAGE __max_lines__
#define MAX_SEARCH_LENGTH 64
#define MIN_SEARCH_BAR_LENGTH 20
#define MAX_HITBOXES 64

#define FOLDER_ICON "📁"
#define FILE_ICON "📄"
#define PARENT_ICON "↩️"
#define LINK_ICON "🔗"
#define INVALID_ICON "❌"
#define FULL_SEPARATOR ""
#define SIMLPE_SEPARATOR ""
#define SEARCH "🔍"

#define FOLDER_COLOR 1
#define FOLDER_REVERSE_COLOR 2
#define FILE_COLOR 3
#define FILE_REVERSE_COLOR 4
#define DIRECTORY_BACK_COLOR 5
#define DIRECTORY_BACK_REVERSE_COLOR 6
#define OPERATOR_COLOR 7
#define LINK_COLOR 8
#define LINK_REVERSE_COLOR 9
#define TARGET_COLOR 10
#define INVALID_COLOR 11
#define INVALID_REVERSE_COLOR 12
#define NEUTRAL_SEARCH_COLOR 15
#define VALID_SEARCH_COLOR 16
#define INVALID_SEARCH_COLOR 17
#define NEUTRAL_SEARCH_REVERSE_COLOR 18

#define F5 269
#define UP_ARROW 259
#define DOWN_ARROW 258
#define LEFT_ARROW 260
#define RIGHT_ARROW 261
#define RETURN_KEY 10
#define TAB_KEY 9
#define SPACE_KEY 32
#define CTRL_X 24
#define BACKSPACE 263
#define CTRL_BACKSPACE 8

typedef struct dirent dirent;

typedef struct
{
    int width;
    int value;
} box_t;

void replaceStartingString(void *dest, char *src, char *pattern, char *override);
void applyAliases(void *dest, char *src, char **patterns, char **overrides, size_t count);
void displayFolder(char *folderName, char *dir, bool useEmojis, char **patterns, char **overrides, size_t patternCount, bool reverse);
void displayFile(char *fileName, bool useEmojis, bool reverse);
void emojify(char *src);
void fillLineBinary(int count);

//https://stackoverflow.com/a/1157217
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do
    {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

static bool refreshUI;
static bool cursorMoved;

void resize(int sig)
{
    refreshUI = true;
    endwin();
    refresh();
}

int main(int argc, char **argv)
{
    //initialization
    /***********************************************************************************/
    srand(time(NULL));
    signal(SIGWINCH, resize); //terminal resized event
    char *patterns[64];
    char *overrides[64];
    char *paths[2] = {//the paths to config files
                      "/etc/expdir/aliases"};
    {
        //we add the config file in the home folder
        paths[1] = (char *)malloc(256 * sizeof(char));
        void *ptr = paths[1];
        ptr += sizeof(char) * sprintf(ptr, "%s", getenv("HOME"));
        ptr += sizeof(char) * sprintf(ptr, "%s", "/.config/expdir/aliases");
        *(char *)ptr = '\0';
    }
    int patternCount = parseAliases(paths, 2, patterns, overrides);
    switch (patternCount) //errors handling
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
        printf("ERROR:Missing closing '\%' in the same line in aliases.\n");
#pragma GCC diagnostic pop
        return -3;
    case -4:
        printf("ERROR:Unknown environment variable.\n");
        return -4;
    }
    char *base_buffer = (char *)malloc(256 * sizeof(char)); //buffer used for temporary stuff
    char *__dir__ = (char *)malloc(256 * sizeof(char));     //current directory
    char *dir = __dir__;
    int __max_lines__; //max lines of files displayed per page
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
    bool invoked = false;

    // args parsing
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-' && argv[i][1] != '-') // args starting with a single '-'
        {
            size_t s = strlen(argv[i]);
            for (int j = 1; j < s; ++j)
            {
                if (argv[i][j] == 'a')
                    displayHidden = true;
                else if (argv[i][j] == 'f')
                    displayFiles = true;
                else if (argv[i][j] == 'v')
                    useEmojis = true;
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
        else if (!strcmp(argv[i], "--emojis"))
            useEmojis = true;
        else if (!strcmp(argv[i], "--visuals"))
            useEmojis = true;
        else if (!strcmp(argv[i], "--help"))
            showHelp = true;
        else if (!strcmp(argv[i], "invoke"))
            invoked = true;
        else
        { // no special arg, the starting directory
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
    }
    if (showHelp)
    {
        printf(
            "Usage :\n\
    expdir [<options>]\n\
options :\n\
    -h, --help          displays this help panel\n\
    -a, --all           displays hidden entries\n\
    -f, --files         displays files\n\
    -v, --visuals       displays more visual stuff\n\
    <path>              start the browser in this directory\n");
        return 0;
    }
    if (!invoked)
    {
        printf("ERROR:Please use 'expdir', not 'expdirapp'.\n");
        return -5;
    }
    // ncurses initialization
    setlocale(LC_ALL, "");
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(0);
    if (has_colors())
    {
        use_default_colors();
        start_color();
        init_color(COLOR_MAGENTA, 50 * 1000 / 255, 50 * 1000 / 255, 50 * 1000 / 255); //current directory back
        init_color(COLOR_YELLOW, 249 * 1000 / 255, 241 * 1000 / 255, 165 * 1000 / 255);
        init_color(COLOR_RED, 197 * 1000 / 255, 15 * 1000 / 255, 31 * 1000 / 255);
        init_color(COLOR_BLUE, 58 * 1000 / 255, 145 * 1000 / 255, 214 * 1000 / 255);
        init_color(COLOR_GREEN, 22 * 1000 / 255, 198 * 1000 / 255, 12 * 1000 / 255);
        init_color(COLOR_BLACK, 0 * 1000 / 255, 0 * 1000 / 255, 0 * 1000 / 255);
        init_color(COLOR_WHITE, 255 * 1000 / 255, 255 * 1000 / 255, 255 * 1000 / 255);
        init_color(COLOR_CYAN, 97 * 1000 / 255, 214 * 1000 / 255, 214 * 1000 / 255);

        init_pair(FOLDER_COLOR, COLOR_YELLOW, -1);
        init_pair(FOLDER_REVERSE_COLOR, COLOR_BLACK, COLOR_YELLOW);
        init_pair(FILE_COLOR, COLOR_WHITE, -1);
        init_pair(FILE_REVERSE_COLOR, COLOR_BLACK, COLOR_WHITE);
        init_pair(DIRECTORY_BACK_COLOR, COLOR_WHITE, COLOR_MAGENTA);
        init_pair(DIRECTORY_BACK_REVERSE_COLOR, COLOR_MAGENTA, -1);
        init_pair(OPERATOR_COLOR, COLOR_BLUE, -1);
        init_pair(TARGET_COLOR, COLOR_GREEN, -1);
        init_pair(LINK_COLOR, COLOR_CYAN, -1);
        init_pair(LINK_REVERSE_COLOR, COLOR_BLACK, COLOR_CYAN);
        init_pair(INVALID_COLOR, COLOR_RED, -1);
        init_pair(INVALID_REVERSE_COLOR, COLOR_WHITE, COLOR_RED);
        init_pair(NEUTRAL_SEARCH_COLOR, COLOR_BLACK, COLOR_WHITE);
        init_pair(VALID_SEARCH_COLOR, COLOR_GREEN, COLOR_WHITE);
        init_pair(INVALID_SEARCH_COLOR, COLOR_RED, COLOR_WHITE);
        init_pair(NEUTRAL_SEARCH_REVERSE_COLOR, COLOR_WHITE, -1);
    }
    else
    {
        endwin();
        printf("Err : No color support.");
        exit(100);
    }
    //we move the the dir of the program
    file_dirname(*argv, base_buffer);
    chdir(base_buffer);
    bool fullRefresh = true;                                       //parse the directory
    bool validate = false;                                         //end the program
    char **folders = (char **)malloc(1024 * sizeof(char));         //null terminated list of all folders
    char **filteredFolders = (char **)malloc(1024 * sizeof(char)); //null terminated list of all folders, after filter
    folders[0] = NULL;
    size_t foldersCount = 0;
    char **files = (char **)malloc(1024 * sizeof(char));         //null terminated list of all files
    char **filteredFiles = (char **)malloc(1024 * sizeof(char)); //null terminated list of all files, after filter
    files[0] = NULL;
    size_t filesCount = 0;
    box_t hitboxes[MAX_HITBOXES];              //list of hitboxes for the mouse (the hitboxes of the entries)
    int selection;                             //currently selection entry
    int page;                                  //current page
    int pagesCount;                            //total pages
    char searchHistory[MAX_SEARCH_LENGTH + 1]; //search bar content
    bool searchChanged = false;                //true when the seachbar has changed

    //main loop
    /****************************************************************************/
    while (!validate)
    {
        if (fullRefresh)
        {
            searchHistory[0] = '\0'; //we reset the searchbar
            searchChanged = true;
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
        }
        __max_lines__ = LINES - 3; //3 lines are occupied by the UI
        size_t filteredFoldersCount = filterList(folders, foldersCount, filteredFolders, searchHistory);
        size_t filteredFilesCount = filterList(files, filesCount, filteredFiles, searchHistory);
        if (searchChanged)
        {
            //smart search
            for (size_t i = 0; i < filteredFoldersCount; ++i)
            {
                char simplified[128];
                simplifyString(filteredFolders[i], simplified);
                if (stringStartWith(simplified, searchHistory))
                {
                    selection = i;
                    break;
                }
            }
            if (selection >= filteredFoldersCount || !strcmp(filteredFolders[selection], ".."))
                selection = anyEntry(filteredFolders, filteredFoldersCount) ? 1 : 0;
            searchChanged = false;
        }
        pagesCount = (filteredFoldersCount + filteredFilesCount) / MAX_LINES_PER_PAGE;
        if ((filteredFoldersCount + filteredFilesCount) % MAX_LINES_PER_PAGE)
            pagesCount++; //one more page for the leftovers
        if (selection == -1)
            selection = !strcmp(filteredFolders[0], "..") && filteredFoldersCount > 1 ? 1 : 0;
        page = selection / MAX_LINES_PER_PAGE;
        move(0, 0);
        printw("Current directory : ");
        {
            char __tmp[256];
            applyAliases(__tmp, dir, patterns, overrides, patternCount);
            if (useEmojis)
                emojify(__tmp);
            else
            {
                attron(COLOR_PAIR(DIRECTORY_BACK_COLOR));
                printw("%s", __tmp);
                attroff(COLOR_PAIR(DIRECTORY_BACK_COLOR));
            }
        }
        clrtoeol();
        move(1, 0);
        attron(A_UNDERLINE);
        printw("Page %d/%d            ", page + 1, pagesCount);
        attroff(A_UNDERLINE);
        move(1, 14);
        attron(COLOR_PAIR(NEUTRAL_SEARCH_COLOR));
        printw(" %s", useEmojis ? SEARCH " " : "Search:");
        attroff(COLOR_PAIR(NEUTRAL_SEARCH_COLOR));
        int searchColor = searchHistory[0] == '\0' || anyEntry(filteredFolders, filteredFoldersCount) ? VALID_SEARCH_COLOR : INVALID_SEARCH_COLOR;
        attron(COLOR_PAIR(searchColor));
        attron(A_UNDERLINE);
        printw("%s", searchHistory);
        size_t searchLen = strlen(searchHistory);
        if (searchLen < MIN_SEARCH_BAR_LENGTH)
            for (int i = searchLen; i < MIN_SEARCH_BAR_LENGTH; ++i)
                printw(" ");
        attroff(A_UNDERLINE);
        attroff(COLOR_PAIR(searchColor));
        attron(COLOR_PAIR(NEUTRAL_SEARCH_COLOR));
        printw(" ");
        attroff(COLOR_PAIR(NEUTRAL_SEARCH_COLOR));
        if (useEmojis)
        {
            attron(COLOR_PAIR(NEUTRAL_SEARCH_REVERSE_COLOR));
            printw(FULL_SEPARATOR);
            attroff(COLOR_PAIR(NEUTRAL_SEARCH_REVERSE_COLOR));
        }
        clrtoeol();
        int displayedCount = pagesCount == 0 ? 0 : (page + 1 == pagesCount) ? (filteredFoldersCount + filteredFilesCount) : MAX_LINES_PER_PAGE; //only god knows this
        if (displayedCount > MAX_LINES_PER_PAGE)
            displayedCount %= MAX_LINES_PER_PAGE;
        if (displayedCount > 0)
            for (int i = page * MAX_LINES_PER_PAGE; i < page * MAX_LINES_PER_PAGE + displayedCount; ++i)
            {
                move(2 + i - page * MAX_LINES_PER_PAGE, 0);
                box_t *hitbox = &hitboxes[i - page * MAX_LINES_PER_PAGE];
                hitbox->value = i;
                if (i < filteredFoldersCount)
                {
                    displayFolder(filteredFolders[i], dir, useEmojis, patterns, overrides, patternCount, false);
                    hitbox->width = (useEmojis ? 4 : 0) + strlen(filteredFolders[i]);
                }
                else
                {
                    displayFile(filteredFiles[i - filteredFoldersCount], useEmojis, false);
                    hitbox->width = (useEmojis ? 4 : 0) + strlen(filteredFiles[i - filteredFoldersCount]);
                }
                clrtoeol();
            }
        for (int i = displayedCount; i < MAX_LINES_PER_PAGE; ++i)
        {
            move(2 + i, 0);
            clrtoeol();
        }
        for (int i = displayedCount; i < MAX_HITBOXES; ++i)
            hitboxes[i].value = -1;
        move(2 + MAX_LINES_PER_PAGE, 1);
        attron(COLOR_PAIR(DIRECTORY_BACK_COLOR));
        printw("Space");
        attroff(COLOR_PAIR(DIRECTORY_BACK_COLOR));
        printw(":Open  ");
        attron(COLOR_PAIR(DIRECTORY_BACK_COLOR));
        printw("^X");
        attroff(COLOR_PAIR(DIRECTORY_BACK_COLOR));
        printw(":Cancel  ");
        attron(COLOR_PAIR(DIRECTORY_BACK_COLOR));
        printw("F5");
        attroff(COLOR_PAIR(DIRECTORY_BACK_COLOR));
        printw(":Refresh  ");
        attron(COLOR_PAIR(DIRECTORY_BACK_COLOR));
        printw("Tab");
        attroff(COLOR_PAIR(DIRECTORY_BACK_COLOR));
        printw(":");
        if (!strcmp(dir, "/"))
            attron(COLOR_PAIR(INVALID_COLOR));
        printw("Parent");
        if (!strcmp(dir, "/"))
            attroff(COLOR_PAIR(INVALID_COLOR));
        clrtoeol();
        refreshUI = false;
        cursorMoved = true;
        int oldSelection = selection;
        int mouseClickedSelection = -1;
        while (!refreshUI)
        {
            //smaller loop, for the cursor moving without changing page/dir
            /*****************************************************************************/
            if (cursorMoved)
            {
                if (filteredFoldersCount + filteredFilesCount > 0)
                {
                    move(oldSelection - page * MAX_LINES_PER_PAGE + 2, 0);
                    if (oldSelection < filteredFoldersCount)
                        displayFolder(filteredFolders[oldSelection], dir, useEmojis, patterns, overrides, patternCount, false);
                    else
                        displayFile(filteredFiles[oldSelection - filteredFoldersCount], useEmojis, false);
                    clrtoeol();
                }
                if (filteredFoldersCount + filteredFilesCount > 0)
                {
                    move(selection - page * MAX_LINES_PER_PAGE + 2, 0);
                    if (selection < filteredFoldersCount)
                        displayFolder(filteredFolders[selection], dir, useEmojis, patterns, overrides, patternCount, true);
                    else
                        displayFile(filteredFiles[selection - filteredFoldersCount], useEmojis, true);
                    clrtoeol();
                }
                move(LINES - 1, 0);
                refresh();
                cursorMoved = false;
            }
            if (selection + oldSelection)
                oldSelection = selection;
            int key = getch();
            if (key == UP_ARROW)
            {
                if (selection > 0)
                    selection--;
                if (selection % MAX_LINES_PER_PAGE == MAX_LINES_PER_PAGE - 1)
                    refreshUI = true;
                cursorMoved = true;
            }
            else if (key == DOWN_ARROW)
            {
                if (selection < filteredFoldersCount + filteredFilesCount - 1)
                    selection++;
                if (selection % MAX_LINES_PER_PAGE == 0)
                    refreshUI = true;
                cursorMoved = true;
            }
            else if (key == LEFT_ARROW)
            {
                if (page > 0)
                {
                    selection -= MAX_LINES_PER_PAGE;
                    refreshUI = true;
                }
                cursorMoved = true;
            }
            else if (key == RIGHT_ARROW)
            {
                if (page < pagesCount - 1)
                {
                    selection += MAX_LINES_PER_PAGE;
                    if (selection > filteredFoldersCount + filteredFilesCount - 1)
                        selection = filteredFoldersCount + filteredFilesCount - 1;
                    refreshUI = true;
                }
                cursorMoved = true;
            }
            else if ((key == RETURN_KEY && selection < filteredFoldersCount) || (key == TAB_KEY && !strcmp(folders[0], "..")))
            {
                char __fullDir[256];
                strcpy(__fullDir, dir);
                file_combine(__fullDir, selection < filteredFoldersCount ? filteredFolders[selection] : "..");
                if (!access(__fullDir, R_OK) || (key == TAB_KEY && !strcmp(folders[0], "..")))
                {
                    if (key == TAB_KEY)
                        selection = 0;
                    fullRefresh = true;
                    refreshUI = true;
                    if (!strcmp(folders[0], ".."))
                    {
                        strcpy(base_buffer, dir);
                        base_buffer[strlen(base_buffer) - 1] = '\0';
                    }
                    sprintf(base_buffer, "%s/", filteredFolders[selection]);
                    file_combine(dir, base_buffer);
                }
            }
            else if (key == SPACE_KEY)
            {
                refreshUI = true;
                validate = true;
                FILE *f = fopen("/var/cache/expdir/location", "w");
                fwrite(dir, sizeof(char) * strlen(dir), 1, f);
                fflush(f);
                fclose(f);
            }
            else if (key == CTRL_X)
            {
                refreshUI = true;
                validate = true;
            }
            else if (key == F5)
            {
                refreshUI = true;
                fullRefresh = true;
            }
            else if (key == BACKSPACE)
            {
                size_t historyLen = strlen(searchHistory);
                if (historyLen > 0)
                {
                    searchHistory[historyLen - 1] = '\0';
                    refreshUI = true;
                    searchChanged = true;
                }
            }
            else if (key == CTRL_BACKSPACE)
            {
                searchHistory[0] = '\0';
                refreshUI = true;
                searchChanged = true;
            }
            else if (key == ERR)
                msleep(20);
            else if (key == KEY_MOUSE)
            {
                MEVENT event;
                if (getmouse(&event) == OK)
                {
                    if (event.bstate & BUTTON1_PRESSED)
                    {
                        if (event.y > 1 && event.y - 2 < MAX_HITBOXES && hitboxes[event.y - 2].value != -1 && hitboxes[event.y - 2].width >= event.x)
                        {
                            if (mouseClickedSelection == hitboxes[event.y - 2].value && selection < filteredFoldersCount)
                            {
                                char __fullDir[256];
                                strcpy(__fullDir, dir);
                                file_combine(__fullDir, filteredFolders[selection]);
                                if (!access(__fullDir, R_OK))
                                {
                                    fullRefresh = true;
                                    refreshUI = true;
                                    sprintf(base_buffer, "%s/", filteredFolders[selection]);
                                    file_combine(dir, base_buffer);
                                }
                            }
                            selection = hitboxes[event.y - 2].value;
                            mouseClickedSelection = selection;
                            cursorMoved = true;
                        }
                    }
                    else if (event.bstate & BUTTON3_PRESSED)
                    {
                        char __fullDir[256];
                        strcpy(__fullDir, dir);
                        file_combine(__fullDir, "..");
                        if (!strcmp(folders[0], ".."))
                        {
                            selection = 0;
                            fullRefresh = true;
                            refreshUI = true;
                            if (!strcmp(folders[0], ".."))
                            {
                                strcpy(base_buffer, dir);
                                base_buffer[strlen(base_buffer) - 1] = '\0';
                            }
                            sprintf(base_buffer, "%s/", filteredFolders[selection]);
                            file_combine(dir, base_buffer);
                        }
                    }
                    else if (event.bstate & BUTTON4_PRESSED) //mouse wheel up
                    {
                        if (page > 0)
                        {
                            selection -= MAX_LINES_PER_PAGE;
                            refreshUI = true;
                        }
                        cursorMoved = true;
                    }
                    else if (event.bstate & BUTTON5_PRESSED) //mouse wheel down
                    {
                        if (page < pagesCount - 1)
                        {
                            selection += MAX_LINES_PER_PAGE;
                            if (selection > filteredFoldersCount + filteredFilesCount - 1)
                                selection = filteredFoldersCount + filteredFilesCount - 1;
                            refreshUI = true;
                        }
                        cursorMoved = true;
                    }
                }
            }
            else
            {
                char original[2] = " ", simple[2];
                original[0] = key;
                simplifyString(original, simple);
                key = simple[0];
                if ((key >= 'a' && key <= 'z') || (key >= '0' && key <= '9'))
                {
                    size_t historyLen = strlen(searchHistory);
                    if (historyLen < MAX_SEARCH_LENGTH)
                    {
                        searchHistory[historyLen] = key;
                        searchHistory[historyLen + 1] = '\0';
                        refreshUI = true;
                        searchChanged = true;
                    }
                }
            }
            if (filteredFoldersCount + filteredFilesCount == 0)
                selection = 0;
        }
    }
    for (int i = 0; i < patternCount; ++i)
    {
        free(patterns[i]);
        free(overrides[i]);
    }
    clear();
    free(folders);
    free(files);
    free(filteredFolders);
    free(filteredFiles);
    endwin();
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
            dest += sizeof(char) * sprintf(dest, "%s", override);
            break;
        }
        ++patternCharMatch;
    }
    strcpy(dest, src);
}

void displayFile(char *fileName, bool useEmojis, bool reverse)
{
    int fileColor = reverse ? FILE_REVERSE_COLOR
                            : FILE_COLOR;
    attron(COLOR_PAIR(fileColor));
    if (useEmojis)
        printw(FILE_ICON);
    printw("%s", fileName);
    if (useEmojis && reverse)
    {
        printw(" ");
        attroff(COLOR_PAIR(fileColor));
        attron(COLOR_PAIR(FILE_COLOR));
        printw(FULL_SEPARATOR);
        attroff(COLOR_PAIR(FILE_COLOR));
    }
    else
        attroff(COLOR_PAIR(fileColor));
    clrtoeol();
}

void displayFolder(char *folderName, char *dir, bool useEmojis, char **patterns, char **overrides, size_t patternCount, bool reverse)
{
    char __fullDir[256];
    strcpy(__fullDir, dir);
    file_combine(__fullDir, folderName);
    if (file_isLink(__fullDir))
    {
        int linkColor = reverse ? LINK_REVERSE_COLOR : LINK_COLOR;
        attron(COLOR_PAIR(linkColor));
        if (useEmojis)
            printw(!access(__fullDir, R_OK) ? LINK_ICON : INVALID_ICON);
        printw("%s", folderName);
        if (!useEmojis)
            printw("/");
        else if (useEmojis && reverse)
        {
            printw(" ");
            attroff(COLOR_PAIR(linkColor));
            attron(COLOR_PAIR(LINK_COLOR));
            printw(FULL_SEPARATOR);
            attroff(COLOR_PAIR(LINK_COLOR));
        }
        else
        {
            printw("  ");
            attroff(COLOR_PAIR(linkColor));
        }
        attron(COLOR_PAIR(OPERATOR_COLOR));
        printw(" -> ");
        attroff(COLOR_PAIR(OPERATOR_COLOR));
        int targetColor = !access(__fullDir, R_OK) ? TARGET_COLOR : INVALID_COLOR;
        attron(COLOR_PAIR(targetColor));
        attron(A_UNDERLINE);
        char __tmp1[256];
        char __tmp2[256];
        __tmp1[readlink(__fullDir, __tmp1, 256)] = '\0';
        applyAliases(__tmp2, __tmp1, patterns, overrides, patternCount);
        printw("%s", __tmp2);
        printw("/");
        attroff(A_UNDERLINE);
        attroff(COLOR_PAIR(targetColor));
    }
    else
    {
        int folderColor = reverse ? !access(__fullDir, R_OK) ? FOLDER_REVERSE_COLOR
                                                             : INVALID_REVERSE_COLOR
                                  : !access(__fullDir, R_OK) ? FOLDER_COLOR
                                                             : INVALID_COLOR;
        attron(COLOR_PAIR(folderColor));
        if (useEmojis)
            printw("%s", strcmp(folderName, "..") ? !access(__fullDir, R_OK) ? FOLDER_ICON : INVALID_ICON : PARENT_ICON);
        printw("%s", folderName);
        if (!useEmojis)
        {
            printw("/");
            attroff(COLOR_PAIR(folderColor));
        }
        else if (useEmojis && reverse)
        {
            printw(" ");
            attroff(COLOR_PAIR(folderColor));
            folderColor = !access(__fullDir, R_OK) ? FOLDER_COLOR : INVALID_COLOR;
            attron(COLOR_PAIR(folderColor));
            printw(FULL_SEPARATOR);
            attroff(COLOR_PAIR(folderColor));
        }
        else
            attroff(COLOR_PAIR(folderColor));
    }
    clrtoeol();
}

void emojify(char *src)
{
    while (*src == '/')
        ++src;
    attron(COLOR_PAIR(DIRECTORY_BACK_COLOR));
    if (*src == '\0')
        printw(" / ");
    else
    {
        char __tmp[128];
        bool first = true;
        while (*src != '\0')
        {
            char *folderName = __tmp;
            while (*src != '/')
            {
                *folderName = *src;
                src += sizeof(char);
                folderName += sizeof(char);
            }
            ++src;
            *folderName = '\0';
            if (!first)
                printw(SIMLPE_SEPARATOR);
            else
                first = false;
            printw(" %s ", __tmp);
        }
    }
    attroff(COLOR_PAIR(DIRECTORY_BACK_COLOR));
    attron(COLOR_PAIR(DIRECTORY_BACK_REVERSE_COLOR));
    printw(FULL_SEPARATOR);
    attroff(COLOR_PAIR(DIRECTORY_BACK_REVERSE_COLOR));
}

void fillLineBinary(int count)
{
    for (int i = 0; i < count; ++i)
        addch('0' + (rand() % 2));
}