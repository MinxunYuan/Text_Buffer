// testTextbuffer.c
// A stub file for you to test your textbuffer implementation.
// Note that you will NOT be submitting this - this is simply for you to
// test  your  functions as you go. We will have our own testTexbuffer.c
// for testing your submission.

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "textbuffer.h"

static void testNewTB(void);

// TODO: Add more function prototypes
static void testNewTB_Empty();
static void testNewTB_Normal();
static void testNewTB_Dup();

// dumpTB
static void testDumpTB();
static void testDumpTB_Show_Line_Num();

// addPrefixTB
static void testAddPrefixTB();

// deleteTB
static void testDeleteTB();

int main(void) {
	
	testNewTB();
    testDumpTB();
    testAddPrefixTB();
    testDeleteTB();
	// TODO: Call more test functions
	
	printf("All tests passed! You are awesome!\n");
}

static void testNewTB() {
    printf("-----------------------------------------\n"
           "               newTB tests               \n"
           "-----------------------------------------\n");
    testNewTB_Empty();
    testNewTB_Normal();
    testNewTB_Dup();
    printf("newTB tests passed!\n\n");

}

static void testNewTB_Empty() {
    // Create an empty TB if input text is an empty string
    puts("testing testNewTB_Empty:");

    TB tb = newTB("");
    assert(linesTB(tb) == 0);
    char* text = dumpTB(tb, false);
//    printf("%ld\n", strlen(text));
    assert(strcmp(text, "") == 0);
    free(text);
    releaseTB(tb);

    puts("testNewTB_Empty passed!");
}

static void testNewTB_Dup() {
    puts("testNewTB_Dup:");
    TB tb = newTB("aaa\n\nbbb\n\n\n");
    assert(linesTB(tb) == 5);
    char* text = dumpTB(tb, false);
    assert(strcmp(text, "aaa\n\nbbb\n\n\n") == 0);
    free(text);
    releaseTB(tb);
    puts("testNewTB_Dup passed!");

}

static void testNewTB_Normal() {
    puts("testing testNewTB_Normal:");
	// Calling dumpTB immediately after newTB, without modifying the TB
	TB tb1 = newTB("hello there,\nhow\nare\nthings\n");
	assert(linesTB(tb1) == 4);

    char *text1 = dumpTB(tb1, false); // Don't show line numbers
//    puts(text1);
    assert(strcmp("hello there,\nhow\nare\nthings\n", text1) == 0);
    free(text1);

	releaseTB(tb1);
    puts("testNewTB_Normal passed!");
	
}

static void testDumpTB() {
    printf("-----------------------------------------\n"
           "               dumpTB tests               \n"
           "-----------------------------------------\n");
    testDumpTB_Show_Line_Num();

    printf("newTB tests passed!\n\n");
}

static void testDumpTB_Show_Line_Num() {
    // Create an empty TB if input text is an empty string
    puts("testing testDumpTB_Show_Line_Num (empty tb):");

    TB tb = newTB("");
    assert(linesTB(tb) == 0);
    char* text = dumpTB(tb, true);
    //    printf("%ld\n", strlen(text));
    assert(strcmp(text, "") == 0);
    free(text);
    releaseTB(tb);

    // less than 10 line
    TB tb2 = newTB("hello world\namazing\n");
    assert(linesTB(tb2) == 2);
    char* text2 = dumpTB(tb2, true);
    assert(strcmp("1. hello world\n2. amazing\n", text2) == 0);

    puts(text2);
    free(text2);
    releaseTB(tb2);

    puts("testDumpTB_Show_Line_Num passed!");
}

static void testAddPrefixTB() {
    printf("-----------------------------------------\n"
           "               addPrefixTB tests         \n"
           "-----------------------------------------\n");

    // from = to = 1
    char* text1 = "room\nmoon\ncow jumping over the moon\nlight\n";

    TB tb1 = newTB(text1);
    assert(linesTB(tb1) == 4);
    char* dup_text1 = dumpTB(tb1, false);
    assert(strcmp(text1, dup_text1) == 0);
    puts(dup_text1);

    addPrefixTB(tb1, 1, 3, "goodnight ");
    char* dup_text1_prefix = dumpTB(tb1, false);
    puts(dup_text1_prefix);

    // empty prefix

    // out of range

    printf("addPrefixTB tests passed!\n\n");
}

static void testDeleteTB() {
    printf("-----------------------------------------\n"
           "               deleteTB tests            \n"
           "-----------------------------------------\n");
    // from = to = 1
    TB tb1 = newTB("hello there,\nhow\nare\nthings\n");
    assert(linesTB(tb1) == 4);
    deleteTB(tb1, 1, 1);
    char *text1 = dumpTB(tb1, false);
    puts(text1);
    assert(strcmp("how\nare\nthings\n", text1) == 0);
    releaseTB(tb1);

    // from = 1, 1 < to < tb->nline
    TB tb2 = newTB("hello there,\nhow\nare\nthings\n");
    assert(linesTB(tb2) == 4);
    deleteTB(tb2, 1, 2);
    char* text2 = dumpTB(tb2, false);
    puts(text2);
    assert(strcmp("are\nthings\n", text2) == 0);
    releaseTB(tb2);

    // from > 1, to = tb->nline
    TB tb3 = newTB("hello there,\nhow\nare\nthings\n");
    assert(linesTB(tb3) == 4);
    deleteTB(tb3, 2, 4);
    char* text3 = dumpTB(tb3, false);
    puts(text3);
    assert(strcmp("hello there,\n", text3) == 0);
    releaseTB(tb3);

    // from > 1, to < tb->line, from == to
    TB tb4 = newTB("hello there,\nhow\nare\nthings\n");
    assert(linesTB(tb4) == 4);
    deleteTB(tb4, 3, 3);
    char* text4 = dumpTB(tb4, false);
    puts(text4);
    assert(strcmp("hello there,\nhow\nthings\n", text4) == 0);
    releaseTB(tb4);

    // from = to = tb->nline
    TB tb5 = newTB("hello there,\nhow\nare\nthings\n");
    assert(linesTB(tb5) == 4);
    deleteTB(tb5, linesTB(tb5), linesTB(tb5));
    char* text5 = dumpTB(tb5, false);
    puts(text5);
    assert(strcmp("hello there,\nhow\nare\n", text5) == 0);
    releaseTB(tb5);

    // from = 1, to = tb->nline
    TB tb6 = newTB("hello there,\nhow\nare\nthings\n");
    assert(linesTB(tb6) == 4);
    deleteTB(tb6, 1, linesTB(tb6));
    char* text6 = dumpTB(tb6, false);
    puts(text6);
    assert(strcmp("", text6) == 0);
    releaseTB(tb6);

    printf("deleteTB tests passed!\n\n");
}
