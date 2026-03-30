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
#include "../include/install_system.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int install_system(const char *deb_path) {
    char cmd[MAX_CMD_LEN];
    
    if (!file_exists(deb_path)) {
        print_error("DEB file does not exist");
        return -1;
    }
    
    print_info("Installing package using dpkg (system-wide)...");
    print_warning("This requires sudo privileges");
    
    snprintf(cmd, sizeof(cmd), "sudo dpkg -i \"%s\"", deb_path);
    
    if (execute_command(cmd) != 0) {
        print_error("dpkg installation failed");
        print_info("You may need to fix broken dependencies with: sudo apt-get install -f");
        return -1;
    }
    
    print_success("Package installed successfully (system-wide)");
    return 0;
}
