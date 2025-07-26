#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <stdio.h>
#include <string.h>
#include "../nexus_build/color_codes.h"
#include <sys/wait.h>

#define PROJ_PATH_ATTR "PATH=%225s"
#define PROJ_NAME_ATTR "ProjectName=%255s"

extern const char* all_templates;
extern const char* build_template_library;
extern const char* build_template_executable;
extern const char* main_template;
extern const char* lib_template;
extern const char* color_codes_template;
extern const char* gitignore_template;
extern const char* header_template;

int get_proj_attr(FILE* nex_file, char* buffer, size_t size, const char* proj_attr);

#endif
