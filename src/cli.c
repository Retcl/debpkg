/*
 * debpkg - A simple DEB package installer
 * Copyright (C) 2024  debpkg Authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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
    printf("Usage: %s <COMMAND> [OPTIONS] <ARGUMENT>\n", prog_name);
    printf("\n");
    printf("A tool to install/uninstall DEB packages system-wide or to user directory.\n");
    printf("\n");
    printf("Commands:\n");
    printf("  install <package.deb>     Install a DEB package\n");
    printf("  uninstall <package-name>  Uninstall a package from user directory\n");
    printf("\n");
    printf("Install Options:\n");
    printf("  -s, --system      Install system-wide using dpkg (requires sudo)\n");
    printf("  -u, --user        Install to user directory (~/.local)\n");
    printf("\n");
    printf("Uninstall Options:\n");
    printf("  No additional options currently\n");
    printf("\n");
    printf("Global Options:\n");
    printf("  -h, --help        Show this help message\n");
    printf("  -v, --version     Show version information\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s install -s package.deb              # Install system-wide\n", prog_name);
    printf("  %s install -u package.deb              # Install to ~/.local\n", prog_name);
    printf("  %s install package.deb                 # Default: ask user to choose\n", prog_name);
    printf("  %s uninstall package-name              # Uninstall from user directory\n", prog_name);
    printf("\n");
}

void show_version(void) {
    printf("debpkg version 2.0.0\n");
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
