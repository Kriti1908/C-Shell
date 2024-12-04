#include "headers.h"
#include "reveal.h"
#include "display_req.h"

int cmp(const void *a, const void *b) {                     
    dir_entry *entryA = (dir_entry *)a;
    dir_entry *entryB = (dir_entry *)b;
    return strcmp(entryA->entry->d_name, entryB->entry->d_name);
}

void print_permissions(struct stat fileStat) {                      // to print file permissions
    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
}

void print_file_details(char *directory, dir_entry *entry) {         // for deatils in -l
    print_permissions(entry->file_stat);
    printf(" %ld", entry->file_stat.st_nlink);

    struct passwd *pw = getpwuid(entry->file_stat.st_uid);
    struct group *gr = getgrgid(entry->file_stat.st_gid);
    printf(" %s %s", pw->pw_name, gr->gr_name);

    printf(" %5ld", entry->file_stat.st_size);

    char timebuf[80];
    struct tm *tm_info = localtime(&entry->file_stat.st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M ", tm_info);
    printf(" %s", timebuf);

    if (S_ISDIR(entry->file_stat.st_mode)) {
        printf("\033[1;34m%s\033[0m\n", entry->entry->d_name);      // Blue for directories
    } 
    else if (entry->file_stat.st_mode & S_IXUSR) {
        printf("\033[1;32m%s\033[0m\n", entry->entry->d_name);      // Green for executables
    }
    else {
        printf("\033[0m%s\033[0m\n", entry->entry->d_name);         // White for others
    }
}

void reveal(char* directory, int l, int a, char* lastdir_reveal) {
    struct dirent *entry;
    DIR *dp;
    int show_all = 0, long_format = 0;

    char curdir[5000];
    char path[5001];

    if (!getcwd(curdir, sizeof curdir)) {
        *curdir = '\0';
    }
    if (directory == NULL) {
        char cwd[2000];
        strcpy(cwd, getCWD(cwd));
        if (directory[1] == '/' || directory[1] == '\0') {
            snprintf(path, sizeof path, "%s%s", cwd, directory + 1);
            directory = path;
        } else {
            fprintf(stderr, "\033[1;31msyntax not supported: %s\033[0m\n", directory);
            return;
        }
    }
    if (!strcmp(directory, "-")) {
        if (*lastdir_reveal == '\0') {
            fprintf(stderr, "\033[1;31mno previous directory\033[0m\n");
            return;
        }
        directory = lastdir_reveal;
    } else if (*directory == '~') {
        char cwd[2000];
        strcpy(cwd, getCWD(cwd));
        if (directory[1] == '/' || directory[1] == '\0') {
            snprintf(path, sizeof path, "%s%s", cwd, directory + 1);
            directory = path;
        } else {
            fprintf(stderr, "\033[1;31msyntax not supported: %s\033[0m\n", directory);
            return;
        }
    } else if (*directory != '/') {
        snprintf(path, sizeof path, "%s/%s", curdir, directory);
        directory = path;
    }

    if(l) long_format = 1;
    if(a) show_all = 1;

    dp = opendir(directory);
    if (dp == NULL) {
        perror_red("opendir");
        return;
    }

    dir_entry *entries = NULL;      // all dirs in array
    size_t count = 0;
    while ((entry = readdir(dp)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }

        entries = realloc(entries, sizeof(dir_entry) * (count + 1));
        entries[count].entry = entry;

        char fullPath[5000];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", directory, entry->d_name);

        if (stat(fullPath, &entries[count].file_stat) == -1) {
            perror_red("stat");
            continue;
        }

        count++;
    }

    qsort(entries, count, sizeof(dir_entry), cmp);      // sort

    if(l) {
        printf("total %zu\n", count);
    }

    for (size_t i = 0; i < count; i++) {
        if (long_format) {
            print_file_details(directory, &entries[i]);
        } else {
            if (S_ISDIR(entries[i].file_stat.st_mode)) {
                printf("\033[34m%s\033[0m\n", entries[i].entry->d_name);        // Blue for directories
            } 
            else if (entries[i].file_stat.st_mode & S_IXUSR) {
                printf("\033[32m%s\033[0m\n", entries[i].entry->d_name);        // Green for executables
            }
            else {
                printf("\033[0m%s\033[0m\n", entries[i].entry->d_name);         // White for others
            }
        }
    }
    free(entries);
    closedir(dp);
}
