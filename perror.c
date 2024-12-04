#include "headers.h"

void perror_red(const char *message) {
    const char *red_start = "\033[1;31m";
    const char *color_reset = "\033[0m";

    fprintf(stderr, "%s", red_start);
    perror(message);
    fprintf(stderr, "%s", color_reset);
}