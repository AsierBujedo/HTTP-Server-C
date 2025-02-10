#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>

#include "rhandler.h"

#define MAXLINE 100
#define MAXDIRBUFF 1024

void sendIndex(FILE* rstream) {

    FILE *f;

    char* dir = malloc(MAXDIRBUFF);
    if (dir == NULL) {
        fprintf(stderr, "Failed allocating memory for dir.\n");
        fputs("HTTP/1.1 500 Internal Server Error\r\n", rstream);
        fputs("Content-Type: text/plain\r\n", rstream);
        fputs("\r\n", rstream);
        fputs("Failed allocating memory for dir.\n", rstream);
        fflush(rstream);
        fclose(rstream);
        return;
    }

    if (getcwd(dir, MAXDIRBUFF) == NULL) {
        fprintf(stderr, "Failed getting current working directory.\n");
        fputs("HTTP/1.1 500 Internal Server Error\r\n", rstream);
        fputs("Content-Type: text/plain\r\n", rstream);
        fputs("\r\n", rstream);
        fputs("Failed getting current working directory.\n", rstream);
        fflush(rstream);
        free(dir);
        fclose(rstream);
        return;
    }

    if (strcat(dir, "/files/index.html") == NULL) {
        fprintf(stderr, "Failed concatenating file dir.\n");
        fputs("HTTP/1.1 500 Internal Server Error\r\n", rstream);
        fputs("Content-Type: text/plain\r\n", rstream);
        fputs("\r\n", rstream);
        fputs("Failed concatenating file dir.\n", rstream);
        fflush(rstream);
        free(dir);
        fclose(rstream);
        return;
    }

    if ((f = fopen(dir, "r")) == NULL) {
        fprintf(stderr, "Failed opening file: %s\n", dir);
        fputs("HTTP/1.1 500 Internal Server Error\r\n", rstream);
        fputs("Content-Type: text/plain\r\n", rstream);
        fputs("\r\n", rstream);
        fputs("Failed opening file.\n", rstream);
        fflush(rstream);
        free(dir);
        fclose(rstream);
        return;
    }

    fputs("HTTP/1.1 200 OK\r\n", rstream);
    fputs("Content-Type: text/html\r\n", rstream);
    fputs("\r\n", rstream);

    char line[MAXLINE];
    while (fgets(line, MAXLINE, f) != NULL) {
        fputs(line, rstream);
    }

    fflush(rstream);
    fclose(rstream);
    fclose(f);
    free(dir);
}

void endpointGateway(int fd, char *endpoint) {

    FILE *rstream;
    if ((rstream = fdopen(fd, "w")) == NULL) {
        fprintf(stderr, "Failed to open socket stream for writing\n");
        exit(EXIT_FAILURE);
    }

    char* dir = malloc(MAXDIRBUFF);
    if (dir == NULL) {
        fprintf(stderr, "Failed allocating memory for dir.\n");
        fputs("HTTP/1.1 500 Internal Server Error\r\n", rstream);
        fputs("Content-Type: text/plain\r\n", rstream);
        fputs("\r\n", rstream);
        fputs("Failed allocating memory for dir.\n", rstream);
        fflush(rstream);
        fclose(rstream);
        return;
    }
    if (getcwd(dir, MAXDIRBUFF) == NULL) {
        fprintf(stderr, "Failed getting current working directory.\n");
        fputs("HTTP/1.1 500 Internal Server Error\r\n", rstream);
        fputs("Content-Type: text/plain\r\n", rstream);
        fputs("\r\n", rstream);
        fputs("Failed getting current working directory.\n", rstream);
        fflush(rstream);
        free(dir);
        fclose(rstream);
        return;
    }
    if (strcat(dir, "/files/") == NULL) {
        fprintf(stderr, "Failed concatenating file dir.\n");
        fputs("HTTP/1.1 500 Internal Server Error\r\n", rstream);
        fputs("Content-Type: text/plain\r\n", rstream);
        fputs("\r\n", rstream);
        fputs("Failed concatenating file dir.\n", rstream);
        fflush(rstream);
        free(dir);
        fclose(rstream);
        return;
    }

    fprintf(stdout, "DIR %s\n", dir);

    if(strcmp(endpoint, "/favicon.ico") == 0) {
        dir = strcat(dir, "favicon.ico");
        sendFavicon(rstream, dir);
        free(dir);
        fclose(rstream);
        return;
    } else if (strcmp(endpoint, "/") == 0) {
        sendIndex(rstream);
        return;
    }

    DIR *d;
    struct dirent *sdir;

    d = opendir(dir);
    
    if(d) {
        while( (sdir = readdir(d)) != NULL ) {
            char *aux = malloc(MAXLINE);
            if (aux == NULL) {
                fprintf(stderr, "Failed allocating memory for aux.\n");
                fputs("HTTP/1.1 500 Internal Server Error\r\n", rstream);
                fputs("Content-Type: text/plain\r\n", rstream);
                fputs("\r\n", rstream);
                fputs("Failed allocating memory for aux.\n", rstream);
                fflush(rstream);
                fclose(rstream);
                continue;
            }
            int i = 1;
            aux[0] = '/';
            for (int j = 0; sdir->d_name[j] != '\0'; j++) {
                char caux = sdir->d_name[j];
                if(caux == '.' || caux == '\0' || caux == '\n' || caux == '\r') {
                    break;
                }
                aux[i] = caux;
                i++;
            }
            aux[i] = '\0';

            if (strcmp(aux, endpoint) == 0) {
                char* absdir = malloc(MAXLINE);
                if(absdir == NULL) {
                    fprintf(stderr, "Failed allocating memory for absdir.\n");
                    fputs("HTTP/1.1 500 Internal Server Error\r\n", rstream);
                    fputs("Content-Type: text/plain\r\n", rstream);
                    fputs("\r\n", rstream);
                    fputs("Failed allocating memory for absdir.\n", rstream);
                    fflush(rstream);
                    free(aux);
                    closedir(d);
                    free(dir);
                    fclose(rstream);
                    return;
                }
                absdir = strcat(absdir, dir);
                absdir = strcat(absdir, sdir->d_name);
                fprintf(stdout, "%s\n", absdir);
                sendResponse(rstream, absdir);
                free(absdir);
            }
            free(aux);
        }
        closedir(d);
    } else {
        fprintf(stderr, "Failed opening directory: %s\n", dir);
        fputs("HTTP/1.1 500 Internal Server Error\r\n", rstream);
        fputs("Content-Type: text/plain\r\n", rstream);
        fputs("\r\n", rstream);
        fputs("Failed opening directory.\n", rstream);
        fflush(rstream);
        fclose(rstream);
    }
    free(dir);
}

void sendResponse(FILE* rstream, char* endpointdir) {
    FILE *f;

    if ((f = fopen(endpointdir, "r")) == NULL) {
        fprintf(stderr, "Failed opening file: %s\n", endpointdir);
        fputs("HTTP/1.1 500 Internal Server Error\r\n", rstream);
        fputs("Content-Type: text/plain\r\n", rstream);
        fputs("\r\n", rstream);
        fputs("Failed opening file.\n", rstream);
        fflush(rstream);
        fclose(rstream);
        return;
    }

    fputs("HTTP/1.1 200 OK\r\n", rstream);
    fputs("Content-Type: text/html\r\n", rstream);
    fputs("\r\n", rstream);

    char line[MAXLINE];
    while (fgets(line, MAXLINE, f) != NULL) {
        fputs(line, rstream);
    }

    fflush(rstream);
    fclose(f);
    fclose(rstream);
}

void sendFavicon(FILE* rstream, char* dir) {
    FILE *f;

    if ((f = fopen(dir, "r")) == NULL) {
        fprintf(stderr, "Failed opening file: %s\n", dir);
        fputs("HTTP/1.1 500 Internal Server Error\r\n", rstream);
        fputs("Content-Type: text/plain\r\n", rstream);
        fputs("\r\n", rstream);
        fputs("Failed opening file.\n", rstream);
        fflush(rstream);
        return;
    }

    fputs("HTTP/1.1 200 OK\r\n", rstream);
    fputs("Content-Type: image/x-icon\r\n", rstream);
    fputs("\r\n", rstream);

    char buffer[MAXLINE];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, MAXLINE, f)) > 0) {
        fwrite(buffer, 1, bytesRead, rstream);
    }

    fflush(rstream);
    fclose(f);
}