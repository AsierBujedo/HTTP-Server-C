#define _POSIX_C_SOURCE 200112L
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "http.h"

// Specifies a simple HTTP 200 response line
const char* CODE200 = "HTTP/1.1 200 OK\n";

// Reads data from a socket (file_descriptor), collects lines until "\r\n",
// and sends a response back
char *getMessage(int file_descriptor) {
    FILE *sstream;

    // Open the socket as a read-only stream
    if ((sstream = fdopen(file_descriptor, "r")) == NULL) {
        fprintf(stderr, "Failed to open socket stream\n");
        exit(EXIT_FAILURE);
    }

    size_t size = 0;
    char *headers = NULL;
    char *msg = NULL;
    char *aux = NULL;

    // Read the first line (headers)
    if (getline(&headers, &size, sstream) == -1) {
        fprintf(stderr, "Failed setting the headers\n");
        exit(EXIT_FAILURE);
    }

    // Initialize msg buffer
    size_t msg_size = 1;
    msg = (char *)malloc(msg_size);
    if (msg == NULL) {
        fprintf(stderr, "Failed allocating memory for message\n");
        exit(EXIT_FAILURE);
    }
    msg[0] = '\0';

    // Continuously read lines until reaching "\r\n"
    // Expand msg buffer as needed
    size_t aux_size = 0;
    int ln_bytes;
    while ((ln_bytes = getline(&aux, &aux_size, sstream)) > 0) {
        if (strcmp(aux, "\r\n") == 0) {
            break;
        }
        msg_size += ln_bytes;
        char *temp = realloc(msg, msg_size);
        if (temp == NULL) {
            fprintf(stderr, "Failed reallocating memory\n");
            free(msg);
            exit(EXIT_FAILURE);
        }
        msg = temp;
        strncat(msg, aux, ln_bytes);
    }

    // Reopen the same file descriptor in write mode for response
    FILE *response_stream;
    if ((response_stream = fdopen(file_descriptor, "w")) == NULL) {
        fprintf(stderr, "Failed to open socket stream for writing\n");
        exit(EXIT_FAILURE);
    }

    // Send the HTTP 200 response
    if (sendResponse(response_stream)) {
        fprintf(stderr, "Failed sending a response. The program will not end.\n");
    }

    // Print the collected message
    fprintf(stdout, "%s", headers);

    free(headers);
    free(aux);

    return msg;
}

// Writes the 200 OK response to the stream
int sendResponse(FILE *sstream) {
    if (fputs(CODE200, sstream) == EOF) {
        return -1;
    }
    fflush(sstream);
    return 0;
}

/* Receives a header and returns a request struct */
void msgToReq(request *dest, char *msg, int msgSize) {
    size_t char_count = 0;
    char *aux = malloc(sizeof(char));

    if(aux == NULL) {
        fprintf(stderr, "Failed allocating memory\n");
        exit(EXIT_FAILURE);
    }

    for(int i=0; i<=msgSize; i++) {
        char_count++;
        aux = realloc(aux, sizeof(char) * char_count);

        if(aux == NULL) {
            fprintf(stderr, "Failed reallocating memory\n");
            exit(EXIT_FAILURE);
        }

        aux[i] = msg[i];
        if(msg[i] == ' ') {
            dest->method = malloc(sizeof(char) * char_count);

            if (dest->method == NULL) {
                fprintf(stderr, "Failed allocating memory\n");
                exit(EXIT_FAILURE);
            }

            dest->method = strcpy(dest->method, aux);
            free(aux);
            aux = malloc(sizeof(char));

            if(aux == NULL) {
                fprintf(stderr, "Failed allocating memory\n");
                exit(EXIT_FAILURE);
            }

            char_count = 0;

            //TODO
        }
    }
}
