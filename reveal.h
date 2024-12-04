#ifndef REVEAL_H
#define REVEAL_H

int compare_entries(const void *a, const void *b);
void print_permissions(struct stat fileStat);
void print_file_details(char *directory, dir_entry *entry);
void reveal(char* directory, int l, int a, char* lastdir_reveal);

#endif