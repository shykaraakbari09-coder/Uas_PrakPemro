#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "types.h"
#include "fileio.h"
#include "sort.h"

typedef void (*SortFunction)(const Library *, int *, int, SortField, int);

static int isSorted(const Library *library, const int *order, int count,
                    SortField field, int descending)
{
    int i;
    int result;

    for (i = 1; i < count; i++) {
        result = compareBooks(library, order[i - 1], order[i], field);
        if (descending) result = -result;
        if (result > 0) return 0;
    }
    return 1;
}

static int isPermutation(const int *order, int count)
{
    char *seen = (char *)calloc((size_t)count + 1, 1);
    int i;
    int ok = 1;

    if (seen == NULL) return 0;
    for (i = 0; i < count; i++) {
        if (order[i] < 0 || order[i] >= count || seen[order[i]]) { ok = 0; break; }
        seen[order[i]] = 1;
    }
    free(seen);
    return ok;
}

int main(int argc, char *argv[])
{
    static const char *algorithmNames[3] = { "bubble", "selection", "insertion" };
    static const char *fieldNames[5] = { "id", "title", "year", "pages", "stock" };
    SortFunction algorithms[3];
    Library library;
    int *orders[3];
    int count;
    int field;
    int descending;
    int a;
    int i;
    int failures = 0;
    clock_t started;
    double seconds[3];

    algorithms[0] = bubbleSort;
    algorithms[1] = selectionSort;
    algorithms[2] = insertionSort;

    initLibrary(&library);
    if (loadBooks(&library) != SUCCESS) { printf("load failed\n"); return 1; }

    count = library.bookCount;
    if (argc > 1 && atoi(argv[1]) > 0 && atoi(argv[1]) < count) count = atoi(argv[1]);
    printf("Testing %d books\n", count);

    for (a = 0; a < 3; a++) orders[a] = (int *)malloc((size_t)count * sizeof(int));

    for (field = 0; field < 5; field++) {
        for (descending = 0; descending < 2; descending++) {
            for (a = 0; a < 3; a++) {
                for (i = 0; i < count; i++) orders[a][i] = i;
                started = clock();
                algorithms[a](&library, orders[a], count, (SortField)field, descending);
                seconds[a] = (double)(clock() - started) / (double)CLOCKS_PER_SEC;
                if (!isSorted(&library, orders[a], count, (SortField)field, descending) ||
                    !isPermutation(orders[a], count)) {
                    printf("FAIL  %s %s %s\n", algorithmNames[a], fieldNames[field],
                           descending ? "desc" : "asc");
                    failures++;
                }
            }
            if (memcmp(orders[0], orders[1], (size_t)count * sizeof(int)) != 0 ||
                memcmp(orders[0], orders[2], (size_t)count * sizeof(int)) != 0) {
                printf("FAIL  algorithms disagree: %s %s\n", fieldNames[field],
                       descending ? "desc" : "asc");
                failures++;
            }
            printf("ok    %-6s %-4s  bubble %.2fs  selection %.2fs  insertion %.2fs\n",
                   fieldNames[field], descending ? "desc" : "asc",
                   seconds[0], seconds[1], seconds[2]);
        }
    }

    /* Edge cases: empty and single-element arrays must not crash. */
    for (a = 0; a < 3; a++) {
        i = 0;
        algorithms[a](&library, orders[a], 0, SORT_BY_ID, 0);
        algorithms[a](&library, orders[a], 1, SORT_BY_ID, 0);
    }

    for (a = 0; a < 3; a++) free(orders[a]);
    freeLibrary(&library);
    printf(failures == 0 ? "\nALL PASSED\n" : "\n%d FAILURES\n", failures);
    return failures != 0;
}
