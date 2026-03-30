#define _GNU_SOURCE
#include "../include/cli.h"
#include "../include/install_system.h"
#include "../include/install_user.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void show_help(const char *prog_name) {
    printf("Usage: %s [OPTIONS] <package.deb>\n", prog_name);
    printf("\n");
    printf("A tool to install DEB packages system-wide or to user directory.\n");
    printf("\n");
    printf("Options:\n");
    printf("  -s, --system      Install system-wide using dpkg (requires sudo)\n");
    printf("  -u, --user        Install to user directory (~/.local)\n");
    printf("  -h, --help        Show this help message\n");
    printf("  -v, --version     Show version information\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s -s package.deb    # Install system-wide\n", prog_name);
    printf("  %s -u package.deb    # Install to ~/.local\n", prog_name);
    printf("  %s package.deb       # Default: ask user to choose\n", prog_name);
    printf("\n");
}

void show_version(void) {
    printf("debpkg version 1.0.0\n");
    printf("A simple DEB package installer\n");
}

int interactive_install(const char *deb_path) {
    char choice;
    
    printf("Choose installation method:\n");
    printf("  [1] System-wide (requires sudo, uses dpkg)\n");
    printf("  [2] User directory (~/.local, no root required)\n");
    printf("Enter your choice [1/2]: ");
    
    if (scanf(" %c", &choice) != 1) {
        print_error("Invalid input");
        return -1;
    }
    
    switch (choice) {
        case '1':
            return install_system(deb_path);
        case '2':
            return install_user(deb_path);
        default:
            print_error("Invalid choice");
            return -1;
    }
}
