#include "../include/templates.h"

const char* all_templates =
"#include \"../include/templates.h\"\n"
"\n"
"const char* build_template_library =\n"
"    \"#include <stdio.h>\\n\"\n"
"    \"#include <stdlib.h>\\n\"\n"
"    \"#include <string.h>\\n\"\n"
"    \"#include \\\"../src_files.h\\\"\\n\"\n"
"    \"#include \\\"color_codes.h\\\"\\n\"\n"
"    \"\\n\"\n"
"    \"int main() {\\n\"\n"
"    \"    char obj_cmd[512];\\n\"\n"
"    \"    char base[64];\\n\"\n"
"    \"\\n\"\n"
"    \"    for (int i = 0; files[i] != NULL; i++) {\\n\"\n"
"    \"        const char* path = files[i];\\n\"\n"
"    \"        const char* slash = strrchr(path, '/');\\n\"\n"
"    \"        if (!slash) continue;\\n\"\n"
"    \"        slash++;\\n\"\n"
"    \"\\n\"\n"
"    \"        const char* dot = strrchr(slash, '.');\\n\"\n"
"    \"        if (!dot) continue;\\n\"\n"
"    \"        int len = dot - slash;\\n\"\n"
"    \"        strncpy(base, slash, len);\\n\"\n"
"    \"        base[len] = '\\\\0';\\n\"\n"
"    \"\\n\"\n"
"    \"        snprintf(obj_cmd, sizeof(obj_cmd),\\n\"\n"
"    \"            \\\"gcc -c -Iinclude %s -o build/%s.o\\\",\\n\"\n"
"    \"            path, base);\\n\"\n"
"    \"\\n\"\n"
"    \"        printf(GREEN \\\"[nexus]%s Compiling: %s\\\\n\\\", RESET, obj_cmd);\\n\"\n"
"    \"        if (system(obj_cmd) != 0) {\\n\"\n"
"    \"            fprintf(stderr, RED \\\"[nexus]%s Compilation failed: %s\\\\n\\\", RESET, path);\\n\"\n"
"    \"            return 1;\\n\"\n"
"    \"        }\\n\"\n"
"    \"    }\\n\"\n"
"    \"\\n\"\n"
"    \"    printf(GREEN \\\"[nexus]%s Archiving static library: lib/libmylib.a\\\\n\\\", RESET);\\n\"\n"
"    \"    int res = system(\\\"ar rcs lib/libmylib.a build/*.o\\\");\\n\"\n"
"    \"    if (res != 0) {\\n\"\n"
"    \"        fprintf(stderr, RED \\\"[nexus]%s Archiving failed.\\\\n\\\", RESET);\\n\"\n"
"    \"        return 1;\\n\"\n"
"    \"    }\\n\"\n"
"    \"\\n\"\n"
"    \"    printf(GREEN \\\"[nexus]\\\\033[0m Library built successfully.\\\\n\\\");\\n\"\n"
"    \"    return 0;\\n\"\n"
"    \"}\\n\";\n"
"\n"
"const char* build_template_executable = \n"
"    \"#include <stdio.h>\\n\"\n"
"    \"#include <stdlib.h>\\n\"\n"
"    \"#include \\\"color_codes.h\\\"\\n\"\n"
"    \"#include \\\"../src_files.h\\\"\\n\"\n"
"    \"#include \\\"../include/templates.h\\\"\\n\"\n"
"    \"#include <string.h>\\n\\n\"\n"
"    \"/*\\n\"\n"
"    \"    Use gcc for C files or g++ for c++\\n\"\n"
"    \"    Performs compilation and linking with -o all at once\\n\"\n"
"    \"*/\\n\"\n"
"    \"\\n\"\n"
"    \"int main() {\\n\"\n"
"    \"   char comp[] = \\\"gcc -o \\\";\\n\"\n"
"    \"\\n\"\n"
"    \"   /*\\n\"\n"
"    \"       Provide directory and output exe name here.\\n\"\n"
"    \"       Provide relative path from the \\\"build\\\" directory\\n\"\n"
"    \"   */\\n\"\n"
"    \"   char out_name[256] = \\\"bin/\\\";\\n\"\n"
"    \"   int result = get_proj_attr(fopen(\\\".nexus\\\", \\\"r\\\"), out_name, 256, PROJ_NAME_ATTR);\\n\"\n"
"    \"   if (result != 0) { return 12; }\\n\"\n"
"    \"\\n\"\n"
"    \"   char cmd[2048] = {0};\\n\"\n"
"    \"\\n\"\n"
"    \"   strcat(cmd, comp);\\n\"\n"
"    \"   strcat(cmd, out_name);\\n\"\n"
"    \"\\n\"\n"
"    \"   int i = 0;\\n\"\n"
"    \"   while (files[i] != NULL) {\\n\"\n"
"    \"       strcat(cmd, \\\" \\\" );\\n\"\n"
"    \"       strcat(cmd, files[i]);\\n\"\n"
"    \"       i++;\\n\"\n"
"    \"   }\\n\"\n"
"    \"\\n\"\n"
"    \"   printf(GREEN \\\"[nexus]%s Compiling and linking: %s\\\\n\\\", RESET, cmd);\\n\"\n"
"    \"   result = system(cmd);\\n\"\n"
"    \"\\n\"\n"
"    \"   if (result != 0) {\\n\"\n"
"    \"       fprintf(stderr, RED \\\"[nexus]%s Compilation or linking failed with code %d\\\\n\\\", RESET, result);\\n\"\n"
"    \"       return 1;\\n\"\n"
"    \"   }\\n\"\n"
"    \"\\n\"\n"
"    \"   printf(GREEN \\\"[nexus]%s Compilation and linking successful.\\\\033[0m\\\\n\\\", RESET);\\n\"\n"
"    \"\\n\"\n"
"    \"   return 0;\\n\"\n"
"    \"}\\n\";\n"
"\n"
"const char* main_template = \n"
"    \"#include <stdio.h>\\n\\n\"\n"
"    \"int main() {\\n\"\n"
"    \"   printf(\\\"Hello, World!\\\\n\\\");\\n\"\n"
"    \"   return 0;\\n\"\n"
"    \"}\\n\";\n"
"\n"
"const char* lib_template = \n"
"    \"/*\\n\"\n"
"    \"   Add library code in this file.\\n\"\n"
"    \"   Add headers for associated .c files in include/\\n\"\n"
"    \"*/\\n\";\n"
"\n"
"const char* color_codes_template =\n"
"    \"#ifndef COLOR_CODES_H\\n\"\n"
"    \"#define COLOR_CODES_H\\n\"\n"
"    \"\\n\"\n"
"    \"#define GREEN \\\"\\\\033[0;32m\\\"\\n\"\n"
"    \"#define RED \\\"\\\\033[0;31m\\\"\\n\"\n"
"    \"#define YELLOW \\\"\\\\033[0;33m\\\"\\n\"\n"
"    \"#define RESET \\\"\\\\033[0m\\\"\\n\"\n"
"    \"\\n\"\n"
"    \"#endif\\n\";\n"
"\n"
"const char* gitignore_template =\n"
"    \"# --- Ignore Build Directory ---\\n\"\n"
"    \"# Ignore binary output\\n\"\n"
"    \"bin/\\n\"\n"
"    \"# Ignore compiled files\\n\"\n"
"    \"*.exe\\n\"\n"
"    \"# Ignore IDE specific files\\n\"\n"
"    \".vscode/\\n\";\n"
"\n"
"int get_proj_attr(FILE* nex_file, char* buffer, size_t size, const char* proj_attr) {\n"
"    char line[256] = {0};\n"
"    char tmp_buffer[256] = {0};\n"
"\n"
"    while (fgets(line, sizeof(line), nex_file)) {\n"
"        if (sscanf(line, proj_attr, tmp_buffer) == 1) {\n"
"            tmp_buffer[strcspn(tmp_buffer, \"\\r\\n\")] = 0;  // Strip newline\n"
"            strncat(buffer, tmp_buffer, size - strlen(buffer) - 1);\n"
"            return 0;\n"
"        }\n"
"    }\n"
"\n"
"    fprintf(stderr, \"Attribute not found in .nexus file\\n\");\n"
"    return 12;\n"
"}\n";




const char* build_template_library =
    "#include <stdio.h>\n"
    "#include <stdlib.h>\n"
    "#include <string.h>\n"
    "#include \"../src_files.h\"\n"
    "#include \"color_codes.h\"\n"
    "\n"
    "int main() {\n"
    "    char obj_cmd[512];\n"
    "    char base[64];\n"
    "\n"
    "    for (int i = 0; files[i] != NULL; i++) {\n"
    "        const char* path = files[i];\n"
    "        const char* slash = strrchr(path, '/');\n"
    "        if (!slash) continue;\n"
    "        slash++;\n"
    "\n"
    "        const char* dot = strrchr(slash, '.');\n"
    "        if (!dot) continue;\n"
    "        int len = dot - slash;\n"
    "        strncpy(base, slash, len);\n"
    "        base[len] = '\\0';\n"
    "\n"
    "        snprintf(obj_cmd, sizeof(obj_cmd),\n"
    "            \"gcc -c -Iinclude %s -o build/%s.o\",\n"
    "            path, base);\n"
    "\n"
    "        printf(GREEN \"[nexus]%s Compiling: %s\\n\", RESET, obj_cmd);\n"
    "        if (system(obj_cmd) != 0) {\n"
    "            fprintf(stderr, RED \"[nexus]%s Compilation failed: %s\\n\", RESET, path);\n"
    "            return 1;\n"
    "        }\n"
    "    }\n"
    "\n"
    "    printf(GREEN \"[nexus]%s Archiving static library: lib/libmylib.a\\n\", RESET);\n"
    "    int res = system(\"ar rcs lib/libmylib.a build/*.o\");\n"
    "    if (res != 0) {\n"
    "        fprintf(stderr, RED \"[nexus]%s Archiving failed.\\n\", RESET);\n"
    "        return 1;\n"
    "    }\n"
    "\n"
    "    printf(GREEN \"[nexus]\\033[0m Library built successfully.\\n\");\n"
    "    return 0;\n"
    "}\n";





const char* build_template_executable = 
    "#include <stdio.h>\n"
    "#include <stdlib.h>\n"
    "#include \"color_codes.h\"\n"
    "#include \"../src_files.h\"\n"
    "#include \"../include/templates.h\"\n"
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
    "   char out_name[256] = \"bin/\";\n"
    "   int result = get_proj_attr(fopen(\".nexus\", \"r\"), out_name, 256, PROJ_NAME_ATTR);\n"
    "   if (result != 0) { return 12; }\n"
    "\n"
    "   char cmd[2048] = {0};\n"
    "\n"
    "   strcat(cmd, comp);\n"
    "   strcat(cmd, out_name);\n"
    "\n"
    "   int i = 0;\n"
    "   while (files[i] != NULL) {\n"
    "       strcat(cmd, \" \" );\n"
    "       strcat(cmd, files[i]);\n"
    "       i++;\n"
    "   }\n"
    "\n"
    "   printf(GREEN \"[nexus]%s Compiling and linking: %s\\n\", RESET, cmd);\n"
    "   result = system(cmd);\n"
    "\n"
    "   if (result != 0) {\n"
    "       fprintf(stderr, RED \"[nexus]%s Compilation or linking failed with code %d\\n\", RESET, result);\n"
    "       return 1;\n"
    "   }\n"
    "\n"
    "   printf(GREEN \"[nexus]%s Compilation and linking successful.\\033[0m\\n\", RESET);\n"
    "\n"
    "   return 0;\n"
    "}\n";





const char* main_template = 
    "#include <stdio.h>\n\n"
    "int main() {\n"
    "   printf(\"Hello, World!\\n\");\n"
    "   return 0;\n"
    "}\n";





const char* lib_template = 
    "/*\n"
    "   Add library code in this file.\n"
    "   Add headers for associated .c files in include/\n"
    "*/\n";





const char* color_codes_template =
    "#ifndef COLOR_CODES_H\n"
    "#define COLOR_CODES_H\n"
    "\n"
    "#define GREEN \"\\033[0;32m\"\n"
    "#define RED \"\\033[0;31m\"\n"
    "#define YELLOW \"\\033[0;33m\"\n"
    "#define RESET \"\\033[0m\"\n"
    "\n"
    "#endif\n";





const char* gitignore_template =
    "# --- Ignore Build Directory ---\n"
    "# Ignore binary output\n"
    "bin/\n"
    "# Ignore compiled files\n"
    "*.exe\n"
    "# Ignore IDE specific files\n"
    ".vscode/\n";





int get_proj_attr(FILE* nex_file, char* buffer, size_t size, const char* proj_attr) {
    char line[256] = {0};
    char tmp_buffer[256] = {0};

    while (fgets(line, sizeof(line), nex_file)) {
        // Look for proj_attr at the start
        if (sscanf(line, proj_attr, tmp_buffer) == 1) {
            tmp_buffer[strcspn(tmp_buffer, "\r\n")] = 0;  // Strip newline
            strncat(buffer, tmp_buffer, size - strlen(buffer) - 1);
            return 0;
        }
    }

    fprintf(stderr, "Attribute not found in .nexus file\n");
    return 12;
}





const char* header_template =
    "#ifndef TEMPLATES_H\n"
    "#define TEMPLATES_H\n"
    "\n"
    "#include <stdio.h>\n"
    "#include <string.h>\n"
    "#include \"../nexus_build/color_codes.h\"\n"
    "\n"
    " const char* build_template_library;\n"
    " const char* build_template_executable;\n"
    " const char* main_template;\n"
    " const char* lib_template;\n"
    " const char* color_codes_template;\n"
    " const char* gitignore_template;\n"
    " const char* all_templates;\n"
    " const char* header_template;\n"
    "\n"
    "int get_proj_attr(FILE* nex_file, char* buffer, size_t size, const char* proj_attr);\n"
    "\n"
    "#endif\n";
