#ifndef TARGET_TYPE_H
#define TARGET_TYPE_H

/**
 * @brief Defines whether project is built for an executable
 * context (includes main.c w/ a main function). If --lib is
 * specified in the init, a lib.c file is created instead with no
 * main function. "nexus run" command will not work for LIBRARY 
 * TargetType.
 */
typedef enum {
    EXECUTABLE,
    LIBRARY
} TargetType;

#endif