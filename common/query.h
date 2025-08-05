/**
 * query.h     Ahmed Al Sunbati    May 21st, 2025
 * 
 * Interface for managing query results in the form of bags of documents.
 * Provides functions to create, manipulate, search, combine (union/intersect),
 * iterate over, and delete query result sets.
 * 
 */
#ifndef QUERY_H
#define QUERY_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "bag.h"
#include "index.h"
#include "document.h"
#include "counters.h"
#include "hashtable.h"
#include "webpage.h"
#include "mem.h"
#include "word.h"


typedef bag_t query_t;

/***
 * Description: Creates a new empty query result set (bag of documents).
 * @returns pointer to new query_t bag, or NULL if allocation fails.
 */
query_t* query_new(void);

/***
 * Description: Adds a document to the query result set.
 * Checks if either qresults or doc is NULL.
 * @param qresults: the query bag to insert into.
 * @param doc: the document to insert.
 * @returns void
 */
void query_add_document(query_t* qresults, document_t* doc);

/***
 * Description: Iterates through all documents in the query result set,
 * applying itemfunc to each document.
 * @param qresults: the bag to iterate.
 * @param arg: argument passed to itemfunc.
 * @param itemfunc: function applied to each document.
 */
void query_iterate(query_t* qresults, void* arg, void (*itemfunc)(void* arg, void* doc));

/**
 * Description: Computes the number of documents in a query result. 
 *              This function iterates over the query result bag and
 *              counts how many document entries it contains.
 * 
 * @param qresults The query result bag (query_t*) to evaluate.
 * @return The number of documents contained in the query result.
 */
int query_size(query_t* qresults);

document_t* query_extract(query_t* qresults);

/***
 * Description: Searches the index for a word and adds matching documents to the query result.
 * @param qresults: query bag to insert results into.
 * @param index: index to search.
 * @param word: word to search for.
 */
void query_search_index(query_t* qresults, index_t* index, char* word);

/***
 * Description: Returns a new query result set containing only documents present in both inputs.
 * Caller is responsible for freeing qresults1 & qresults2 later.
 * @param qresults1: first query result.
 * @param qresults2: second query result.
 * @returns new query result containing intersection.
 */
query_t* query_intersect(query_t* qresults1, query_t* qresults2);

/***
 * Description: Returns a new query result set containing all unique documents from both inputs.
 * @param qresults1: first query result.
 * @param qresults2: second query result.
 * @returns new query result containing union.
 */
query_t* query_union(query_t* qresults1, query_t* qresults2);

/***
 * Description: Deletes a query result bag and all its documents.
 * @param qresults: query result to delete.
 * @returns void
 */
void query_delete(query_t* qresults);

#endif // QUERY_H
