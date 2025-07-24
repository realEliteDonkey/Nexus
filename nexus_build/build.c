#include <stdio.h>
#include <stdlib.h>
#include "color_codes.h"
#include "../src_files.h"
#include "../include/templates.h"
#include <string.h>

/*
    Use gcc for C files or g++ for c++
    Performs compilation and linking with -o all at once
*/

int main() {
   char comp[] = "gcc -o ";

   /*
       Provide directory and output exe name here.
       Provide relative path from the "build" directory
   */
   char out_name[256] = "bin/";  // or any size that fits your expected path

    FILE* nex_file = fopen(".nexus", "r");
    if (!nex_file) {
        perror("Failed to open .nexus file");
        return 11;
    }

    int result = get_proj_attr(nex_file, out_name, 256, PROJ_NAME_ATTR);
    fclose(nex_file);  // Always close the file
    if (result != 0) {
        return 12;
    }

   /*
       Command input buffer is 2048 chars.
       If you have a lot of files, just input whatever you need for a size. (FIX)
   */
   char cmd[2048] = {0};

   strcat(cmd, comp);
   strcat(cmd, out_name);

   int i = 0;
   while (files[i] != NULL) {
       strcat(cmd, " ");
       strcat(cmd, files[i]);
       i++;
   }

   printf(GREEN "[nexus]%s Compiling and linking: %s\n", RESET, cmd);
   result = system(cmd);

   if (result != 0) {
       fprintf(stderr, RED "[nexus]%s Compilation or linking failed with code %d\n", RESET, result);
       return 1;
   }

   printf(GREEN "[nexus]%s Compilation and linking successful.\033[0m\n", RESET);

   return 0;
}
