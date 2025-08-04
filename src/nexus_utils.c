#include "../include/nexus_utils.h"



#define BUFFER_SIZE 1024

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
 * @brief Replaces all occurrences of a word in a file with a new word.
 *
 * This function reads the contents of the specified file, replaces every occurrence
 * of `old_word` with `new_word`, and writes the updated content to a temporary file.
 * After processing, the original file is replaced with the updated file.
 *
 * @param filename The path to the file to be modified.
 * @param old_word The word to be replaced.
 * @param new_word The word to replace with.
 * @return NEX_ERROR Returns SUCCESS on success, or ERR_FAILED_TO_OPEN if the file cannot be opened or created.
 *
 * @note The function uses a fixed buffer size (BUFFER_SIZE) for reading lines.
 *       If a line exceeds this size, it may not be processed correctly.
 *       The temporary file is named "temp.txt" and will overwrite any existing file with that name.
 */
NEX_ERROR replace_word(const char *filename, const char *old_word, const char *new_word) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening file");
        return ERR_FAILED_TO_OPEN;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        perror("Error creating temp file");
        fclose(fp);
        return ERR_FAILED_TO_OPEN;
    }

    char buffer[BUFFER_SIZE];

    while (fgets(buffer, BUFFER_SIZE, fp)) {
        char *start = buffer;
        char *pos;
        char tempBuf[BUFFER_SIZE * 2] = ""; // To accommodate possible expansion
        tempBuf[0] = '\0';

        while ((pos = strstr(start, old_word)) != NULL) {
            strncat(tempBuf, start, pos - start); // Copy up to match
            strcat(tempBuf, new_word);            // Add replacement
            start = pos + strlen(old_word);       // Move past the match
        }

        strcat(tempBuf, start); // Add remaining part of the line
        fputs(tempBuf, temp);
    }

    fclose(fp);
    fclose(temp);

    remove(filename);
    rename("temp.txt", filename);

    return SUCCESS;
}





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
    // TODO: Change to dynstr
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

        // TODO: Change to dynstr
        char entry_relative[BUFFER_SIZE];
        if (relative_path[0] != '\0')
            snprintf(entry_relative, sizeof(entry_relative), "%s/%s", relative_path, entry->d_name);
        else
            snprintf(entry_relative, sizeof(entry_relative), "%s", entry->d_name);

        // TODO: Change to dynstr
        char entry_full[BUFFER_SIZE];
        int written = snprintf(entry_full, sizeof(entry_full), "%s/%s", base_path, entry_relative);
        if (written < 0 || written >= sizeof(entry_full)) {
            fprintf(stderr, "Path too long: %s/%s\n", base_path, entry_relative);
            return ERR_PATH_TOO_LONG;  // define an appropriate error code
        }

        
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
    if (result != SUCCESS) {
        perror("Failed to recursively search src/ files");
        return result;
    }

    closedir(dir);
    
    fprintf(src_files, "    NULL\n");
    fprintf(src_files, "};\n\n");
    fprintf(src_files, "#endif\n");
    
    fclose(src_files);
    return SUCCESS;
}





/**
 * @brief Creates new directory with input relative path to project root directory.
 * 
 * @param str Relative path from root directory.
 * @return NEX_ERROR 
 */
NEX_ERROR nexus_mkdir(const char* str) {
    if (mkdir(str, 0777) == 0)
        printf(GREEN "[nexus]%s Directory \"%s\" created.\n", RESET, str);
    else
        return ERR_MKDIR_FAILED;
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
    if (res != 0) {
        perror("Failed to initialize git repository");
        return ERR_FAILED_TO_OPEN;
    }

    res = system("git add .");
    if (res != 0) {
        perror("Failed to add files to git repository");
        return ERR_FAILED_TO_OPEN;
    }

    res = system("git commit -m 'Initial NEXUS PROJECT commit!'");
    if (res != 0) {
        perror("Failed to commit files to git repository");
        return ERR_FAILED_TO_OPEN;
    }

    return SUCCESS;
}

