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
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <sys/statvfs.h>

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_WHITE   "\x1b[37m"
#define COLOR_RESET   "\x1b[0m"

int get_disk_str(const char *path, char *buf, size_t buf_len)
{
    struct statvfs vfs;
    if (statvfs(path, &vfs) != 0) return -1;

    unsigned long long total_bytes = (unsigned long long)vfs.f_blocks * vfs.f_frsize;
    unsigned long long free_bytes = (unsigned long long)vfs.f_bfree * vfs.f_frsize;
    unsigned long long used_bytes = total_bytes - free_bytes;

    double total_gb = (double)total_bytes / (1024 * 1024 * 1024);
    double used_gb = (double)used_bytes / (1024 * 1024 * 1024);

    return snprintf(buf, buf_len, "%.2fGB/%.2fGB", used_gb, total_gb);
}

int get_ram_str(char *buf, size_t buf_len)
{
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return -1;

    char line[128];
    unsigned long long total_kb = 0, avail_kb = 0;
    int found = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            total_kb = strtoull(line + 9, NULL, 10);
            found++;
        } else if (strncmp(line, "MemAvailable:", 13) == 0) {
            avail_kb = strtoull(line + 13, NULL, 10);
            found++;
        }
        if (found == 2) break;
    }
    fclose(fp);

    if (found < 2) return -1;

    double total_gb = (double)total_kb / (1024 * 1024);
    double used_gb = (double)(total_kb - avail_kb) / (1024 * 1024);

    return snprintf(buf, buf_len, "%.2fGB/%.2fGB", used_gb, total_gb);
}

char *get_distribution_name()
{
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

void print_info(struct utsname *buffer)
{
    char ram_buffer[1024];
    get_ram_str(ram_buffer, sizeof(ram_buffer));

    char disk_buffer[1024];
    get_disk_str("/", disk_buffer, sizeof(disk_buffer));

    char *labels[] = {
        "Kernel version: ",
        "Kernel name: ",
        "Distribution: ",
        "Machine type: ",
        "Node name: ",
        "RAM: ",
        "Disk: ",
    };

    char *values[] = {
        buffer->release,
        buffer->sysname,
        get_distribution_name(),
        buffer->machine,
        buffer->nodename,
        ram_buffer,
        disk_buffer,
    };

    int max_label_len = 0;
    int max_value_len = 0;
    int top_border_size = 0;

    // don't forget to increment this number when you add a new thing
    for (int i = 0; i < 7; i++) {
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

    // don't forget to increment this number when you add a new thing
    for (int i = 0; i < 7; i++) {
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
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        print_info(&buffer);
    } else {
        perror("uname");
        return 1;
    }

    return 0;
}
