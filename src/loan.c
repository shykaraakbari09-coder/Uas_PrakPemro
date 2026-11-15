#include <stdio.h>
#include "loan.h"
#include "utils.h"
#include "fileio.h"
#include "book.h"

int borrowBook(Library *library, const char *username)
{
    (void)library;
    (void)username;
    printf("[borrowBook stub]\n");
    return FAILURE;
}

int returnBook(Library *library, const char *username)
{
    (void)library;
    (void)username;
    printf("[returnBook stub]\n");
    return FAILURE;
}

void showMyLoans(const Library *library, const char *username)
{
    (void)library;
    (void)username;
    printf("[showMyLoans stub]\n");
}

void showAllLoans(const Library *library)
{
    (void)library;
    printf("[showAllLoans stub]\n");
}
