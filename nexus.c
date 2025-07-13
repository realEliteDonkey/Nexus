#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "utils.h"


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

    // Builds a project with an executable
    // TODO: Prevent re-init 
    if (strcmp(argv[1], "init") == 0) {
        result = nexus_mkdir("nexus_build");
        if (result != SUCCESS) {
            return ERR_MKDIR_FAILED;
        }

        result = nexus_mkdir("src");
        if (result != SUCCESS) {
            return ERR_MKDIR_FAILED;
        }

        result = nexus_mkdir("include");
        if (result != SUCCESS) {
            return ERR_MKDIR_FAILED;
        }

        result = nexus_mkdir("build");
        if (result != SUCCESS) {
            return ERR_MKDIR_FAILED;
        }

        FILE* nex_file = fopen(".nexus", "w");
        if (nex_file == NULL) {
            perror("Could not open .nexus file");
            return ERR_FAILED_TO_OPEN;
        }
        

        // TODO: put resulting code in a build_exe_proj()
        // TODO: create a library style buiild_lib_proj() if argv[2] == "--lib"
        // If a --lib tag is specified, exclude main.c and add a lib.c w/ no main()
        if (argc == 2) {
            result = nexus_mkdir("bin");
            if (result != 0) {
                return ERR_MKDIR_FAILED;
            }

            result = build_file("src/main.c", main_template);
            if (result != 0) exit(result);

            fprintf(nex_file, "TargetType=executable\n");
        } 
        else if (argc == 3 && strcmp(argv[2], "--lib") == 0) {
            result = nexus_mkdir("lib");
            if (result != 0) {
                return ERR_MKDIR_FAILED;
            }

            result = nexus_mkdir("examples");
            if (result != 0) {
                return ERR_MKDIR_FAILED;
            }

            result = build_file("src/lib.c", lib_template);
            if (result != 0) exit(result);

            fprintf(nex_file, "TargetType=library\n");
        }

        // Close .nexus file for writing
        fclose(nex_file);

        // BUILD SRC_FILES_H
        add_src_files();

        // TODO: Perform a 'git init'
    }

    else if (strcmp(argv[1], "build") == 0 && argc == 2) {
        result = nexus_build();
    }

    // TODO: Disable run for LIBRARY TargetTypes.
    else if (strcmp(argv[1], "run") == 0 && argc == 2) {
        result = nexus_build();
        result = nexus_run();
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

