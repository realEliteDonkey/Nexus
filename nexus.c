#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

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

/*
    1. nexus init:
        - opens a new build.c file
        - creates another directory called "lib"
        - creates another directory called "src"
        - creates another directory called "bin"
        - opens a new main.c file inside src with 
          <stdio.h> included as well as a sample 
          hello world program
        - ***perform a "git init"***
        - add .gitignore
    2. nexus build:
        - recompiles build.c and runs build.exe which 
          then compiles, and links all known files in
          build.c
        - put executable inside bin directory
    3. nexus run:
        - perform a "nexus build" followed by running
          the executable in bin directory
    

*/

/*
    TODO: Add git init
    TODO: Add .gitignore

    TODO: FUTURE
    *************
    Instead of adding files w/ relative path manually
    to build.c, we will pivot to auto-adding src files
    to build process. -> *-'nexus build' and 'nexus run'-*

*/

int nexus_mkdir(const char* str);
void add_src_files();

void add_src_files() {
    // used for src_file.h creation
    const char* dir_name = "src";
    struct dirent* entry;
    DIR *dir = opendir(dir_name);

    if (dir == NULL)
        perror("Could not open directory.");

    // Add src_file.h functionality code generation at runtime
    // must be done before build.c is built
    FILE* src_files = fopen("src_files.h", "w");
    if ((src_files) == NULL) {
        perror("Failed to open file");
    } else {
        fprintf(src_files, "#ifndef SRC_FILES_H\n");
        fprintf(src_files, "#define SRC_FILES_H\n\n");
        fprintf(src_files, "const char* files[] = {\n");

        // loop through src directory, retrieving all src file names 
        // with .c or .cpp extensions
        while ((entry = readdir(dir)) != NULL) {
            // Skip "." and ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            const char* ext = strrchr(entry->d_name, '.');
            if (ext != NULL && (strcmp(ext, ".c") == 0 || strcmp(ext, ".cpp") == 0)) {
                fprintf(src_files, "    \"src/");
                fprintf(src_files, "%s\",\n", entry->d_name);
            }
            
        }

        closedir(dir);
        
        fprintf(src_files, "    NULL\n");
        fprintf(src_files, "};\n\n");
        fprintf(src_files, "#endif\n");
    }
    fclose(src_files);
}

int nexus_mkdir(const char* str) {
    if (mkdir(str, 0777) == 0) {
        printf("Directory \"%s\" created.\n", str);
    } else {
        perror("mkdir failed");
        return 1;
    }
    return 0;
}

const char* build_template = 
    "#include <stdio.h>\n"
    "#include <stdlib.h>\n"
    "#include \"src_files.h\"\n"
    "#include <string.h>\n\n"
    "/*\n"
    "    Use gcc for C files or g++ for c++\n"
    "    Performs compilation and linking with -o all at once\n"
    "*/\n"
    "\n"
    "int main() {\n"
    "   char comp[] = \"gcc -o \";\n"
    "\n"
    "   /*\n"
    "       Provide directory and output exe name here.\n"
    "       Provide relative path from the \"build\" directory\n"
    "   */\n"
    "   char out_name[] = \"bin/main\";    // change \"bin\" and \"main\" if you wish\n"
    "\n"
    "   /*\n"
    "       Insert all .c or .cpp files in files[].\n"
    "       These files will be compiled and linked all at once.\n"
    "       Ensure you provide the relative path in relation to the \"build\"\n"
    "       directory.\n"
    "   */\n"
    "   /*\n"
    "       Command input buffer is 2048 chars.\n"
    "       If you have a lot of files, just input whatever you need for a size.\n"
    "   */\n"
    "   char cmd[2048] = {0};\n"
    "\n"
    "   strcat(cmd, comp);\n"
    "   strcat(cmd, out_name);\n"
    "\n"
    "   int i = 0;\n"
    "   while (files[i] != NULL) {\n"
    "       strcat(cmd, \" \");\n"
    "       strcat(cmd, files[i]);\n"
    "       i++;\n"
    "   }\n"
    "\n"
    "   printf(\"Compiling and linking: %s\\n\", cmd);\n"
    "   int result = system(cmd);\n"
    "\n"
    "   if (result != 0) {\n"
    "       fprintf(stderr, \"Compilation or linking failed with code %d\\n\", result);\n"
    "       return 1;\n"
    "   }\n"
    "\n"
    "   printf(\"Compilation and linking successful.\\n\");\n"
    "\n"
    "   return 0;\n"
    "}\n";



const char* main_template = 
    "#include <stdio.h>\n\n"
    "int main() {\n"
    "   printf(\"Hello, World!\\n\");\n"
    "   return 0;\n"
    "}\n";
                             
int main(int argc, char* argv[]) {

    if (argc != 2)
        perror("nexus has max ONE argument other than 'nexus' itself!\n");


    if (strcmp(argv[1], "init") == 0) {
        int result = nexus_mkdir("src");
        if (result != 0)
            perror("Error creating directory 'src'");

        result = nexus_mkdir("lib");
        if (result != 0)
            perror("Error creating directory 'lib'");

        result = nexus_mkdir("bin");
        if (result != 0)
            perror("Error creating directory 'bin'");



        FILE* main_file = fopen("src/main.c", "w");
        if (main_file == NULL)
            perror("Failed to open file");
        else {
            if (fputs(main_template, main_file) == EOF) {
                perror("Failed to write to file");
                fclose(main_file);
            }
        }


        // BUILD SRC_FILES_H
        add_src_files();



        FILE* build_file = fopen("build.c", "w");
        if (build_file == NULL)
            perror("Failed to open file");
        else {
            if (fputs(build_template, build_file) == EOF) {
                perror("Failed to write to file");
                fclose(build_file);
            }
        }   

        // TODO: Perform a 'git init'
    }

    else if (strcmp(argv[1], "build") == 0) {
        add_src_files();
        system("gcc -o bin/build build.c");
        system("bin/build");
    }

    else if (strcmp(argv[1], "run") == 0) {
        add_src_files();
        system("gcc -o bin/build build.c");
        system("bin/build");
        system("bin/main");
    }

    else perror("No such nexus command exists!\n");

    return 0;
}

