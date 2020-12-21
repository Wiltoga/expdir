#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "fileUtils.h"

int main(int argc, char **argv)
{
    char base_buffer[256];
    char __dir__[256];
    char *dir = __dir__;
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
        else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
        {
            printf(
                "Usage :\n\
    expdir [<options>]\n\
options :\n\
    -a          displays hidden files & folders\n\
    -f          displays files\n\
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
    printf("%s\n", dir);

    return 0;
}