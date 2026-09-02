/*
 *
 * fetch tool for Linux systems
 * made for learning purposes.
 *
 * by terra2o (M. Taha Kircali), under GPLv3
 *
 */

#ifndef __linux__
#error "This program requires a Linux operating system to compile and run."
#endif

#include <stdio.h>
#include <sys/utsname.h>
#include <string.h>

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_WHITE   "\x1b[37m"
#define COLOR_RESET   "\x1b[0m"

char *get_distribution_name() {
    static char distro[256] = {0};
    FILE *fp = fopen("/etc/os-release", "r");
    if (!fp) {
        return "Unknown";
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "PRETTY_NAME=", 12) != 0) continue;

        char *start = line + 12;
        while (*start == ' ' || *start == '\t') start++;

        if (*start != '"') {
            fclose(fp);
            return "Unknown";
        }

        start++;

        char *end = strchr(start, '"');
        if (end) {
            *end = '\0';
            strcpy(distro, start);
        } else {
            fclose(fp);
            return "Unknown";
        }
        break;
    }
    fclose(fp);

    if (distro[0] == '\0') {
        return "Unknown";
    }

    return distro;
}
void print_info(struct utsname *sysinfo) {
    char *labels[] = {
        "Kernel version: ",
        "Kernel name: ",
        "Distribution: ",
        "Machine type: ",
        "Node name: "
    };

    char *values[] = {
        sysinfo->release,
        sysinfo->sysname,
        get_distribution_name(),
        sysinfo->machine,
        sysinfo->nodename
    };

    int max_label_len = 0;
    int max_value_len = 0;
    int top_border_size = 0;

    // don't forget to increment this number when you add a new thing
    for (int i = 0; i < 5; i++) {
        int label_len = strlen(labels[i]);
        int value_len = strlen(values[i]);
        if (label_len > max_label_len) max_label_len = label_len;
        if (value_len > max_value_len) max_value_len = value_len;
    }

    int content_width = max_label_len + max_value_len;
    int box_width = (content_width < 20) ? 20 : (content_width > 160) ? 160 : content_width;

    box_width += 2;

    printf("%s", COLOR_MAGENTA);
    printf("─");
    for (int i = 0; i < box_width; i++) {
        printf("─");
        top_border_size++;
    }
    printf("─\n%s", COLOR_RESET);

    for (int i = 0; i < 5; i++) {
        int label_len = strlen(labels[i]);
        int value_len = strlen(values[i]);

        printf("%s| %s", COLOR_MAGENTA, COLOR_RESET);

        printf("%s%s", COLOR_CYAN, labels[i]);

        printf("%s%s%s", COLOR_WHITE, values[i], COLOR_RESET);

        int chars_used = 3 + label_len + value_len; // 3 for " | " at start

        for (int j = 0; j < top_border_size - chars_used + 1; j++) {
            printf(" ");
        }

        printf("%s |\n%s", COLOR_MAGENTA, COLOR_RESET);
    }

    printf("%s", COLOR_MAGENTA);
    printf("─");
    for (int i = 0; i < box_width; i++) {
        printf("─");
    }
    printf("─\n%s", COLOR_RESET);
}

int main()
{
    struct utsname sysinfo;

    if (uname(&sysinfo) == 0) {
        print_info(&sysinfo);
    } else {
        perror("uname");
        return 1;
    }

    return 0;
}
