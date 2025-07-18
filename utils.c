#include "utils.h"

/*
    Provides cross-platform compatability for the purpose
    of creating new directories using 'system()' command.
*/
#ifdef _WIN32
    #include <direct.h>
    #define mkdir(dir, mode) _mkdir(dir)
#else
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

/**
 * @brief builds a file with text inside (template).
 * 
 * @param file_name: name of file with relative path in project folder.
 * @param template: multiline string containing contents users want in their files.
 * 
 * @returns 0: Success, 1: Failed to open file, 
    2: Failed to write to file.
 */
NEX_ERROR build_file(const char* file_name, const char* template) {
    printf("[DEBUG] build_file trying to create: %s\n", file_name);
    FILE* file = fopen(file_name, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return ERR_FAILED_TO_OPEN;
    }

    if (fputs(template, file) == EOF) {
        perror("Failed to write to file");
        fclose(file);
        return ERR_FAILED_TO_WRITE;
    }

    if (fclose(file) == EOF) {
        perror("Failed to close file");
        return ERR_FAILED_TO_WRITE;
    }

    return SUCCESS;
}

/**
 * @brief Recursively writes all source files in the src directory
 * to the src_files.h file.
 * 
 * @param src_files: file pointer to the src_files.h file.
 * @param base_path: base path of the project directory.
 * @param relative_path: relative path from the base path to the current directory.
 * @return NEX_ERROR 
 */
NEX_ERROR write_src_files_recursive(FILE *src_files, const char *base_path, const char *relative_path) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", base_path, relative_path);

    DIR *dir = opendir(full_path);
    if (dir == NULL) {
        perror("Could not open directory");
        return ERR_DIR_NOT_FOUND;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char entry_relative[512];
        if (relative_path[0] != '\0')
            snprintf(entry_relative, sizeof(entry_relative), "%s/%s", relative_path, entry->d_name);
        else
            snprintf(entry_relative, sizeof(entry_relative), "%s", entry->d_name);

        char entry_full[512];
        snprintf(entry_full, sizeof(entry_full), "%s/%s", base_path, entry_relative);

        struct stat st;
        if (stat(entry_full, &st) == -1) {
            perror("Failed to stat entry");
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            // Recurse into subdirectory
            write_src_files_recursive(src_files, base_path, entry_relative);
        } else if (S_ISREG(st.st_mode)) {
            const char *ext = strrchr(entry->d_name, '.');
            if (ext && (strcmp(ext, ".c") == 0 || strcmp(ext, ".cpp") == 0)) {
                fprintf(src_files, "    \"src/%s\",\n", entry_relative);
            }
        }
    }

    closedir(dir);
    return SUCCESS;
}

/**
 * @brief Generates a src_files.h file containing all source files
 * in the src directory with .c or .cpp extensions.
 * This file is used to compile the project
 * and is created before the build.c file.
 * This function reads the src directory,
 * retrieves all source files with .c or .cpp extensions,
 * and writes their names into src_files.h.
 * 
 * @return NEX_ERROR 
 */
NEX_ERROR add_src_files() {
    // used for src_file.h creation
    const char* base_dir = "src";

    struct dirent* entry;
    DIR *dir = opendir(base_dir);

    if (dir == NULL) {
        perror("Could not open directory.");
        return ERR_DIR_NOT_FOUND;
    }

    // Add src_file.h functionality code generation at runtime
    // must be done before build.c is built
    FILE* src_files = fopen("src_files.h", "w");
    if ((src_files) == NULL) {
        perror("Failed to open file");
        return ERR_FAILED_TO_OPEN;
    }

    // Write the header guard and array declaration
    fprintf(src_files, "#ifndef SRC_FILES_H\n");
    fprintf(src_files, "#define SRC_FILES_H\n\n");
    fprintf(src_files, "const char* files[] = {\n");

    NEX_ERROR result = write_src_files_recursive(src_files, base_dir, "");

    closedir(dir);
    
    fprintf(src_files, "    NULL\n");
    fprintf(src_files, "};\n\n");
    fprintf(src_files, "#endif\n");
    
    fclose(src_files);
    return SUCCESS;
}

NEX_ERROR nexus_mkdir(const char* str) {
    if (mkdir(str, 0777) == 0) {
        printf(GREEN "[nexus]%s Directory \"%s\" created.\n", RESET, str);
    } else {
        return ERR_MKDIR_FAILED;
    }
    return SUCCESS;
}

NEX_ERROR nexus_build() {
    NEX_ERROR result = SUCCESS;

    result = add_src_files();

    printf(GREEN "[nexus]%s Added src files.\n", RESET);
    char type_str[32] = "executable";

    FILE* nex_file = fopen(".nexus",  "r");
    if (nex_file) {
        fscanf(nex_file, "TargetType=%32s", type_str);
        fclose(nex_file);
    }

    printf(GREEN "[nexus]%s Created .nexus file.\n", RESET);
    
    const char* build_template = 
        strcmp(type_str, "executable") == 0 ? build_template_executable : build_template_library;
    
    printf(GREEN "[nexus]%s Building build.c file.\n", RESET);

    // create the build.c file in current project directory
    result = build_file("nexus_build/build.c", build_template);
    if (result != SUCCESS) exit(ERR_FAILED_TO_OPEN);

    printf(GREEN "[nexus]%s Created build.c file.\n", RESET);

    printf(GREEN "[nexus]%s Executing: gcc -o nexus_build/build nexus_build/build.c\n", RESET);

    int res = system("gcc -o nexus_build/build nexus_build/build.c");
    if (res != 0) {
        perror("System compilation call unsuccessful");
        return 1;
    }

    printf(GREEN "[nexus]%s Execution successful\n", RESET);

    printf(GREEN "[nexus]%s Running nexus_build\n", RESET);
    
    system("nexus_build/build");
}

NEX_ERROR nexus_run() {
    NEX_ERROR result = SUCCESS;

    printf(GREEN "[nexus]%s Executing: bin/main\n", RESET);

    result = system("bin/main");
    if (result != 0) {
        perror("System compilation call unsuccessful");
        return 1;
    }
}

NEX_ERROR nexus_new(const char* project_name, int argc, char* argv[]) {
    NEX_ERROR result = SUCCESS;

    // Create the project directory
    result = nexus_mkdir(project_name);
    if (result != SUCCESS) {
        return ERR_MKDIR_FAILED;
    }

    // Change to the project directory
    if (chdir(project_name) != 0) {
        perror("Failed to change directory");
        return ERR_FAILED_TO_OPEN;
    }
    printf("[DEBUG] Changed directory to %s\n", project_name);

    // Create necessary directories
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

    result = nexus_mkdir("nexus_build");
    if (result != SUCCESS) {
        return ERR_MKDIR_FAILED;
    }

    // Create .nexus file
    FILE* nex_file = fopen(".nexus", "w");
    if (nex_file == NULL) {
        fclose(nex_file);
        perror("Could not open .nexus file");
        return ERR_FAILED_TO_OPEN;
    }

    fprintf(nex_file, "ProjectName=%s\n", project_name);

    // TODO: put resulting code in a build_exe_proj()
    // TODO: create a library style buiild_lib_proj() if argv[2] == "--lib"
    // If a --lib tag is specified, exclude main.c and add a lib.c w/ no main()
    if (argc == 3) {
        result = nexus_mkdir("bin");
        if (result != 0) {
            return ERR_MKDIR_FAILED;
        }

        result = build_file("src/main.c", main_template);
        if (result != 0) exit(result);

        fprintf(nex_file, "TargetType=executable\n");
    } 
    else if (argc == 4 && strcmp(argv[3], "--lib") == 0) {
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
    
    fclose(nex_file);

    add_src_files();

    result = build_file("nexus_build/color_codes.h", color_codes_template);
    if (result != SUCCESS) exit(1);

    printf(GREEN "[nexus]%s Project %s created successfully.\n", RESET, project_name);
    
    return SUCCESS;
}

NEX_ERROR nexus_init(int argc, char* argv[]) {
    NEX_ERROR result = SUCCESS;

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

    result = build_file("nexus_build/color_codes.h", color_codes_template);
    if (result != SUCCESS) exit(1);
    

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


    printf(GREEN "[nexus]%s Project default_project initialized successfully.\n", RESET);
    
    return SUCCESS;
}