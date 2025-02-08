#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "http/http.h"
#include "thread_pool/tpool.h"

#define MAXCONNS 10

int lsocket;
int *conn_pool;
int childs = 0;
thread_pool *pool;

void* connectionHandler(void* arg) {
    int conn_s = *(int*)arg;  
    free(arg);

    while(1) { //TODO: Autoclose connections needs to be implemented
        request *req = malloc(sizeof(request));
        char *msg = malloc(sizeof(char));
        size_t *size = malloc(sizeof(size_t));

        msg = getMessage(conn_s, msg, size);
        msgToReq(req, msg, (int) *size, 0);

        free(req);
        free(msg);
        free(size);
    }
}

void handleInterrupt(int sig) {
    fprintf(stdout, "Exiting the program. Signal: %i\n", sig);

    for (int i = 0; i < childs; i++) {
        close(conn_pool[i]);
    }
    free(conn_pool);

    endAllThreads(pool);
    free(pool);

    exit(1);
}

int main() {
    int conn_s;
    short int port = 8090;
    struct sockaddr_in addr;

    signal(SIGINT, handleInterrupt);

    if ((lsocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Failed creating socket at %i\n", port);
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    socklen_t addr_size = sizeof(addr);

    if ((bind(lsocket, (struct sockaddr*)&addr, addr_size)) < 0) {
        fprintf(stderr, "Error binding the socket\n");
        exit(EXIT_FAILURE);
    }

    if ((listen(lsocket, MAXCONNS)) == -1) {
        fprintf(stderr, "Failed listening\n");
        exit(EXIT_FAILURE);
    }

    pool = initPool();
    while (childs < MAXCONNS) {
        if((conn_s = accept(lsocket, (struct sockaddr*)&addr, &addr_size)) > 0) {
            childs++;
            conn_pool = realloc(conn_pool, sizeof(int) * childs);
            if(conn_pool == NULL) {
                fprintf(stderr, "Failed reallocating memory");
                exit(EXIT_FAILURE);
            }
            conn_pool[childs - 1] = conn_s;
        }

        pthread_t *th = malloc(sizeof(pthread_t));
        int *conn_s_ptr = malloc(sizeof(int));
        *conn_s_ptr = conn_s;
        if(pthread_create(th, NULL, connectionHandler, (void*) conn_s_ptr) != 0) {
            fprintf(stderr, "Failed to create thread\n");
            exit(EXIT_FAILURE);
        }
        addThread(pool, th);
    }
    return 0;
}
