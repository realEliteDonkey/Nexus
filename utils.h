#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "target_type.h"
#include "nex_error.h"
#include "templates.h"

NEX_ERROR nexus_mkdir(const char* str);
NEX_ERROR add_src_files();
NEX_ERROR build_file(const char* file_name, const char* template);
NEX_ERROR nexus_build();
NEX_ERROR nexus_run();

#endif