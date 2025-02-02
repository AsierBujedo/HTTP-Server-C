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
char *getMessage(int file_descriptor, char *h_msg, size_t *h_size) {
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
    
    int h_char_count = 0;
    while (headers[h_char_count] != '\0') {
        h_char_count++;
    }

    // Initialize msg buffer
    size_t msg_size = 1;
    msg = (char *) malloc(msg_size);
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

    free(h_msg);
    h_msg = malloc(sizeof(char) * h_char_count);
    *h_size = h_char_count;
    h_msg = strcpy(h_msg, headers);

    free(headers);
    free(aux);

    return h_msg;
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
void msgToReq(request *dest, char *msg, int msgSize, int step) {

    fprintf(stdout, msg);

    if(step < 3 && msg != NULL && dest != NULL) {
        for (int i = 0; i < msgSize; i++) {
            if (msg[i] == ' ' || msg[i] == '\0' || msg[i] == '\r' || msg[i] == '\n') {
                switch (step) {
                    case 0:
                        dest->method = malloc(sizeof(char) * (i + 1));
                        if (dest->method == NULL) {
                            fprintf(stderr, "Failed allocating memory for method\n");
                            exit(EXIT_FAILURE);
                        }
                        dest->method = strncpy(dest->method, msg, i);
                        dest->method[i] = '\0';
                        step++;
                        break;
                    case 1:
                        dest->route = malloc(sizeof(char) * (i + 1));
                        if (dest->route == NULL) {
                            fprintf(stderr, "Failed allocating memory for route\n");
                            exit(EXIT_FAILURE);
                        }
                        dest->route = strncpy(dest->route, msg, i);
                        dest->route[i] = '\0';
                        step++;
                        break;
                    case 2:
                        dest->version = malloc(sizeof(char) * (i + 1));
                        if (dest->version == NULL) {
                            fprintf(stderr, "Failed allocating memory for version\n");
                            exit(EXIT_FAILURE);
                        }
                        dest->version = strncpy(dest->version, msg, i);
                        dest->version[i] = '\0';
                        step++;
                        break;
                    default:
                        break;
                }

                char *aux = malloc(sizeof(char) * (msgSize - i - 1));
                if (aux == NULL) {
                    fprintf(stderr, "Failed allocating memory\n");
                    exit(EXIT_FAILURE);
                }

                // COPY TO AUX FROM MSG[I + 1] TO MSG[MSGSIZE]
                int counter = 0;
                for (int j = i + 1; j < msgSize; j++) {
                    aux[counter] = msg[j];
                    counter++;
                }

                msgToReq(dest, aux, msgSize - i - 1, step);
                free(aux);
                break;
            }
        }
    }
}