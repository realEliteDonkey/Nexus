#ifndef NEXUS_COMMANDS_H
#define NEXUS_COMMANDS_H

#include "nexus_utils.h"

NEX_ERROR nexus_init(int argc, char* argv[]);
NEX_ERROR nexus_new(const char* project_name, int argc, char* argv[]);
NEX_ERROR nexus_build();
NEX_ERROR nexus_run();
NEX_ERROR nexus_path_export();

#endif
