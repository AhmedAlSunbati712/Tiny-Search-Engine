/**
 * index.c              Ahmed Al Sunbati
 * 
 * Implementation of an inverted index data structure using a hashtable, where each word maps
 * to a set of counters. Each counter tracks the number of times a word appears in a specific document.
 * Implements the following functionality; creating a new index with a fixed number of slots;
 * inserting word-document-count entries; looking up counters for a given word; saving an index
 * to a file in a readable format; loading an index from a file
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "counters.h"
#include "mem.h"
#include "file.h"
#include "index.h"
#include "word.h"
// Aliasing hashtable_t to index_t
typedef hashtable_t index_t;

static void counters_delete_helper(void *item);
static void counters_save_item(void *fp, const int docID, const int count);
static void index_save_item(void *fp, const char *word, void *item);
/**
 * Description: Creates a new index with # num_slots
 * @param num_slots: Number of slots to allocate in the index
 * @returns A pointer to an index object
*/
index_t* index_new(const int num_slots) {
    return hashtable_new(num_slots);
}

/**
 * Description: Indexes a (document, count) pair into the given word and index
 * @param index: A pointer to an index.
 * @param word: The word to be indexed.
 * @param docID: The id of the document it appears in.
 * @param count: The number of occurences of that word in the document with ID 'docID'.
 * 
 * @returns true if insertion succeeded; false if any of the params was NULL or not eligible
*/
bool index_insert(index_t *index, const char *word, const int docID, const int count) {
    // Checking eligibilty of the parameters
    if (!index || !word || docID < 0 || count < 0) return false;

    // Try to find the counters set associated with this word
    counters_t *ctr = hashtable_find(index, word);
    if (ctr == NULL) {
        // If it hasn't been initialized yet. Initialize it and add it to the hashtable
        ctr = counters_new();
        hashtable_insert(index, word, ctr);
    }
    // Add a counter with the respective docID as key and count as value.
    counters_set(ctr, docID, count);
    return true;
}


counters_t* index_find(index_t* index, const char* word){
    return (counters_t*)hashtable_find(index, word);
}

/**
 * Description: Saves an index object into a file with the name filename
 * @param index: A pointer to an index object.
 * @param filename: A string with the filename to save the index content to.
 * 
 * @returns true if the index was saved successfully, or false if: - Any of the params is null
 *          or failed to open a file with name filename
*/
bool index_save(index_t *index, const char *filename) {
    if (!index || !filename) {return false;}
    FILE *fp = fopen(filename, "w");
    if (!fp) return false;
    // Iterate through each word to iterate through its counter set
    hashtable_iterate(index, fp, index_save_item);
    fclose(fp);
    return true;
}

/**
 * Description: Loads index from a given file.
 * @param filename: String with the filename to be read from.
 * 
 * @returns A pointer to an index object.
*/
index_t* index_load(const char* filename) {
    // Checking the filename is not Null
    if (!filename) return NULL;
    // Opening the file and reading it
    FILE* fp = fopen(filename, "r");
    // Making sure the pointer is not NULL
    if (fp == NULL) return NULL;

    // Helps in knowing how many slots to allocate in the index
    int nlines = file_numLines(fp);
    index_t* index = index_new(nlines);

    char* line;
    while ((line = file_readLine(fp)) != NULL) {
        char word[200]; // buffer to hold content of the line
        int docID, count;
        int numChars; // Will use to advance as needed through the line

        // Get the first word
        if (sscanf(line, "%s%n", word, &numChars) != 1) {
            // If the number of items parsed is not correct, free the line
            free(line);
            continue;
        }
        counters_t* ctrs = counters_new(); // Initialize a counters set for this word

        // Parse (docID, count) pairs from the rest of the line
        char* ptr = line + numChars; // Pointer to hold where we are right now in the string
        while (sscanf(ptr, "%d %d%n", &docID, &count, &numChars) == 2) {
            // Inserting a counter with (docID, count) pair
            counters_set(ctrs, docID, count);
            ptr += numChars; // Advancing by the number of characters we have read
        }
        // Inserting the word and its counters set into the index
        hashtable_insert(index, word, ctrs);
        // Freeing the line after using it
        free(line);
    }

    fclose(fp);
    return index;
}

/***
 * Description: Deletes the index and frees the memory allocated for its content.
 * @param index: Pointer to an index.
 * @returns void
*/
void index_delete(index_t *index) {
    hashtable_delete(index, counters_delete_helper);
}

/***
 * Description: Helper function to delete counters from index.
 * @param item: Pointer to an item (counters set) to be deleted 
 * @return void
*/
static void counters_delete_helper(void *item) {
    counters_t *ctrs = item;
    counters_delete(ctrs);
}

/***
 * Description: Helper function to save a counter (pair (docID, count)) into a file.
 * @param fp: Pointer to the file to be written to
 * @param docID: The id of the document in which this word appeared
 * @param count: the number of occurences of that word in that document
 * @return void
*/
static void counters_save_item(void *fp, const int docID, const int count) {
    fprintf((FILE*)fp, " %d %d", docID, count);
}

/***
 * Description: Helper function to save a word and its counters set to a file.
 * @param fp: Pointer to the file to be written to
 * @param word: String of the word to be saved
 * @param item: A pointer to the counters set of that word.
 * @returns void
*/
static void index_save_item(void *fp, const char *word, void *item) {
    fprintf((FILE*)fp, "%s", word);
    counters_t *ctrs = item;
    // Iterating over each counter to save its content with counters_save_item
    counters_iterate(ctrs, fp, counters_save_item);
    fprintf((FILE*)fp, "\n");
}