#ifndef CHILD_INTERACTIVE_H
#define CHILD_INTERACTIVE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <string.h>

typedef struct {
    int child_num;
    int write_fd;
} ChildInteractiveConfig;

void child_interactive_process(int child_num, int write_fd);

#endif // CHILD_INTERACTIVE_H
