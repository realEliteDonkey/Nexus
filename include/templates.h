#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <stdio.h>
#include <string.h>
#include "../nexus_build/color_codes.h"

extern const char* build_template_library;
extern const char* build_template_executable;
extern const char* main_template;
extern const char* lib_template;
extern const char* color_codes_template;
extern const char* gitignore_template;
extern const char* all_templates;
extern const char* header_template;

int get_proj_name(FILE* nex_file, char* buffer, size_t size);
int get_proj_path(FILE* nex_file, char* path, size_t size);

#endif
