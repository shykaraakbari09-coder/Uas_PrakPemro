#include <stdio.h>
#include <string.h>
#include "auth.h"
#include "utils.h"

int authenticate(const Library *library, const char *username, const char *password)
{
    int i;

    for (i = 0; i < library->accountCount; i++) {
        if (strcmp(library->accounts[i].username, username) == 0 &&
            strcmp(library->accounts[i].password, password) == 0) {
            return i;
        }
    }
    return FAILURE;
}

int login(const Library *library)
{
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int attempt;
    int idx;

    for (attempt = 1; attempt <= LOGIN_ATTEMPTS; attempt++) {
        printf("\n=== LOGIN (percobaan %d/%d) ===\n", attempt, LOGIN_ATTEMPTS);
        if (readString("Username: ", username, MAX_USERNAME) != SUCCESS) {
            return FAILURE;
        }
        if (readString("Password: ", password, MAX_PASSWORD) != SUCCESS) {
            return FAILURE;
        }
        idx = authenticate(library, username, password);
        if (idx != FAILURE) {
            printf("Selamat datang, %s (%s).\n",
                   library->accounts[idx].username,
                   library->accounts[idx].role);
            return idx;
        }
        printf("Username atau password salah.\n");
    }
    return FAILURE;
}
