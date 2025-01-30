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

#define MAXCONNS 10

int lsocket;
int *conn_pool;
pthread_t *thread_pool;
int childs;

void* connectionHandler(void* arg) {
    int conn_s = *(int*)arg;  
    free(arg);
    getMessage(conn_s);
    close(conn_s);
}

void handleInterrupt(int sig) {
    fprintf(stdout, "Exiting the program. Signal: %i\n", sig);

    for (int i = 0; i < childs; i++) {
        close(conn_pool[i]);
    }
    free(conn_pool);

    for (int i = 0; i < childs; i++) {
        pthread_join(thread_pool[i], NULL);
    }
    free(thread_pool);

    exit(1);
}

int main() {
    int conn_s;
    short int port = 8017;
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

    childs = 0;

    while (childs < MAXCONNS) {
        if ((conn_s = accept(lsocket, (struct sockaddr*)&addr, &addr_size)) > 0) {
            childs++;

            int* conn_copy = malloc(sizeof(int));
            if (conn_copy == NULL) {
                fprintf(stderr, "Failed to allocate memory for conn_copy\n");
                exit(EXIT_FAILURE);
            }
            *conn_copy = conn_s;

            int* temp_conn_pool = realloc(conn_pool, sizeof(int) * childs);
            if (temp_conn_pool == NULL) {
                fprintf(stderr, "Failed reallocating memory for conn_pool\n");
                free(conn_copy);
                exit(EXIT_FAILURE);
            }
            conn_pool = temp_conn_pool;

            conn_pool[childs - 1] = conn_s;

            pthread_t* temp_thread_pool = realloc(thread_pool, sizeof(pthread_t) * childs);
            if (temp_thread_pool == NULL) {
                fprintf(stderr, "Failed reallocating memory for thread_pool\n");
                free(conn_copy);
                exit(EXIT_FAILURE);
            }
            thread_pool = temp_thread_pool;

            pthread_t th;
            thread_pool[childs - 1] = th;

            fprintf(stdout, "Current number of threads: %i\n", childs);

            if (pthread_create(&th, NULL, connectionHandler, conn_copy) != 0) {
                fprintf(stderr, "Failed to create thread\n");
                free(conn_copy);
                exit(EXIT_FAILURE);
            }
        }
    }
    return 0;
}
