#include <dirent.h>
#include <grp.h>
#include <langinfo.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

extern int optind;

static int compar(const void *a, const void *b)
{
    return strcmp(*(const char **) a, *(const char **) b);
}

static char get_type(mode_t mode)
{
    if (S_ISBLK(mode))
        return 'b';
    if (S_ISCHR(mode))
        return 'c';
    if (S_ISDIR(mode))
        return 'd';
    if (S_ISFIFO(mode))
        return 'p';
    if (S_ISLNK(mode))
        return 'l';
    if (S_ISSOCK(mode))
        return 's';
    return '-';
}

static char *const sperm(mode_t mode)
{
    static char perm[16] = { 0 };

    perm[0] = get_type(mode);
    perm[1] = (mode & S_IRUSR) == S_IRUSR ? 'r' : '-';
    perm[2] = (mode & S_IWUSR) == S_IWUSR ? 'w' : '-';
    perm[3] = (mode & S_IXUSR) == S_IXUSR ? 'x' : '-';
    perm[4] = (mode & S_IRGRP) == S_IRGRP ? 'r' : '-';
    perm[5] = (mode & S_IWGRP) == S_IWGRP ? 'w' : '-';
    perm[6] = (mode & S_IXGRP) == S_IXGRP ? 'x' : '-';
    perm[7] = (mode & S_IROTH) == S_IROTH ? 'r' : '-';
    perm[8] = (mode & S_IWOTH) == S_IWOTH ? 'w' : '-';
    perm[9] = (mode & S_IXOTH) == S_IXOTH ? 'x' : '-';

    return perm;
}

int main(int argc, char *argv[])
{
    const char *program = argv[0];

    int all = 0;
    int long_listing = 0;
    int addition = 1;

    int opt;
    while ((opt = getopt(argc, argv, "alr")) != -1) {
        switch (opt) {
        case 'a':
            all = 1;
            break;
        case 'l':
            long_listing = 1;
            break;
        case 'r':
            addition = -1;
            break;
        default:               /* ? */
            return -1;
        }
    }
    argc -= optind;
    argv += optind;

    for (; *argv; argv++) {
        int i;

        DIR *dirp;
        if ((dirp = opendir(*argv)) == NULL) {
            perror(program);
            continue;
        }

        int count = 0;
        const struct dirent *dirent;
        while ((dirent = readdir(dirp))) {
            count++;
        }

        const char **d_name =
            (const char **) malloc(sizeof(const char *) * count);
        rewinddir(dirp);

        for (i = 0; (dirent = readdir(dirp)); i++) {
            d_name[i] = dirent->d_name;
        }
        qsort(d_name, count, sizeof(const char *), compar);

        i = addition == 1 ? 0 : count - 1;
        int last = addition == 1 ? count : -1;
        for (; i != last; i += addition) {
            if (d_name[i][0] == '.' && !all) {
                continue;
            }
            if (long_listing) {
                char fullname[256];
                struct stat statbuf;
                struct passwd *passwdp;
                struct group *groupp;
                struct tm *tm;
                char datestring[256];

                sprintf(fullname, "%s/%s", *argv, d_name[i]);
                if (stat(fullname, &statbuf) == -1) {
                    perror(program);
                    continue;
                }

                printf("%s", sperm(statbuf.st_mode));
                printf(" %4ld", statbuf.st_nlink);

                if ((passwdp = getpwuid(statbuf.st_uid))) {
                    printf(" %-8.8s", passwdp->pw_name);
                } else {
                    printf(" %-8d", statbuf.st_uid);
                }

                if ((groupp = getgrgid(statbuf.st_gid))) {
                    printf(" %-8.8s", groupp->gr_name);
                } else {
                    printf(" %-8d", statbuf.st_gid);
                }

                printf(" %8ld", statbuf.st_size);

                tm = localtime(&statbuf.st_mtime);
                strftime(datestring, sizeof datestring,
                         nl_langinfo(D_T_FMT), tm);
                printf(" %s", datestring);

                printf(" %s\n", d_name[i]);
            } else {
                printf("%-20.20s", d_name[i]);
            }
        }
        if (!long_listing) {
            printf("\n");
        }

        if (closedir(dirp) == -1) {
            perror(program);
        }

        free(d_name);
    }

    return 0;
}
