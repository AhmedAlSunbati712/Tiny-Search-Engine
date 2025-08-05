#ifndef __INDEX_H
#define __INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "counters.h"
#include "mem.h"
#include "file.h"
#include "word.h"

typedef hashtable_t index_t;

// Creates a new index with # slots given for a specific number of words to index
index_t *index_new(const int num_slots);

// Insert a word-docID-count triple into the index
bool index_insert(index_t *index, const char *word, const int docID, const int count);

// Save the index to a file
bool index_save(index_t *index, const char *filename);

// Load an index from a file
index_t *index_load(const char *filename);

// Finding the counters set associated with a given word. Returns Null if it doesnt exist.
counters_t *index_find(index_t* index, const char* word);
// Delete the index and free all memory
void index_delete(index_t *index);

#endif
