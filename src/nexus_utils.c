#include "../include/utils.h"

#define BUFFER_SIZE 2048

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

    printf(GREEN "[nexus]%s Read .nexus file.\n", RESET);

/*
    // NEXUS'S BUILD.C IS EMPTY WHEN THIS EXECUTES.
    // INSTEAD COPY CONTENTS FROM THE TEXT FILES AND THEN USE THAT AS A BUILD.C

    FILE* templates = fopen("nexus_build/templates.c", "r");
    char template_contents[BUFFER_SIZE] = {0};
    if (templates) {
        if (strcmp(type_str, "executable") == 0) {
            fscanf(templates, "const char* build_template_executable = %s", template_contents);
        } else if (strcmp(type_str, "library") == 0)
        fscanf(templates, "const char* build_template_library = %s", template_contents);
        fclose(templates);
    }

*/

    FILE* templates;

    if (strcmp(type_str, "executable") == 0)
        templates = fopen("nexus_build/template_exe.txt", "r");
    else if (strcmp(type_str, "library") == 0)
        templates = fopen("nexus_build/template_lib.txt", "r");
    else 
        return ERR_FAILED_TO_READ;

    char template_contents[BUFFER_SIZE] = {0};

    if (templates != NULL) {
        size_t offset = 0;
        while (fgets(template_contents + offset, sizeof(template_contents) - offset, templates) != NULL) {
            offset = strlen(template_contents);
            if (offset >= sizeof(template_contents) - 1) break; // prevents overflow
        }

        if (offset > 0) {
            printf(GREEN "[nexus]%s Successfully read contents of template file\n", RESET);
        } else {
            printf(RED "[nexus]%s Template file is empty or unreadable\n", RESET);
            return ERR_FAILED_TO_READ;
        }

        fclose(templates);
    } else {
        printf(RED "[nexus]%s Failed to open template file\n", RESET);
        fclose(templates);
        return ERR_FAILED_TO_OPEN;
    }




    printf(GREEN "[nexus]%s Building build.c file.\n", RESET);

    // create the build.c file in current project directory
    result = build_file("nexus_build/build.c", template_contents);
    if (result != SUCCESS) exit(ERR_FAILED_TO_OPEN);

    printf(GREEN "[nexus]%s Created build.c file.\n", RESET);

    printf(GREEN "[nexus]%s Executing: gcc -o nexus_build/build nexus_build/build.c\n", RESET);

    int res = system("gcc -o nexus_build/build nexus_build/build.c nexus_build/templates.c");
    if (res != 0) {
        perror("System compilation call unsuccessful");
        return 1;
    }

    printf(GREEN "[nexus]%s Execution successful\n", RESET);

    printf(GREEN "[nexus]%s Running nexus_build/build\n", RESET);
    
    system("nexus_build/build");
}

NEX_ERROR nexus_run() {
    NEX_ERROR result = SUCCESS;

    printf(GREEN "[nexus]%s Executing: bin/main\n", RESET);

    char cmd[256] = "bin/";

    FILE* nex_file = fopen(".nexus", "r");
    if (!nex_file)  {
        perror("Could not open .nexus file");
        return ERR_FAILED_TO_OPEN;
    }
    get_proj_name(nex_file, cmd, 256);


    result = system(cmd);
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

    // create the templates.c file in the build directory
    result = build_file("nexus_build/templates.c", all_templates);
    if (result != SUCCESS) {
        printf(RED "[nexus]%s \"nexus_build/templates.c\" failed to create.\n", RESET);
        exit(ERR_FAILED_TO_OPEN);
    }
    printf(GREEN "[nexus]%s \"nexus_build/templates.c\" created.\n", RESET);

    // create the templates.c file in the build directory
    result = build_file("include/templates.h", header_template);
    if (result != SUCCESS) {
        printf(RED "[nexus]%s \"include/templates.h\" failed to create.\n", RESET);
        exit(ERR_FAILED_TO_OPEN);
    }
    printf(GREEN "[nexus]%s \"include/templates.h\" created.\n", RESET);

    // create txt files in build directory
    result = build_file("nexus_build/template_exe.txt", build_template_executable);
    if (result != SUCCESS) {
        printf(RED "[nexus]%s \"nexus_build/template_exe.txt\" failed to create.\n", RESET);
        exit(ERR_FAILED_TO_OPEN);
    }
    printf(GREEN "[nexus]%s \"nexus_build/template_exe.txt\" created.\n", RESET);

    // create txt files in build directory
    result = build_file("nexus_build/template_lib.txt", build_template_library);
    if (result != SUCCESS) {
        printf(RED "[nexus]%s \"nexus_build/template_lib.txt\" failed to create.\n", RESET);
        exit(ERR_FAILED_TO_OPEN);
    }
    printf(GREEN "[nexus]%s \"nexus_build/template_lib.txt\" created.\n", RESET);


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

    result = build_file(".gitignore", gitignore_template);
    if (result != 0) exit(result);

    printf(GREEN "[nexus]%s Project %s created successfully.\n", RESET, project_name);

    // Initialize git repository
    result = nexus_git_init();
    
    return SUCCESS;
}

NEX_ERROR nexus_init(int argc, char* argv[]) {
    NEX_ERROR result = SUCCESS;

    result = nexus_mkdir("nexus_build");
    if (result != SUCCESS) {
        return ERR_MKDIR_FAILED;
    }

    // create the templates.c file in the build directory
    result = build_file("nexus_build/templates.c", all_templates);
    if (result != SUCCESS) {
        printf(RED "[nexus]%s \"nexus_build/templates.c\" failed to create.\n", RESET);
        exit(ERR_FAILED_TO_OPEN);
    }
    printf(GREEN "[nexus]%s \"nexus_build/templates.c\" created.\n", RESET);

    // create the templates.c file in the build directory
    result = build_file("include/templates.h", header_template);
    if (result != SUCCESS) {
        printf(RED "[nexus]%s \"include/templates.h\" failed to create.\n", RESET);
        exit(ERR_FAILED_TO_OPEN);
    }
    printf(GREEN "[nexus]%s \"include/templates.h\" created.\n", RESET);

    // create txt files in build directory
    result = build_file("nexus_build/template_exe.txt", build_template_executable);
    if (result != SUCCESS) {
        printf(RED "[nexus]%s \"nexus_build/template_exe.txt\" failed to create.\n", RESET);
        exit(ERR_FAILED_TO_OPEN);
    }
    printf(GREEN "[nexus]%s \"nexus_build/template_exe.txt\" created.\n", RESET);

    // create txt files in build directory
    result = build_file("nexus_build/template_lib.txt", build_template_library);
    if (result != SUCCESS) {
        printf(RED "[nexus]%s \"nexus_build/template_lib.txt\" failed to create.\n", RESET);
        exit(ERR_FAILED_TO_OPEN);
    }
    printf(GREEN "[nexus]%s \"nexus_build/template_lib.txt\" created.\n", RESET);

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

    result = build_file(".gitignore", gitignore_template);
    if (result != 0) exit(result);

    printf(GREEN "[nexus]%s Project default_project initialized successfully.\n", RESET);
    
    result = nexus_git_init();

    return SUCCESS;
}

NEX_ERROR git_installed() {
    return system("git --version > /dev/null 2>&1") == SUCCESS ? SUCCESS : ERR_GIT_NOT_INSTALLED;
}

NEX_ERROR nexus_git_init() {
    NEX_ERROR result = git_installed();
    if (result != SUCCESS) {
        printf(RED "[nexus]%s Git is not installed. Please install Git to use this feature.\n", RESET);
        return ERR_GIT_NOT_INSTALLED;
    }

    // Initialize a new git repository
    int res = system("git init");
    if (res != SUCCESS) {
        perror("Failed to initialize git repository");
        return ERR_FAILED_TO_OPEN;
    }

    res = system("git add .");
    if (res != SUCCESS) {
        perror("Failed to add files to git repository");
        return ERR_FAILED_TO_OPEN;
    }

    res = system("git commit -m 'Initial NEXUS PROJECT commit!'");
    if (res != SUCCESS) {
        perror("Failed to commit files to git repository");
        return ERR_FAILED_TO_OPEN;
    }

    return SUCCESS;
}

NEX_ERROR nexus_path_export() {
    FILE* nex_file = fopen(".nexus", "r");
    if (nex_file == NULL) {
        perror("Failed to open .nexus for reading");
        return ERR_FAILED_TO_OPEN;
    }

    char exe_name[256];
    if (fscanf(nex_file, "ExeName=%255s", exe_name) == EOF) {
        perror("Failed to retrieve executable name in .nexus");
        return ERR_FAILED_TO_GET_EXE;
    }

    char path[256];
    if (fscanf(nex_file, "PATH=%255s", path) == EOF) {
        perror("Failed to retrieve PATH in .nexus");
        return ERR_FAILED_TO_GET_PATH;
    }

    char action[] = "cp ";
    char cmd[BUFFER_SIZE] = {0};

    strcat(cmd, action);
    strcat(cmd, exe_name);
    strcat(cmd, " ");
    strcat(cmd, path);

    if (system(cmd) != SUCCESS)
        return ERR_FAILED_PATH_EXPORT;
    
    return SUCCESS;
}