#include "../include/nexus_commands.h"





#define BUFFER_SIZE 2048





NEX_ERROR nexus_init(int argc, char* argv[]) {
    NEX_ERROR result = SUCCESS;

    
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

    // Create the self-build directory
    result = nexus_mkdir("nexus_build");
    if (result != SUCCESS) {
        return ERR_MKDIR_FAILED;
    }
    
    // create the templates.c file in the build directory
    result = build_file("nexus_build/user_templates.c", all_templates);
    if (result != SUCCESS) {
        printf(RED "[nexus]%s \"nexus_build/user_templates.c\" failed to create.\n", RESET);
        exit(ERR_FAILED_TO_OPEN);
    }
    printf(GREEN "[nexus]%s \"nexus_build/user_templates.c\" created.\n", RESET);

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

    result = build_file("nexus_build/color_codes.h", color_codes_template);
    if (result != SUCCESS) exit(1);

    result = nexus_mkdir("bin");
    if (result != 0) return ERR_MKDIR_FAILED;
    

    // TODO: put resulting code in a build_exe_proj()
    // TODO: create a library style buiild_lib_proj() if argv[2] == "--lib"
    // If a --lib tag is specified, exclude main.c and add a lib.c w/ no main()
    // TODO: argc conditions make it so a new project wont have a bin/
    if ((argc == 2 || (argc == 3 && strcmp(argv[1], "new") == 0)) || 
        (argc == 4) && (strcmp(argv[3], "--lib") == 0)) {

        if (argc == 4) {
            result = build_file("src/lib.c", lib_template);
            if (result != SUCCESS) exit(result);
            fprintf(nex_file, "TargetType=library\n");
        }
        else {
            result = build_file("src/main.c", main_template);
            if (result != SUCCESS) exit(result);
            fprintf(nex_file, "TargetType=executable\n");
        }

        fprintf(nex_file, "CC=gcc\n");
    } 
    else if (
        ((argc == 3) && (strcmp(argv[2], "--lib") == 0)) || 
        (((argc == 4) && (strcmp(argv[3], "--lib") == 0)))
    )
    {
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

    strcmp(argv[1], "init") == 0 
        ? fprintf(nex_file, "ProjectName=DefaultName") : fprintf(nex_file, "ProjectName=%s", argv[2]);

    // Close .nexus file for writing
    fclose(nex_file);

    // BUILD SRC_FILES_H
    add_src_files();

    result = build_file(".gitignore", gitignore_template);
    if (result != 0) exit(result);
    
    result = nexus_git_init();

    printf(GREEN "[nexus]%s Project default_project initialized successfully.\n", RESET);

    return SUCCESS;
}




// TODO: Get rid of project_name param and just use the argv indexing
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

    result = nexus_init(argc, argv);
    if (result != SUCCESS) {
        printf(RED "[nexus]%s Failed to initialize nexus project\n", RESET);
        return result;
    }
    
    return SUCCESS;
}





/**
 * @brief Executes binary executable located in bin/ directory. Disabled
 *        if TargetType=library
 * 
 * @return NEX_ERROR 
 */
NEX_ERROR nexus_run() {
    NEX_ERROR result = SUCCESS;

    printf(GREEN "[nexus]%s Executing project...\n", RESET);

    // TODO: Change to dynstr
    char cmd[256] = "bin/";

    FILE* nex_file = fopen(".nexus", "r");
    if (!nex_file)  {
        perror("Could not open .nexus file");
        return ERR_FAILED_TO_OPEN;
    }
    result = get_proj_attr(nex_file, cmd, 256, PROJ_NAME_ATTR);
    fclose(nex_file);
    if (result != 0) {
        return ERR_FAILED_TO_GET_EXE;
    }

    result = system(cmd);
    if (result != 0) {
        perror("System compilation call unsuccessful");
        return 1;
    }

    return SUCCESS;
}





/**
 * @brief Builds executable and library projects, leaving the binary in the projects
 *        bin/ directory. Combines compilation and linking steps.
 * 
 * @return NEX_ERROR 
 */
NEX_ERROR nexus_build() {
    NEX_ERROR result = SUCCESS;

    result = add_src_files();

    printf(GREEN "[nexus]%s Added src files.\n", RESET);

    // TODO: Change to dynstr
    char type_str[32] = "executable";

    FILE* nex_file = fopen(".nexus",  "r");
    if (nex_file) {
        fscanf(nex_file, "TargetType=%32s", type_str);
        fclose(nex_file);
    }


    // TODO: Change to dynstr
    char cc_name[8] = "gcc";

    nex_file = fopen(".nexus",  "r");
    if (nex_file) {
        fscanf(nex_file, "CC=%8s", cc_name);
        fclose(nex_file);
    }


    printf(GREEN "[nexus]%s Read .nexus file.\n", RESET);

    FILE* templates;

    if (strcmp(type_str, "executable") == 0)
        templates = fopen("nexus_build/template_exe.txt", "r");
    else if (strcmp(type_str, "library") == 0)
        templates = fopen("nexus_build/template_lib.txt", "r");
    else 
        return ERR_FAILED_TO_READ;

    // TODO: Change to dynstr
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
        return ERR_FAILED_TO_OPEN;
    }

    printf(GREEN "[nexus]%s Building build.c file.\n", RESET);

    // create the build.c file in current project directory
    result = build_file("nexus_build/build.c", template_contents);
    if (result != SUCCESS) exit(ERR_FAILED_TO_OPEN);

    printf(GREEN "[nexus]%s Created build.c file.\n", RESET);

    printf(GREEN "[nexus]%s Executing: gcc -std=c2x -Wall -Wextra -o nexus_build/build nexus_build/build.c nexus_build/user_templates.c\n", RESET);

    int res = system("gcc -std=c2x -Wall -Wextra -o nexus_build/build nexus_build/build.c nexus_build/user_templates.c");
    if (res != 0) {
        perror("System compilation call unsuccessful");
        return 1;
    }

    printf(GREEN "[nexus]%s Execution successful\n", RESET);

    printf(GREEN "[nexus]%s Running nexus_build/build\n", RESET);
    
    res = system("nexus_build/build");
    if (res != 0) {
        perror("Failed to execute nexus_build/build.exe");
        return ERR_FAILED_SYS_CALL;
    }

    return SUCCESS;

}




/*
    TODO: Clean this up. Works unless you call export on the program that is currently running in PATH.
*/
NEX_ERROR nexus_path_export() {
    FILE* nex_file = fopen(".nexus", "r");
    if (nex_file == NULL) {
        perror("Failed to open .nexus for reading");
        return ERR_FAILED_TO_OPEN;
    }


    // TODO: Change to dynstr
    char exe_name[256] = "bin/";
    

    nex_file = fopen(".nexus", "r");
    if (!nex_file)  {
        perror("Could not open .nexus file");
        return ERR_FAILED_TO_OPEN;
    }
    NEX_ERROR result = get_proj_attr(nex_file, exe_name, 256, PROJ_NAME_ATTR);
    fclose(nex_file);
    if (result != 0) {
        return ERR_FAILED_TO_GET_EXE;
    }

    // TODO: Change to dynstr
    char path[256] = {0};
    nex_file = fopen(".nexus", "r");
    if (!nex_file)  {
        perror("Could not open .nexus file");
        return ERR_FAILED_TO_OPEN;
    }
    result = get_proj_attr(nex_file, path, 256, PROJ_PATH_ATTR);
    fclose(nex_file);
    if (result != 0) {
        return ERR_FAILED_TO_GET_EXE;
    }

    // TODO: Change to dynstr
    char action[] = "cp ";
    // TODO: Change to dynstr
    char cmd[BUFFER_SIZE] = {0};

    strcat(cmd, action);
        printf("action: %s\n", action);
    strcat(cmd, " ");
    strcat(cmd, exe_name);
        printf("exe_name: %s\n", exe_name);
    strcat(cmd, " ");
    strcat(cmd, path);
        printf("path: %s\n", path);


    printf(GREEN "[nexus]%s Executing: %s\n", RESET, cmd);

    if (system(cmd) != 0) {
        strcat(cmd, "_");
        if (system(cmd) != 0) return ERR_FAILED_PATH_EXPORT;
        // TODO: Change to dynstr
        char rm_cmd[256] = "rm ";
        strcat(rm_cmd, path);
        strcat(rm_cmd, exe_name);
        result = system(rm_cmd);
        if (result !=0) return ERR_FAILED_PATH_EXE_EXCH;
        cmd[strlen(cmd) - 1] = '\0';
    }   

    printf(GREEN "[nexus]%s Successfully exported to PATH\n", RESET);
    
    return SUCCESS;
}


