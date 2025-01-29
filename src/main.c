#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define MAXCONNS 10

int lsocket;
int *conn_pool;
int childs;

char *getMessage(int file_descriptor) {

    FILE *socket_stream;

    // Open the stream with read mode
    // if( (socket_stream = fdopen(file_descriptor, "r")) == NULL ) {
    //     fprintf(stderr, "Failed opening the socket stream");
    //     exit(EXIT_FAILURE);
    // }

    size_t size = 1;
    char *line;
    char *aux;

    if( (line = (char*) malloc(sizeof(char) * size)) == NULL ) {
        fprintf(stderr, "Failed allocating memory\n");
        exit(EXIT_FAILURE);
    }

    if( (aux = (char*) malloc(sizeof(char) * size)) == NULL ) {
        fprintf(stderr, "Failed allocating memory\n");
        exit(EXIT_FAILURE);
    }

    char *end;

    fprintf(stdout, "Getting the message\n");
    fflush(stdout);



    while ( read(file_descriptor, aux, size) ) {
        fprintf(stdout , "%s\n", aux);
        fflush(stdout);
    }

    return "A";

}

void* connectionHandler(void* arg) {
    int conn_s = *(int*)arg;
    getMessage(conn_s);
}

void handleInterrupt(int sig) {
    fprintf(stdout, "Exiting the program. Signal: %i\n", sig);

    for (int i = 0; i <= childs; i++) {
        close(conn_pool[i]);
    }

    free(conn_pool);

    exit(1);
}

int main() {
    int conn_s;
    short int port = 8010;
    struct sockaddr_in addr;

    if( (lsocket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        fprintf(stderr, "Failed creating socket at %i\n", port);
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    socklen_t addr_size = sizeof(addr);
    
    if( (bind(lsocket, (struct sockaddr*) &addr, addr_size)) < 0 ) {
        fprintf(stderr, "Error binding the socket\n");
        exit(EXIT_FAILURE);
    }

    if( (listen(lsocket, MAXCONNS)) == -1 ) {
        fprintf(stderr, "Failed listening\n");
        exit(EXIT_SUCCESS);
    }

    childs = 0;

    while (childs <= MAXCONNS) {
        signal(SIGINT, handleInterrupt);

        if(conn_s = accept(lsocket, (struct sockaddr *) &addr, &addr_size)) {
            childs++;
            conn_pool = (int*) realloc(conn_pool, sizeof(int) * childs);
            conn_pool[childs - 1] = conn_s;
            
            pthread_t th;
            pthread_create(&th, NULL, connectionHandler, &conn_s);
            pthread_join(th, NULL);

            // Then, get the message opening a stream associated with conn_s file descriptor (fdopen)
        }
    }

    return 0;
}