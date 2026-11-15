#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"

#define LINE_BUFFER 4096

static int parseQuotedField(const char **cursor, char *out, int outSize);
static int parseBareField(const char **cursor, char *out, int outSize);
static int parseIntField(const char **cursor, int *out);
static int parseBookLine(const char *line, Book *book, int hasStock);
static void seedInventory(void);

void initLibrary(Library *library)
{
    library->books = NULL;
    library->bookCount = 0;
    library->bookCapacity = 0;
    library->accounts = NULL;
    library->accountCount = 0;
    library->accountCapacity = 0;
    library->loans = NULL;
    library->loanCount = 0;
    library->loanCapacity = 0;
    library->nextLoanId = 1;
}

static int parseQuotedField(const char **cursor, char *out, int outSize)
{
    const char *p = *cursor;
    int n = 0;

    while (*p == ' ' || *p == '\t') p++;
    if (*p != '"') return FAILURE;
    p++;
    while (*p != '\0' && *p != '"') {
        if (n < outSize - 1) out[n++] = *p;
        p++;
    }
    if (*p != '"') return FAILURE;
    p++;
    out[n] = '\0';
    *cursor = p;
    return SUCCESS;
}

static int parseBareField(const char **cursor, char *out, int outSize)
{
    const char *p = *cursor;
    int n = 0;

    while (*p == ' ' || *p == '\t') p++;
    while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') {
        if (n < outSize - 1) out[n++] = *p;
        p++;
    }
    if (n == 0) return FAILURE;
    out[n] = '\0';
    *cursor = p;
    return SUCCESS;
}

static int parseIntField(const char **cursor, int *out)
{
    char temp[32];
    if (parseBareField(cursor, temp, (int)sizeof(temp)) != SUCCESS) return FAILURE;
    *out = atoi(temp);
    return SUCCESS;
}

static int parseTextField(const char **cursor, char *out, int outSize)
{
    const char *p = *cursor;

    while (*p == ' ' || *p == '\t') p++;
    if (*p == '"') return parseQuotedField(cursor, out, outSize);
    return parseBareField(cursor, out, outSize);
}

/*
 * Books.txt format (real dataset, CRLF line endings):
 *   ocaid "title" "author" year page_count edition work url
 * title and author are quoted; all other fields are bare tokens.
 * inventory.txt adds a trailing stock integer.
 */
static int parseBookLine(const char *line, Book *book, int hasStock)
{
    const char *cursor = line;

    if (line[0] == '\0' || line[0] == '\n' || line[0] == '\r') return FAILURE;
    if (strncmp(line, "ocaid", 5) == 0) return FAILURE;

    if (parseBareField(&cursor, book->ocaid, MAX_OCAID) != SUCCESS) return FAILURE;
    if (parseTextField(&cursor, book->title, MAX_TITLE) != SUCCESS) return FAILURE;
    if (parseTextField(&cursor, book->author, MAX_AUTHOR) != SUCCESS) return FAILURE;
    if (parseIntField(&cursor, &book->year) != SUCCESS) return FAILURE;
    if (parseIntField(&cursor, &book->pageCount) != SUCCESS) return FAILURE;
    if (parseBareField(&cursor, book->edition, MAX_EDITION) != SUCCESS) return FAILURE;
    if (parseBareField(&cursor, book->work, MAX_WORK) != SUCCESS) return FAILURE;
    if (parseBareField(&cursor, book->url, MAX_URL) != SUCCESS) return FAILURE;
    if (hasStock) {
        if (parseIntField(&cursor, &book->stock) != SUCCESS) return FAILURE;
    } else {
        book->stock = DEFAULT_STOCK;
    }
    return SUCCESS;
}

static int growBooks(Library *library)
{
    Book *temp;
    int newCap;

    if (library->bookCount < library->bookCapacity) return SUCCESS;
    newCap = (library->bookCapacity == 0) ? 64 : library->bookCapacity * 2;
    temp = (Book *)realloc(library->books, (size_t)newCap * sizeof(Book));
    if (temp == NULL) return FAILURE;
    library->books = temp;
    library->bookCapacity = newCap;
    return SUCCESS;
}

static void stripLineEnd(char *line)
{
    int len = (int)strlen(line);
    if (len > 0 && line[len - 1] == '\n') { line[--len] = '\0'; }
    if (len > 0 && line[len - 1] == '\r') { line[--len] = '\0'; }
}

static void seedInventory(void)
{
    FILE *in;
    FILE *out;
    char line[LINE_BUFFER];
    Book book;
    int seeded;

    in = fopen(BOOKS_MASTER_PATH, "r");
    if (in == NULL) return;
    out = fopen(INVENTORY_PATH, "w");
    if (out == NULL) { fclose(in); return; }
    seeded = 0;
    while (fgets(line, LINE_BUFFER, in) != NULL) {
        stripLineEnd(line);
        if (parseBookLine(line, &book, 0) != SUCCESS) continue;
        if (seeded == 0) {
            fprintf(out, "ocaid \"title\" \"author\" year page_count edition work url stock\n");
            seeded = 1;
        }
        fprintf(out, "%s \"%s\" \"%s\" %d %d %s %s %s %d\n",
                book.ocaid, book.title, book.author, book.year, book.pageCount,
                book.edition, book.work, book.url, book.stock);
    }
    fclose(in);
    fclose(out);
}

static int loadBooksFrom(const char *path, Library *library, int hasStock)
{
    FILE *fp;
    char line[LINE_BUFFER];
    Book book;

    fp = fopen(path, "r");
    if (fp == NULL) return FAILURE;
    while (fgets(line, LINE_BUFFER, fp) != NULL) {
        stripLineEnd(line);
        if (parseBookLine(line, &book, hasStock) != SUCCESS) continue;
        if (growBooks(library) != SUCCESS) { fclose(fp); return FAILURE; }
        library->books[library->bookCount++] = book;
    }
    fclose(fp);
    return SUCCESS;
}

int loadBooks(Library *library)
{
    FILE *check;

    check = fopen(INVENTORY_PATH, "r");
    if (check == NULL) {
        seedInventory();
    } else {
        fclose(check);
    }
    return loadBooksFrom(INVENTORY_PATH, library, 1);
}

int saveBooks(const Library *library)
{
    FILE *fp;
    int i;

    fp = fopen(INVENTORY_PATH, "w");
    if (fp == NULL) return FAILURE;
    fprintf(fp, "ocaid \"title\" \"author\" year page_count edition work url stock\n");
    for (i = 0; i < library->bookCount; i++) {
        fprintf(fp, "%s \"%s\" \"%s\" %d %d %s %s %s %d\n",
                library->books[i].ocaid,
                library->books[i].title,
                library->books[i].author,
                library->books[i].year,
                library->books[i].pageCount,
                library->books[i].edition,
                library->books[i].work,
                library->books[i].url,
                library->books[i].stock);
    }
    fclose(fp);
    return SUCCESS;
}

static int growAccounts(Library *library)
{
    Account *temp;
    int newCap;

    if (library->accountCount < library->accountCapacity) return SUCCESS;
    newCap = (library->accountCapacity == 0) ? 8 : library->accountCapacity * 2;
    temp = (Account *)realloc(library->accounts, (size_t)newCap * sizeof(Account));
    if (temp == NULL) return FAILURE;
    library->accounts = temp;
    library->accountCapacity = newCap;
    return SUCCESS;
}

int loadAccounts(Library *library)
{
    FILE *fp;
    char line[LINE_BUFFER];
    Account account;

    fp = fopen(ACCOUNTS_PATH, "r");
    if (fp == NULL) return FAILURE;
    while (fgets(line, LINE_BUFFER, fp) != NULL) {
        stripLineEnd(line);
        if (sscanf(line, "%31s %31s %11s",
                   account.username, account.password, account.role) != 3) continue;
        if (growAccounts(library) != SUCCESS) { fclose(fp); return FAILURE; }
        library->accounts[library->accountCount++] = account;
    }
    fclose(fp);
    return SUCCESS;
}

int saveAccounts(const Library *library)
{
    FILE *fp;
    int i;

    fp = fopen(ACCOUNTS_PATH, "w");
    if (fp == NULL) return FAILURE;
    for (i = 0; i < library->accountCount; i++) {
        fprintf(fp, "%s %s %s\n",
                library->accounts[i].username,
                library->accounts[i].password,
                library->accounts[i].role);
    }
    fclose(fp);
    return SUCCESS;
}

static int growLoans(Library *library)
{
    Loan *temp;
    int newCap;

    if (library->loanCount < library->loanCapacity) return SUCCESS;
    newCap = (library->loanCapacity == 0) ? 16 : library->loanCapacity * 2;
    temp = (Loan *)realloc(library->loans, (size_t)newCap * sizeof(Loan));
    if (temp == NULL) return FAILURE;
    library->loans = temp;
    library->loanCapacity = newCap;
    return SUCCESS;
}

int loadLoans(Library *library)
{
    FILE *fp;
    char line[LINE_BUFFER];
    Loan loan;

    fp = fopen(LOANS_PATH, "r");
    if (fp == NULL) return FAILURE;
    while (fgets(line, LINE_BUFFER, fp) != NULL) {
        stripLineEnd(line);
        if (sscanf(line, "%u %31s %63s %19s %19s",
                   &loan.loanId, loan.username, loan.ocaid,
                   loan.loanDate, loan.status) != 5) continue;
        if (growLoans(library) != SUCCESS) { fclose(fp); return FAILURE; }
        library->loans[library->loanCount++] = loan;
        if (loan.loanId >= library->nextLoanId) library->nextLoanId = loan.loanId + 1;
    }
    fclose(fp);
    return SUCCESS;
}

int saveLoans(const Library *library)
{
    FILE *fp;
    int i;

    fp = fopen(LOANS_PATH, "w");
    if (fp == NULL) return FAILURE;
    for (i = 0; i < library->loanCount; i++) {
        fprintf(fp, "%u %s %s %s %s\n",
                library->loans[i].loanId,
                library->loans[i].username,
                library->loans[i].ocaid,
                library->loans[i].loanDate,
                library->loans[i].status);
    }
    fclose(fp);
    return SUCCESS;
}

void freeLibrary(Library *library)
{
    free(library->books);
    free(library->accounts);
    free(library->loans);
    library->books = NULL;
    library->accounts = NULL;
    library->loans = NULL;
    library->bookCount = 0;
    library->accountCount = 0;
    library->loanCount = 0;
    library->bookCapacity = 0;
    library->accountCapacity = 0;
    library->loanCapacity = 0;
}
