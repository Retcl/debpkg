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
