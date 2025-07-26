#ifndef HELP_MSG_H
#define HELP_MSG_H

const char* help_msg = 
"   --   Type \"help [name]\" for explanation of the named command   --   \n"
"\n"
"   Commands\n"
"   --------\n"
"   init\n"
"   new [proj_name]\n"
"   build\n"
"   run\n"
"   path_export\n"
"\n";

const char* help_init =
"   NEX_ERROR nexus_init(int argc, char* argv[])\n"
"\n"
"   ie. command: nexus init\n"
"   Initializes a new nexus project inside of a directory\n"
"   already created by the user.\n"
"   Project name will be set to a default name in .nexus file.\n"
"   Users may manually edit .nexus file to change this project name.\n"
"\n";

const char* help_new = 
"   NEX_ERROR nexus_new(const char* project_name, int argc, char* argv[])\n"
"\n"
"   ie. command: nexus new hello_world\n"
"   This will automatically create a new directory called hello_world\n"
"   and place all build system contents inside this new directory.\n"
"\n"
"   Acts the same as \"nexus init\", but also creates the directory for you.\n"
"\n"
"   Project name will be set to the name specified upon \"nexus new\" command.\n"
"   Users may manually edit .nexus file to change this project name.\n"
"\n";

const char* help_build = 
"   NEX_ERROR nexus_build()\n"
"\n"
"   ie. command: nexus build\n"
"   This command will automatically build a static library or executable\n"
"   depending on whether or not users initialized their project with --lib\n"
"   or not.\n"
"\n";

const char* help_run =
"   NEX_ERROR nexus_run()\n"
"\n"
"   ie. command: nexus run\n"
"   This command will auto rebuild project, and subsequently run the\n"
"   program executable located in the project bin/ directory.\n"
"\n";

#endif