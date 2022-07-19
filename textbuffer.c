
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "textbuffer.h"

typedef struct TextNode* TextLine;
struct TextNode {
    char* str;
    int size;
    TextLine next;
};

struct textbuffer {
    int nline;       // line num
    TextLine head;  // head of the given list
};

// helper function protocol
static TextLine newTextLine(char*);
static void freeTextLine(TextLine);

static void printTB(TB tb);

// dumpTB
static char* dumpTBWithLineNum(TB tb);

// addPrefix
static bool checkRange(int from, int to, TB tb);
static void addPrefixLine(TextLine txl, char* prefix);

static TextLine newTextLine(char* str) {
    TextLine line = malloc(sizeof(*line));
    line->str = strdup(str);
    line->size = strlen(str);
    line->next = NULL;
    return line;
}

/**
 * Allocate a new textbuffer whose contents is initialised with the text
 * in the given string.
 */
TB newTB(char* text) {
    // Each fragment of the string that ends with a newline character ('\n')
    // indicates a separate line in the textbuffer.
    // The whole text is terminated by '\0'.

    if (!text) {
        fprintf(stderr, "invalid text\n");
        abort();
    }
    // Unless text is the empty string, it will always have a newline at the end.
    if (strlen(text) > 0 && *(text + strlen(text) - 1) != '\n') {
        fprintf(stderr, "input text must end with \\n");
        abort();
    }

    TextLine head, prev;
    head = prev = NULL;

    int size = 0;

    // use \n to split each line(TextNode)
    char *backup, *input, *line;
    backup = input = strdup(text);

    while ((line = strsep(&input, "\n")) && input) {
        // create a new TextLine
        TextLine tl = newTextLine(line);

        // cur->prev = (cur == tb->head) ? NULL : cur->prev;
        if (!head) {
            head = tl;
        }
        // tl->prev == NULL
        else {
            prev->next = tl;
        }
        // prev refer to recently appended TextLine
        prev = tl;

        size++;
    }

    // caller is responsible for free, because input will be modified by strsep()
    free(backup);
    TB tb = malloc(sizeof(*tb));
    tb->nline = size;
    tb->head = head;

    return tb;
}

static void printTB(TB tb) {
    TextLine line = tb->head;
    while (line) {
        printf("%s\n", line->str);
        line = line->next;
    }
}

static void freeTextLine(TextLine line) {
    free(line->str);
    free(line);
}

/**
 * Free  the  memory occupied by the given TB.
 * It is an error to access the buffer afterwards.
 */
void releaseTB(TB tb) {
    TextLine cursor = tb->head;
    while (cursor != NULL) {
        TextLine next = cursor->next;
        freeTextLine(cursor);
        cursor = next;
    }
    free(tb);
    tb = NULL;
}


static int doGetByte(TextLine txl) {
    // "Hi there" -> "Hi there\n"
    if (!txl) return 0;
    return strlen(txl->str) + 1 + doGetByte(txl->next);
}

/**
 * get the num of byte occupied by the given TB, with \n each node
 */
static int getByteTB(TB tb) {
    return doGetByte(tb->head);
}


/**
 * Allocate and return a string containing all of the text in the given TB
 * If showLineNumbers is true, prepend a line number (along with a dot and space) to each line of the output.
 */
char* dumpTB(TB tb, bool showLineNumbers) {
    if (!tb) {
        fprintf(stderr, "invalid tb\n");
        abort();
    }
    /*
     * Assume:
     * line number start at 1
     * if tb has no line, return an empty string regardless of whether showLineNumbers is true or false
     * should allocate memory even if return empty string
     */
    char* str;
    // empty tb -> empty string
    if (tb->nline == 0) {
        str = calloc(1, sizeof(char));
        return str;
    }

    if (showLineNumbers) return dumpTBWithLineNum(tb);

    // get the nline of tb
    str = calloc(getByteTB(tb) + 1, sizeof(char));

    char* cur = str;
    for (TextLine txl = tb->head; txl; txl = txl->next) {
        strcat(cur, txl->str);
        cur += txl->size;
        *cur = '\n';
        cur++;
    }
    return str;
}

/**
 * Get the byte num occupied by the line number in each prefix
 */
static int nDigit(int num) {
    assert(num > 0);
    int div = 1, cnt = 0;
    while (num / div > 0) {
        div *= 10;
        cnt ++;
    }
    return cnt;
}

/**
 * return a string with a line number (along with a dot and space), which is prepend to each line
 */
static char* dumpTBWithLineNum(TB tb) {
    // if tb->nline = 123, consider every line prefix use 3+2 bytes to represent line prefix
    int lineDigit = nDigit(tb->nline);
    int bytes = tb->nline * (lineDigit + 2) + getByteTB(tb) + 1; // leave a byte for '\0'

    char* str = calloc(bytes, sizeof (char));
    char* cur = str; // cursor pointing to the right most \0 of str
    int lineNum = 1;
    for (TextLine txl = tb->head; txl; txl = txl->next) {
        // concat the target string from the position referred by the cursor
        sprintf(cur, "%d. %s\n", lineNum, txl->str);

        cur += strlen(cur);
        lineNum++;
    }

    return str;
}

/**
 * Return the number of lines of the given textbuffer.
 */
int linesTB(TB tb) {
    return tb->nline;
}


/**
 *  check whether from and to is valid.
 */
static bool checkRange(int from, int to, TB tb) {
    return from >= 1 && from <= to && to <= tb->nline;
}

/**
 * modify the string in TextLine\n
 * allocate a chunk of memory for prefix + original string in TextLine\n
 * free the original string
 */
static void addPrefixLine(TextLine txl, char* prefix) {
    assert(txl);
    // room -> goodnight room
    int prefixLen = strlen(prefix);
    int newSize = txl->size + prefixLen;
    char* newStr = calloc(newSize, sizeof(char));

    char* cursor = newStr;
    strcat(cursor, prefix);
    cursor += prefixLen;
    strcat(cursor, txl->str);

    txl->size = newSize;
    free(txl->str);
    txl->str = newStr;
}

/**
 * Add a given prefix to all lines between 'from' and 'to', inclusive.
 * - The  program  should abort() wih an error message if 'from' or 'to'
 *   is out of range. The first line of a textbuffer is at position 1.
 */
void addPrefixTB(TB tb, int from, int to, char* prefix) {
    assert(tb);
    if (!prefix) {
        fprintf(stderr, "invalid prefix\n");
        abort();
    }
    // do nothing if prefix is empty string
    if (strcmp("", prefix) == 0) return;
    if (!checkRange(from, to, tb)) {
        fprintf(stderr, "invalid range\n");
        abort();
    }

    TextLine cursor = tb->head;
    // move cursor to the from-th TextLine
    for (int i = 1; i <= to; i++, cursor = cursor->next) {
        if (i < from) continue;
        // add prefix
        addPrefixLine(cursor, prefix);
    }
}

/**
 * Merge 'tb2' into 'tb1' at line 'pos'.
 * - After this operation:
 *   - What was at line 1 of 'tb2' will now be at line 'pos' of 'tb1'.
 *   - Line  'pos' of 'tb1' will moved to line ('pos' + linesTB('tb2')),
 *     after the merged-in lines from 'tb2'.
 *   - 'tb2' can't be used anymore (as if we had used releaseTB() on it)
 * - The program should abort() with an error message if 'pos' is out of
 *   range.
 */
void mergeTB(TB tb1, int pos, TB tb2) {
}

/**
 * Copy 'tb2' into 'tb1' at line 'pos'.
 * - After this operation:
 *   - What was at line 1 of 'tb2' will now be at line 'pos' of 'tb1'.
 *   - Line  'pos' of 'tb1' will moved to line ('pos' + linesTB('tb2')),
 *     after the pasted-in lines from 'tb2'.
 *   - 'tb2' is unmodified and remains usable independent of tb1.
 * - The program should abort() with an error message if 'pos' is out of
 *   range.
 */
void pasteTB(TB tb1, int pos, TB tb2) {
}

/**
 * Cut  the lines between and including 'from' and 'to' out of the given
 * textbuffer 'tb' into a new textbuffer.
 * - The result is a new textbuffer (much as one created with newTB()).
 * - The cut lines will be deleted from 'tb'.
 * - The  program should abort() with an error message if 'from' or 'to'
 *   is out of range.
 */
TB cutTB(TB tb, int from, int to) {
    return NULL;
}

/**
 * Return  a  linked list of match nodes containing the positions of all
 * of the matches of string 'search' in 'tb'.
 * - The textbuffer 'tb' should remain unmodified.
 * - The user is responsible for freeing the returned list.
 */
Match searchTB(TB tb, char* search) {
    return NULL;
}

/**
 * Remove  the  lines between 'from' and 'to' (inclusive) from the given
 * textbuffer 'tb'.
 * - The  program should abort() with an error message if 'from' or 'to'
 *   is out of range.
 */
void deleteTB(TB tb, int from, int to) {
    assert(tb);
    // If to < from, abort
    if (!checkRange(from, to, tb)) {
        fprintf(stderr, "invalid range\n");
        abort();
    }
    // delete TextLine and manage link
    if (from == 1) {
        // should back up head for free memory
        TextLine txl = tb->head;
        TextLine backup = txl;

        // move txl (to-1) time, makes txl->next refer to the to-th TextLine
        while (--to) txl = txl->next;
        tb->head = txl->next;
        txl->next = NULL;
        freeTextLine(backup);
    } else if (to == tb->nline) {
        TextLine txl = tb->head;
        // move txl to the (from-1)-th TextLine
        from--;
        while (--from) txl = txl->next;
        TextLine backup = txl->next;
        txl->next = NULL;
        freeTextLine(backup);
    } else {
            // from !=1 and to != tb->nline
            TextLine t1 = tb->head;
            TextLine t2 = t1;
            for (int i = 1; i < to - 1; i++) {
                t2 = t2->next;
                if (i >= from - 1) continue;
                t1 = t1->next;
            }
            TextLine backup = t1->next;
            if (t1 == t2) {
                // from == to
                t1->next = backup->next;
                backup->next = NULL;
            } else {
                // from != to
                t1->next = t2->next;
                t2->next = NULL;
            }
            freeTextLine(backup);
        }
}

/**
 * Search  every  line of the given textbuffer for every occurrence of a
 * set of specified substitutions and alter them accordingly.
 * - Refer to the spec for details.
 */
void formRichText(TB tb) {
}

////////////////////////////////////////////////////////////////////////
// Bonus challenges

char* diffTB(TB tb1, TB tb2) {
    return NULL;
}

void undoTB(TB tb) {
}

void redoTB(TB tb) {
}
