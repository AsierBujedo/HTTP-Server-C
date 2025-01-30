#ifndef HTTP_H_
#define HTTP_H_

typedef struct {
    char* method;
    char* route;
    char* version;
} request;

char *getMessage(int file_descriptor);
int sendResponse(FILE* sstream);

#endif /* HTTP_H_ */