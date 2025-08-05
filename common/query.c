/**
 * query.c    Ahmed Al Sunbati    May 21st, 2025
 * 
 * Description: Implements query result handling for the querier system.
 *              Uses bags of documents (query_t) to represent query results.
 *              Provides functions to create, add to, search, intersect, union,
 *              and delete query result sets. Includes helper functions for
 *              managing document insertion, iteration, and intersection logic.
 * 
 * Key Functions:
 *   query_new: Creates a new empty query result set.
 *   query_add_document: Adds a document to the query result set.
 *   query_search_index: Searches an index for a word and adds matching documents.
 *   query_intersect: Returns documents common to two query results.
 *   query_union: Returns all unique documents from two query results.
 *   query_delete: Deletes a query result and its documents.
 * 
 * Internal Helpers:
 *   query_search_helper: Helper to add documents from index counters.
 *   query_intersect_helper: Helper for computing intersection of query results.
 *   query_seen_docs_union_helper: Helper for building union of documents.
 *   document_delete_helper: Deletes documents in a query bag.
 *   query_size_helper: Counts documents in a query result.
 */
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

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct seenDocsIntersect {
    hashtable_t* seenDocs;
    hashtable_t* commonDocs;
} seenDocsIntersect_t;

typedef bag_t query_t;

static void query_search_helper(void* arg, const int docID, const int count);
static void seen_docs_to_query_helper(void* queriesUnion, const char* docID, void* docScore);
static void query_seen_docs_union_helper(void* seenDocs, void* doc);
static void query_seen_docs(void* seenDocs, void* doc);
static void document_delete_helper(void* item);
static void query_size_helper(void* size, void* doc);
static void query_iterate_copy_item(void* qresults, void* doc);
static seenDocsIntersect_t* query_seenDocs_struct_new(hashtable_t* seenDocs, hashtable_t* commonDocs);
static void query_intersect_helper(void* seenDocsIntersect, void* doc);

/***
 * Description: Creates a new empty query result set (bag of documents).
 * @returns pointer to new query_t bag, or NULL if allocation fails.
 */
query_t* query_new(){
    return bag_new();
}

/***
 * Description: Adds a document to the query result set.
 * Checks if either qresults or doc is NULL.
 * @param qresults: the query bag to insert into.
 * @param doc: the document to insert.
 * @returns void
 */
void query_add_document(query_t* qresults, document_t* doc){
    if (qresults && doc) bag_insert(qresults, doc);
}

/***
 * Description: Iterates through all documents in the query result set,
 * applying itemfunc to each document.
 * @param qresults: the bag to iterate.
 * @param arg: argument passed to itemfunc.
 * @param itemfunc: function applied to each document.
 */
void query_iterate(query_t* qresults, void* arg, void (*itemfunc)(void* arg, void* doc)){
    bag_iterate(qresults, arg, itemfunc);
}

/**
 * Description: Computes the number of documents in a query result. 
 *              This function iterates over the query result bag and
 *              counts how many document entries it contains.
 * 
 * @param qresults: A pointer to the query result bag to evaluate.
 * @return The number of documents contained in the query result.
 */
int query_size(query_t* qresults){
    int size = 0;
    query_iterate(qresults, &size, query_size_helper);
    return size;
}

/**
 * Description: Extracts a document from a query results.
 * @param qresults: A pointer to the query result bag to extract from.
 * @returns a pointer to a document in the query result bag.
*/
document_t* query_extract(query_t* qresults){
    return bag_extract(qresults);
}


/***
 * Description: Searches the index for a word and adds matching documents to the query result.
 * @param qresults: query bag to insert results into.
 * @param index: index to search.
 * @param word: word to search for.
 */
void query_search_index(query_t* qresults, index_t* index, char* word){
    if (qresults && index && word){
        counters_t* ctrs = index_find(index, word);
        if (ctrs) counters_iterate(ctrs, qresults, query_search_helper);
    }
}

/***
 * Description: Returns a new query result set containing only documents present in both inputs.
 * Caller is responsible for freeing qresults1 & qresults2 later.
 * @param qresults1: first query result.
 * @param qresults2: second query result.
 * @returns new query result containing intersection or a copy of one of the queries if either of them is Null
 */
query_t* query_intersect(query_t* qresults1, query_t* qresults2)
{   
    if (query_size(qresults1) > 0 && query_size(qresults2) > 0){
        int numSlots = 1500;    
        hashtable_t* seenDocs = hashtable_new(numSlots);
        hashtable_t* commonDocs = hashtable_new(numSlots);
        seenDocsIntersect_t* seenDocsIntersect = query_seenDocs_struct_new(seenDocs, commonDocs);
        query_iterate(qresults1, seenDocs, query_seen_docs);
        query_iterate(qresults2, seenDocsIntersect, query_intersect_helper);
    
        query_t* queryIntersect = query_new();
        hashtable_iterate(commonDocs, queryIntersect, seen_docs_to_query_helper);
        hashtable_delete(seenDocs, mem_free);
        hashtable_delete(commonDocs, mem_free);
        mem_free(seenDocsIntersect);

        return queryIntersect;
    } else if (query_size(qresults1) > 0) {
        query_t* queryIntersect = query_new();
        query_iterate(qresults1, queryIntersect, query_iterate_copy_item);
        return queryIntersect;
    } else if (query_size(qresults2) > 0) {
        query_t* queryIntersect = query_new();
        query_iterate(qresults2, queryIntersect, query_iterate_copy_item);
        return queryIntersect;
    } else {
        return NULL;
    }
}

/***
 * Description: Returns a new query result set containing all unique documents from both inputs.
 * @param qresults1: first query result.
 * @param qresults2: second query result.
 * @returns new query result containing union.
 */
query_t* query_union(query_t* qresults1, query_t* qresults2){
    if (qresults1 && qresults2){
        int numSlots = 1500;
        hashtable_t* seenDocs = hashtable_new(numSlots);
        query_iterate(qresults1, seenDocs, query_seen_docs_union_helper);
        query_iterate(qresults2, seenDocs, query_seen_docs_union_helper);

        query_t* queryUnion = query_new();
        hashtable_iterate(seenDocs, queryUnion, seen_docs_to_query_helper);
        hashtable_delete(seenDocs, mem_free);
        return queryUnion;
    } else if (qresults1) {
        query_t* queryUnion = query_new();
        query_iterate(qresults1, queryUnion, query_iterate_copy_item);
        return queryUnion;
    } else if (qresults2) {
        query_t* queryUnion = query_new();
        query_iterate(qresults2, queryUnion, query_iterate_copy_item);
        return queryUnion;
    } else {
        return NULL;
    }
}

/***
 * Description: Deletes a query result bag and all its documents.
 * @param qresults: query result to delete.
 * @returns void
 */
void query_delete(query_t* qresults){
    bag_delete(qresults, document_delete_helper);
}

/***
 * Description: Helper function to delete a document inside a query bag.
 * @param item: pointer to document_t to delete.
 */
static void document_delete_helper(void* item){
    document_t* doc = item;
    document_delete(doc);
}

/***
 * Description: Helper function used in counters_iterate to create a document from a docID and count,
 *              then add it to the query result set.
 * @param arg: pointer to the query result set (query_t*).
 * @param docID: integer document ID.
 * @param count: integer count associated with the document (e.g., word count).
 */
static void query_search_helper(void* arg, const int docID, const int count){
    char* tempBuffer = mem_malloc(100 * sizeof(char));
    sprintf(tempBuffer, "%d", docID);
    document_t* doc = document_new(tempBuffer, count);
    if (doc){
        query_add_document((query_t*)arg, doc);
    }
    mem_free(tempBuffer);
}


/***
 * Description: Helper function used during iteration over the hashtable of seen documents.
 *              Converts each document ID and score pair into a document and adds it to a query result set.
 * @param arg: pointer to the query result set (query_t*) to add documents to.
 * @param docID: the document ID string key.
 * @param docScore: pointer to the integer score associated with the document.
 */
static void seen_docs_to_query_helper(void* arg, const char* docID, void* docScore){
    if (arg){
        query_t* queriesCombined = (query_t*)arg;
        document_t* doc = document_new(docID, *(int*)docScore);
        query_add_document(queriesCombined, doc);
    }
}


/***
 * Description: Helper function used during iteration over a query result set to update the union hashtable.
 *              For each document, adds its score to the existing score if present, or inserts a new entry.
 * @param seenDocs: pointer to the hashtable_t storing document scores.
 * @param doc: pointer to the document_t being processed.
 */
static void query_seen_docs_union_helper(void* seenDocs, void* doc){
    if (seenDocs){
        char* docID = document_getID((document_t*) doc);
        int docScore = document_getScore((document_t* ) doc);
        int* currentScore = (int*)hashtable_find((hashtable_t*)seenDocs, docID);
        if (currentScore){
            *currentScore += docScore;
        } else {
        int* newScore = mem_assert(mem_malloc(sizeof(int)), "Error: Failed score allocation\n");
        *newScore = docScore;
        hashtable_insert(seenDocs, docID, newScore);
        }
    }
}


/***
 * Description: Helper function used during iteration over a query result set to update the intersect hashtable.
 *              For each document, updates the stored score to be the minimum of existing and current score,
 *              or inserts a new entry if not present.
 * @param seenDocs: pointer to the hashtable_t storing document scores.
 * @param doc: pointer to the document_t being processed.
 */
static void query_seen_docs(void* seenDocs, void* doc){
    if (seenDocs){
        char* docID = document_getID((document_t*) doc);
        int* docScore = mem_assert(mem_malloc(sizeof(int)), "Error: malloc failed");
        *(docScore) = document_getScore((document_t* ) doc);
        hashtable_insert(seenDocs, docID, docScore);
        }
}


/***
 * Description: Helper function used during iteration to help copy one query results into another
 * @param qresults: The query results to be copied to.
 * @param doc: The document to be copied.
 * @returns void
*/
static void query_iterate_copy_item(void* qresults, void* doc){
    document_t* newDoc = document_new(document_getID((document_t*)doc), document_getScore((document_t*) doc));
    query_add_document((query_t*)qresults, (document_t*)newDoc);
}

/**
 * Description: Helper function for query_size. Increments the integer counter passed as the 
 *              `arg` by 1 for each document found during query iteration.
 *
 * @param size A pointer to an integer counter.
 * @param doc The current document in the bag.
 */
static void query_size_helper(void* size, void* doc){
    (*((int*)size))++;
}

static seenDocsIntersect_t* query_seenDocs_struct_new(hashtable_t* seenDocs, hashtable_t* commonDocs){
    if (!seenDocs || !commonDocs) return NULL;
    seenDocsIntersect_t* seenDocsIntersect = mem_assert(mem_malloc(sizeof(seenDocsIntersect_t)),
                                                        "Error: Failed to allocate memory for seenDocsIntersect.\n");
    seenDocsIntersect->seenDocs = seenDocs;

    seenDocsIntersect->commonDocs = commonDocs;
    return seenDocsIntersect;
}


/**
 * Description: Helper function used during intersection of query results.
 *              It checks if a document is present in the 'seenDocs' hashtable, and if so,
 *              inserts it into the 'commonDocs' hashtable with the minimum score between
 *              the two occurrences.
 * @param seenDocsIntersect: A pointer to a `seenDocsIntersect_t` struct that contains two hashtables:
 *                          `seenDocs`: documents already seen in a previous query term.
 *                          `commonDocs`: intersection of seen documents across all query terms.
 * @param doc: A pointer to a `document_t` representing a document found in the current query term.
 */
static void query_intersect_helper(void* seenDocsIntersect, void* doc){
    if (seenDocsIntersect){
        char* docID = document_getID((document_t*) doc);
        int* docScore = mem_assert(mem_malloc(sizeof(int)), "Error: malloc failed");
        *(docScore) = document_getScore((document_t* ) doc);
        hashtable_t* seenDocs = ((seenDocsIntersect_t*)seenDocsIntersect)->seenDocs;
        hashtable_t* commonDocs = ((seenDocsIntersect_t*)seenDocsIntersect)->commonDocs;
        int* seenScore = hashtable_find(seenDocs, docID);
        if (seenScore != NULL){
            int* newScore = mem_assert(mem_malloc(sizeof(int)), "Error: Failed to allocate memory for newScore.\n");
            *(newScore) = MIN(*(seenScore), *(docScore));
            hashtable_insert(commonDocs, docID, newScore);
        }
        mem_free(docScore);
    }
}