#ifndef NEX_ERROR_H
#define NEX_ERROR_H

typedef enum {
    SUCCESS = 0,
    ERR_FAILED_TO_OPEN,
    ERR_FAILED_TO_WRITE,
    ERR_DIR_NOT_FOUND,
    ERR_MKDIR_FAILED,
    ERR_CMD_NOT_FOUND,
    ERR_NEX_ARG_COUNT,
    ERR_BUILD_FAIL,
    ERR_RUN_FAIL,
} NEX_ERROR;

#endif