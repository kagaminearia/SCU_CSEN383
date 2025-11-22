#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include "utils.h"

#define N   5
#define BUF 256

//accept list of pipes and monitor for outputs
void monitor_children(int pipefd[N])
{
    char linebuf[N][BUF] = {{0}};
    int  pos[N] = {0};
    int  alive  = N;

    while (alive > 0) {

        fd_set rset;
        FD_ZERO(&rset);

        int maxfd = -1;

        for (int i = 0; i < N; i++) {
            if (pipefd[i] != -1) {
                FD_SET(pipefd[i], &rset);
                if (pipefd[i] > maxfd)
                    maxfd = pipefd[i];
            }
        }

        struct timeval tv = {1, 0};

        int ready = select(maxfd + 1, &rset, NULL, NULL, &tv);
        if (ready < 0) {
            perror("select()");
            break;
        }

        for (int i = 0; i < N; i++) {

            if (pipefd[i] != -1 && FD_ISSET(pipefd[i], &rset)) {

                char buf[BUF];
                int n = read(pipefd[i], buf, BUF - 1);

                if (n <= 0) {
                    close(pipefd[i]);
                    pipefd[i] = -1;
                    alive--;
                    continue;
                }

                for (int k = 0; k < n; k++) {
                    linebuf[i][pos[i]++] = buf[k];

                    if (buf[k] == '\n') {
                        linebuf[i][pos[i]] = '\0';

                        double parent_ts = now();
                        handle_parent_output(parent_ts, linebuf[i]);

                        pos[i] = 0;
                    }
                }
            }
        }
    }
}
