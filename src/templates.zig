// Template strings used by nexus to generate project files.

pub const all_templates: []const u8 =
    \\#include "../include/templates.h"
    \\
    \\const char* build_template_library =
    \\    "#include <stdio.h>\n"
    \\    "#include <stdlib.h>\n"
    \\    "#include <string.h>\n"
    \\    "#include \"../src_files.h\"\n"
    \\    "#include \"color_codes.h\"\n"
    \\    "\n"
    \\    "int main() {\n"
    \\    "    char obj_cmd[512];\n"
    \\    "    char base[64];\n"
    \\    "\n"
    \\    "    for (int i = 0; files[i] != NULL; i++) {\n"
    \\    "        const char* path = files[i];\n"
    \\    "        const char* slash = strrchr(path, '/');\n"
    \\    "        if (!slash) continue;\n"
    \\    "        slash++;\n"
    \\    "\n"
    \\    "        const char* dot = strrchr(slash, '.');\n"
    \\    "        if (!dot) continue;\n"
    \\    "        int len = dot - slash;\n"
    \\    "        strncpy(base, slash, len);\n"
    \\    "        base[len] = '\\0';\n"
    \\    "\n"
    \\    "        snprintf(obj_cmd, sizeof(obj_cmd),\n"
    \\    "            \"gcc -std=c2x -c -Iinclude %s -o build/%s.o\",\n"
    \\    "            path, base);\n"
    \\    "\n"
    \\    "        printf(GREEN \"[nexus]%s Compiling: %s\\n\", RESET, obj_cmd);\n"
    \\    "        if (system(obj_cmd) != 0) {\n"
    \\    "            fprintf(stderr, RED \"[nexus]%s Compilation failed: %s\\n\", RESET, path);\n"
    \\    "            return 1;\n"
    \\    "        }\n"
    \\    "    }\n"
    \\    "\n"
    \\    "    printf(GREEN \"[nexus]%s Archiving static library: lib/libmylib.a\\n\", RESET);\n"
    \\    "    int res = system(\"ar rcs lib/libmylib.a build/*.o\");\n"
    \\    "    if (res != 0) {\n"
    \\    "        fprintf(stderr, RED \"[nexus]%s Archiving failed.\\n\", RESET);\n"
    \\    "        return 1;\n"
    \\    "    }\n"
    \\    "\n"
    \\    "    printf(GREEN \"[nexus]\\033[0m Library built successfully.\\n\");\n"
    \\    "    return 0;\n"
    \\    "}\n";\n"
    \\
    \\const char* build_template_executable =
    \\    "#include <stdio.h>\n"
    \\    "#include <stdlib.h>\n"
    \\    "#include \"color_codes.h\"\n"
    \\    "#include \"../src_files.h\"\n"
    \\    "#include \"../include/templates.h\"\n"
    \\    "#include <string.h>\n\n"
    \\    "/*\n"
    \\    "    Use gcc for C files or g++ for c++\n"
    \\    "    Performs compilation and linking with -o all at once\n"
    \\    "*/\n"
    \\    "\n"
    \\    "int main() {\n"
    \\    "   char comp[] = \"gcc -std=c2x -Wall -Wextra -o \";\n"
    \\    "\n"
    \\    "   /*\n"
    \\    "       Provide directory and output exe name here.\n"
    \\    "       Provide relative path from the \"build\" directory\n"
    \\    "   */\n"
    \\    "   char out_name[256] = \"bin/\";\n"
    \\    "   int result = get_proj_attr(fopen(\".nexus\", \"r\"), out_name, 256, PROJ_NAME_ATTR);\n"
    \\    "   if (result != 0) { return 12; }\n"
    \\    "\n"
    \\    "   char cmd[2048] = {0};\n"
    \\    "\n"
    \\    "   strcat(cmd, comp);\n"
    \\    "   strcat(cmd, out_name);\n"
    \\    "\n"
    \\    "   int i = 0;\n"
    \\    "   while (files[i] != NULL) {\n"
    \\    "       strcat(cmd, \" \" );\n"
    \\    "       strcat(cmd, files[i]);\n"
    \\    "       i++;\n"
    \\    "   }\n"
    \\    "\n"
    \\    "   printf(GREEN \"[nexus]%s Compiling and linking: %s\\n\", RESET, cmd);\n"
    \\    "   result = system(cmd);\n"
    \\    "\n"
    \\    "   if (result != 0) {\n"
    \\    "       fprintf(stderr, RED \"[nexus]%s Compilation or linking failed with code %d\\n\", RESET, result);\n"
    \\    "       return 1;\n"
    \\    "   }\n"
    \\    "\n"
    \\    "   printf(GREEN \"[nexus]%s Compilation and linking successful.\\033[0m\\n\", RESET);\n"
    \\    "\n"
    \\    "   return 0;\n"
    \\    "}\n";
    \\
    \\const char* main_template =
    \\    "#include <stdio.h>\n\n"
    \\    "int main() {\n"
    \\    "   printf(\"Hello, World!\\n\");\n"
    \\    "   return 0;\n"
    \\    "}\n";
    \\
    \\const char* lib_template =
    \\    "/*\n"
    \\    "   Add library code in this file.\n"
    \\    "   Add headers for associated .c files in include/\n"
    \\    "*/\n";
    \\
    \\const char* color_codes_template =
    \\    "#ifndef COLOR_CODES_H\n"
    \\    "#define COLOR_CODES_H\n"
    \\    "\n"
    \\    "#define GREEN \"\\033[0;32m\"\n"
    \\    "#define RED \"\\033[0;31m\"\n"
    \\    "#define YELLOW \"\\033[0;33m\"\n"
    \\    "#define RESET \"\\033[0m\"\n"
    \\    "\n"
    \\    "#endif\n";
    \\
    \\const char* gitignore_template =
    \\    "# --- Ignore Build Directory ---\n"
    \\    "# Ignore binary output\n"
    \\    "bin/\n"
    \\    "# Ignore compiled files\n"
    \\    "*.exe\n"
    \\    "# Ignore IDE specific files\n"
    \\    ".vscode/\n";
    \\
    \\int get_proj_attr(FILE* nex_file, char* buffer, size_t size, const char* proj_attr) {
    \\    char line[256] = {0};
    \\    char tmp_buffer[256] = {0};
    \\
    \\    while (fgets(line, sizeof(line), nex_file)) {
    \\        if (sscanf(line, proj_attr, tmp_buffer) == 1) {
    \\            tmp_buffer[strcspn(tmp_buffer, "\r\n")] = 0;
    \\            strncat(buffer, tmp_buffer, size - strlen(buffer) - 1);
    \\            return 0;
    \\        }
    \\    }
    \\
    \\    fprintf(stderr, "Attribute not found in .nexus file\n");
    \\    return 12;
    \\}
;

pub const build_template_library: []const u8 =
    \\#include <stdio.h>
    \\#include <stdlib.h>
    \\#include <string.h>
    \\#include "../src_files.h"
    \\#include "color_codes.h"
    \\
    \\int main() {
    \\    char obj_cmd[512];
    \\    char base[64];
    \\
    \\    for (int i = 0; files[i] != NULL; i++) {
    \\        const char* path = files[i];
    \\        const char* slash = strrchr(path, '/');
    \\        if (!slash) continue;
    \\        slash++;
    \\
    \\        const char* dot = strrchr(slash, '.');
    \\        if (!dot) continue;
    \\        int len = dot - slash;
    \\        strncpy(base, slash, len);
    \\        base[len] = '\0';
    \\
    \\        snprintf(obj_cmd, sizeof(obj_cmd),
    \\            "gcc -std=c2x -c -Iinclude %s -o build/%s.o",
    \\            path, base);
    \\
    \\        printf(GREEN "[nexus]%s Compiling: %s\n", RESET, obj_cmd);
    \\        if (system(obj_cmd) != 0) {
    \\            fprintf(stderr, RED "[nexus]%s Compilation failed: %s\n", RESET, path);
    \\            return 1;
    \\        }
    \\    }
    \\
    \\    printf(GREEN "[nexus]%s Archiving static library: lib/libmylib.a\n", RESET);
    \\    int res = system("ar rcs lib/libmylib.a build/*.o");
    \\    if (res != 0) {
    \\        fprintf(stderr, RED "[nexus]%s Archiving failed.\n", RESET);
    \\        return 1;
    \\    }
    \\
    \\    printf(GREEN "[nexus]\033[0m Library built successfully.\n");
    \\    return 0;
    \\}
    \\
;

pub const build_template_executable: []const u8 =
    \\#include <stdio.h>
    \\#include <stdlib.h>
    \\#include "color_codes.h"
    \\#include "../src_files.h"
    \\#include "../include/templates.h"
    \\#include <string.h>
    \\
    \\/*
    \\    Use gcc for C files or g++ for c++
    \\    Performs compilation and linking with -o all at once
    \\*/
    \\
    \\int main() {
    \\   char comp[] = "gcc -std=c2x -Wall -Wextra -o ";
    \\
    \\   /*
    \\       Provide directory and output exe name here.
    \\       Provide relative path from the "build" directory
    \\   */
    \\   char out_name[256] = "bin/";
    \\   int result = get_proj_attr(fopen(".nexus", "r"), out_name, 256, PROJ_NAME_ATTR);
    \\   if (result != 0) { return 12; }
    \\
    \\   char cmd[2048] = {0};
    \\
    \\   strcat(cmd, comp);
    \\   strcat(cmd, out_name);
    \\
    \\   int i = 0;
    \\   while (files[i] != NULL) {
    \\       strcat(cmd, " " );
    \\       strcat(cmd, files[i]);
    \\       i++;
    \\   }
    \\
    \\   printf(GREEN "[nexus]%s Compiling and linking: %s\n", RESET, cmd);
    \\   result = system(cmd);
    \\
    \\   if (result != 0) {
    \\       fprintf(stderr, RED "[nexus]%s Compilation or linking failed with code %d\n", RESET, result);
    \\       return 1;
    \\   }
    \\
    \\   printf(GREEN "[nexus]%s Compilation and linking successful.\033[0m\n", RESET);
    \\
    \\   return 0;
    \\}
    \\
;

pub const main_template: []const u8 =
    \\#include <stdio.h>
    \\
    \\int main() {
    \\   printf("Hello, World!\n");
    \\   return 0;
    \\}
    \\
;

pub const lib_template: []const u8 =
    \\/*
    \\   Add library code in this file.
    \\   Add headers for associated .c files in include/
    \\*/
    \\
;

pub const color_codes_template: []const u8 =
    \\#ifndef COLOR_CODES_H
    \\#define COLOR_CODES_H
    \\
    \\#define GREEN "\033[0;32m"
    \\#define RED "\033[0;31m"
    \\#define YELLOW "\033[0;33m"
    \\#define RESET "\033[0m"
    \\
    \\#endif
    \\
;

pub const gitignore_template: []const u8 =
    \\# --- Ignore Build Directory ---
    \\# Ignore binary output
    \\bin/
    \\# Ignore compiled files
    \\*.exe
    \\# Ignore IDE specific files
    \\.vscode/
    \\
;

pub const header_template: []const u8 =
    \\#ifndef TEMPLATES_H
    \\#define TEMPLATES_H
    \\
    \\#include <stdio.h>
    \\#include <string.h>
    \\#include "../nexus_build/color_codes.h"
    \\#define PROJ_PATH_ATTR "PATH=%225s"
    \\#define PROJ_NAME_ATTR "ProjectName=%255s"
    \\
    \\extern const char* build_template_library;
    \\extern const char* build_template_executable;
    \\extern const char* main_template;
    \\extern const char* lib_template;
    \\extern const char* color_codes_template;
    \\extern const char* gitignore_template;
    \\extern const char* all_templates;
    \\extern const char* header_template;
    \\
    \\int get_proj_attr(FILE* nex_file, char* buffer, size_t size, const char* proj_attr);
    \\
    \\#endif
    \\
;

pub const help_msg: []const u8 =
    \\   --   Type "help [name]" for explanation of the named command   --
    \\
    \\   Commands
    \\   --------
    \\   init
    \\   new [proj_name]
    \\   build
    \\   run
    \\   export
    \\
;

pub const help_init: []const u8 =
    \\   nexus init
    \\
    \\   ie. command: nexus init
    \\   Initializes a new nexus project inside of a directory
    \\   already created by the user.
    \\   Project name will be set to a default name in .nexus file.
    \\   Users may manually edit .nexus file to change this project name.
    \\
;

pub const help_new: []const u8 =
    \\   nexus new [proj_name]
    \\
    \\   ie. command: nexus new hello_world
    \\   This will automatically create a new directory called hello_world
    \\   and place all build system contents inside this new directory.
    \\
    \\   Acts the same as "nexus init", but also creates the directory for you.
    \\
    \\   Project name will be set to the name specified upon "nexus new" command.
    \\   Users may manually edit .nexus file to change this project name.
    \\
;

pub const help_build: []const u8 =
    \\   nexus build
    \\
    \\   ie. command: nexus build
    \\   This command will automatically build a static library or executable
    \\   depending on whether or not users initialized their project with --lib
    \\   or not.
    \\
;

pub const help_run: []const u8 =
    \\   nexus run
    \\
    \\   ie. command: nexus run
    \\   This command will auto rebuild project, and subsequently run the
    \\   program executable located in the project bin/ directory.
    \\
;
