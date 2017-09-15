// myls.c ... my very own "ls" implementation

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAXDIRNAME 100
//#define MAXFNAME   200
#define MAXNAME    20

#define MODE_LEN 9

char *rwxmode(mode_t, char *);
char *username(uid_t, char *);
char *groupname(gid_t, char *);

int main(int argc, char *argv[])
{
    // string buffers for various names
    char dirname[MAXDIRNAME];
    char uname[MAXNAME+1];
    char gname[MAXNAME+1];
    char mode[MAXNAME+1];

    // collect the directory name, with "." as default
    if (argc < 2)
        strlcpy(dirname, ".", MAXDIRNAME);
    else
        strlcpy(dirname, argv[1], MAXDIRNAME);

    // check that the name really is a directory
    struct stat info;
    if (stat(dirname, &info) < 0)
    { perror(argv[0]); exit(EXIT_FAILURE); }
    if ((info.st_mode & S_IFMT) != S_IFDIR)
    { fprintf(stderr, "%s: Not a directory\n",argv[0]); exit(EXIT_FAILURE); }

    // open the directory to start reading
    DIR *df = opendir(dirname);

    // read directory entries
    struct dirent *entry; // UNCOMMENT this line
    while((entry = readdir(df))) {
        if(entry->d_name[0] == '.') continue;

        lstat(entry->d_name, &info);

        switch (entry->d_type) {
            case DT_REG: printf("-");   break;
            case DT_DIR: printf("d");   break;
            case DT_LNK: printf("l");   break;
            default: printf("?");
        }

        printf("%s  %-8.8s %-8.8s %8lld  %s\n",
                rwxmode(info.st_mode, mode),
                username(info.st_uid, uname),
                groupname(info.st_gid, gname),
                (long long)info.st_size,
                entry->d_name);
    }

    // finish up
    // closedir(df); // UNCOMMENT this line
    return EXIT_SUCCESS;
}

// convert octal mode to -rwxrwxrwx string
char *rwxmode(mode_t mode, char *str)
{
    for(int i = 0; i < MODE_LEN; i += 3) {
        for(int j = 0; j < 3; j++) {
            //printf("i: %d, j: %d, 1 << i << j: %d, MODE_LEN - i - j: %d\n", i, j, ((1 << i) << j), MODE_LEN - i - j);
            int idx = MODE_LEN - i - j - 1;
            if((mode & ((1 << i) << j)) != 0) {
                if(j == 0) {
                    str[idx] = 'x';
                } else if(j == 1) {
                    str[idx] = 'w';
                } else {
                    str[idx] = 'r';
                }
            } else {
                str[idx] = '-';
            }
        }
    }
    return str;
}

// convert user id to user name
char *username(uid_t uid, char *name)
{
    struct passwd *uinfo = getpwuid(uid);
    if (uinfo == NULL)
        snprintf(name, MAXNAME, "%d?", (int)uid);
    else
        snprintf(name, MAXNAME, "%s", uinfo->pw_name);
    return name;
}

// convert group id to group name
char *groupname(gid_t gid, char *name)
{
    struct group *ginfo = getgrgid(gid);
    if (ginfo == NULL)
        snprintf(name, MAXNAME, "%d?", (int)gid);
    else
        snprintf(name, MAXNAME, "%s", ginfo->gr_name);
    return name;
}
