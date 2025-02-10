#ifndef RHANDLER_H
#define RHANDLER_H

#include <stdio.h>

void sendIndex(FILE*);
void endpointGateway(int, char*);
void sendResponse(FILE*, char*);
void sendFavicon(FILE*, char*);

#endif // RHANDLER_H