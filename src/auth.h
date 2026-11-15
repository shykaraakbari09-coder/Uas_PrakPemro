#ifndef AUTH_H
#define AUTH_H

#include "types.h"

int authenticate(const Library *library, const char *username, const char *password);
int login(const Library *library);

#endif
