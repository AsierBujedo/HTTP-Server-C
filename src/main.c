#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define MAXCONNS 10

int lsocket;

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
    
    if( bind(lsocket, (struct sockaddr*) &addr, sizeof(addr)) < 0 ) {
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
                if ( conn_s = accept(lsocket, (struct sockaddr *) &addr, sizeof(addr)) == -1 ) {
                    fprintf(stderr, "Failes accepting a connection");
                    exit(EXIT_FAILURE);
                }

                // Then, get the message opening a stream associated with conn_s file descriptor (fdopen)

            }



        }

    }



    return 0;
}