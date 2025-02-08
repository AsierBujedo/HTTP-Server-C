#ifndef RHANDLER_H
#define RHANDLER_H

#include <stdio.h>

void sendIndex(FILE* rstream);
void endpointGateway(char* endpoint);
void sendResponse();

#endif // RHANDLER_H