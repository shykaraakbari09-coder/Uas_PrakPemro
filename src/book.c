#include <stdio.h>
#include <string.h>
#include "book.h"
#include "utils.h"
#include "fileio.h"

int findBookById(const Library *library, const char *ocaid)
{
    int i;

    for (i = 0; i < library->bookCount; i++) {
        if (strcmp(library->books[i].ocaid, ocaid) == 0) {
            return i;
        }
    }
    return FAILURE;
}

int addBook(Library *library)
{
    (void)library;
    printf("[addBook stub]\n");
    return FAILURE;
}

int editBook(Library *library)
{
    (void)library;
    printf("[editBook stub]\n");
    return FAILURE;
}

int deleteBook(Library *library)
{
    (void)library;
    printf("[deleteBook stub]\n");
    return FAILURE;
}

void searchBooks(const Library *library)
{
    (void)library;
    printf("[searchBooks stub]\n");
}

void showSortedCatalog(const Library *library, int availableOnly)
{
    (void)library;
    (void)availableOnly;
    printf("[showSortedCatalog stub]\n");
}
