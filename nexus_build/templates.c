#include "../include/templates.h"

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
    "   int result = get_proj_name(fopen(\".nexus\", \"r\"), out_name, 256);\n"
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

int get_proj_name(FILE* nex_file, char* buffer, size_t size) {
    char tmp_buffer[256] = "";
    if (fscanf(nex_file, "ProjectName=%255s", tmp_buffer) == EOF) {
        perror("Failed to retrieve executable name in .nexus");
        return 12;
    }
    strncat(buffer, tmp_buffer, size - strlen(buffer) - 1);
    return 0;
}
