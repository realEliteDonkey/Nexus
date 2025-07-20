#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "target_type.h"
#include "nex_error.h"
#include "templates.h"
#include "../nexus_build/color_codes.h"

NEX_ERROR nexus_mkdir(const char* str);
NEX_ERROR write_src_files_recursive(FILE *src_files, const char *base_path, const char *relative_path);
NEX_ERROR add_src_files();
NEX_ERROR build_file(const char* file_name, const char* template);
NEX_ERROR nexus_build();
NEX_ERROR nexus_run();
NEX_ERROR nexus_new(const char* project_name, int argc, char* argv[]);
NEX_ERROR nexus_init(int argc, char* argv[]);
NEX_ERROR git_installed();
NEX_ERROR nexus_git_init();
NEX_ERROR nexus_path_export();


#endif
