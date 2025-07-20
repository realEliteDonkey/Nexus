#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "../include/utils.h"

NEX_ERROR main(int argc, char* argv[]) {

    NEX_ERROR result = SUCCESS;

    /*
        Arguments will usually count to two.
        With tags (ie. --lib), a third argument will be passed
    */
    if (argc < 2 || argc > 3) {
        perror("nexus has max TWO argument other than 'nexus' itself!\n");
        return ERR_NEX_ARG_COUNT;
    }

    // TODO: Nexus new command
    if (strcmp(argv[1], "new") == 0) {
        if (argc != 3) {
            perror("nexus 'new' command requires a project name!\n");
            return ERR_NEX_ARG_COUNT;
        }

        result = nexus_new(argv[2], argc, argv);
        if (result != SUCCESS) {
            perror("Failed to create new Nexus project.\n");
            return result;
        }
        return SUCCESS;
    }

    // Builds a project with an executable
    // TODO: Prevent re-init 
    if (strcmp(argv[1], "init") == 0) {
        result = nexus_init(argc, argv);
        return result;
    }

    else if (strcmp(argv[1], "build") == 0 && argc == 2) {
        result = nexus_build();
        return result;
    }

    // TODO: Disable run for LIBRARY TargetTypes.
    else if (strcmp(argv[1], "run") == 0 && argc == 2) {
        result = nexus_build();
        if (result != SUCCESS) {
            perror("Failed to build the project before running.\n");
            return result;
        }
        result = nexus_run();
        return result;
    }

    else if (strcmp(argv[1], "export") == 0 && argc == 2) {
        return nexus_path_export();
    }

    else {
        perror("No such nexus command exists!\n");
        return ERR_CMD_NOT_FOUND;
    }

    if (result != SUCCESS) {
        perror("An error occurred while executing the command.\n");
        return result;
    }

    return SUCCESS;
}

