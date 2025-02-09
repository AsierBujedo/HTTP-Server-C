#include <stdlib.h>
#include <unistd.h>

#include "rhandler.h"

#define MAXLINE 100

void sendIndex(FILE* rstream) {
    FILE *f;

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        fprintf(stderr, "Current working dir: %s\n", cwd);
    } else {
        perror("getcwd() error");
    }

    if ((f = fopen("./src/http/response_handler/files/index.html", "r")) == NULL) {
        fputs("HTTP/1.1 500 Internal Server Error\r\n", rstream);
        fputs("Server: Mapache\r\n", rstream);
        fputs("Content-Type: text/html\r\n", rstream);
        fputs("\r\n", rstream);
        fputs("Failed opening file.\n", rstream);
        fflush(rstream);
        return;
    }
    fputs("HTTP/1.1 200 OK\r\n", rstream);
    fputs("Server: Mapache\r\n", rstream);
    fputs("Content-Type: text/html\r\n", rstream);
    fputs("\r\n", rstream);

    char line[MAXLINE];
    while (fgets(line, MAXLINE, f) != NULL) {
        fputs(line, rstream);
    }

    fflush(rstream);

    fclose(f);
}