#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define MAXCONNS 10

int lsocket;

char *getMessage(int file_descriptor) {
    FILE *socket_stream;

    // Open the stream with read mode
    if( (socket_stream = fdopen(file_descriptor, "r")) == NULL ) {
        fprintf(stderr, "Failed opening the socket stream");
        exit(EXIT_FAILURE);
    }

    size_t size = 1;
    char *line;
    char *aux;

    if( (line = (char*) malloc(sizeof(char) * size)) == NULL ) {
        fprintf(stderr, "Failed allocating memory");
        exit(EXIT_FAILURE);
    }

    if( (aux = (char*) malloc(sizeof(char) * size)) == NULL ) {
        fprintf(stderr, "Failed allocating memory");
        exit(EXIT_FAILURE);
    }

    char *end;

    while ( strcmp(end = (char*) getline(&aux, &size, socket_stream), "\r\n") == 0 ) {

        //TODO

    }

}

int main() {
    int conn_s;
    short int port = 8080;
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
        fprintf(stderr, "Error binding the socket");
        exit(EXIT_FAILURE);
    }

    if( (listen(lsocket, MAXCONNS)) == -1 ) {
        fprintf(stderr, "Failed listening");
        exit(EXIT_SUCCESS);
    }

    int childs = 0;
    pid_t pid;

    while (1) {

        if (childs <= MAXCONNS) {
            if( (pid = fork()) == -1 ) {
                fprintf(stderr, "Failed forking");
                exit(EXIT_FAILURE);
            }
            childs++;
        }

        if (pid == 0) {

            while (1) {
                if ( conn_s = accept(lsocket, (struct sockaddr *) &addr, &addr_size) == -1 ) {
                    fprintf(stderr, "Failes accepting a connection");
                    exit(EXIT_FAILURE);
                }

                fprintf(stdout, "Hello proccess with PID %i", pid);

                // Then, get the message opening a stream associated with conn_s file descriptor (fdopen)

            }



        }

    }



    return 0;
}