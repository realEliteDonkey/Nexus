#ifndef NEXUS_UTILS_H
#define NEXUS_UTILS_H

#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include "nex_error.h"
#include "templates.h"
#include "../nexus_build/color_codes.h"

NEX_ERROR nexus_mkdir(const char* str);
NEX_ERROR write_src_files_recursive(FILE *src_files, const char *base_path, const char *relative_path);
NEX_ERROR add_src_files();
NEX_ERROR build_file(const char* file_name, const char* template);
NEX_ERROR git_installed();
NEX_ERROR nexus_git_init();

#endif
