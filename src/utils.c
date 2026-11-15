#include <stdio.h>
#include "utils.h"

int readLine(char *buffer, int bufferSize)
{
    (void)buffer;
    (void)bufferSize;
    return FAILURE;
}

int readInt(const char *prompt, int minimum, int maximum, int *result)
{
    (void)prompt;
    (void)minimum;
    (void)maximum;
    (void)result;
    return FAILURE;
}

int readString(const char *prompt, char *buffer, int bufferSize)
{
    (void)prompt;
    (void)buffer;
    (void)bufferSize;
    return FAILURE;
}

int compareIgnoreCase(const char *first, const char *second)
{
    (void)first;
    (void)second;
    return 0;
}

int containsIgnoreCase(const char *text, const char *pattern)
{
    (void)text;
    (void)pattern;
    return 0;
}

void getCurrentDate(char *destination, int destinationSize)
{
    (void)destination;
    (void)destinationSize;
}

void truncateForDisplay(const char *source, char *destination, int maxLength)
{
    (void)source;
    (void)destination;
    (void)maxLength;
}
