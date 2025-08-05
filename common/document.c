/**
 * document.c    Ahmed Al Sunbati    May 21st, 2025
 *
 * Description: Implements the document structure and associated functions
 *              to create, manage, and delete documents used in the querier.
 *              Each document stores an ID and a score, and a function to
 *              retrieve its URL from a crawler page directory.
 *
 * Usage: Used by querier and related modules to represent and handle documents.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "document.h"
#include "file.h"
#include "mem.h"


typedef struct document{
    char* docID;
    int docScore;
} document_t;


document_t* document_new(const char* docID, int docScore){
    if (!docID|| docScore < 0){
        return NULL;
    }
    document_t* document = mem_assert(mem_malloc(sizeof(document_t)), "Error: Failed to allocate memory for document");
    char* documentID = mem_assert(mem_malloc((strlen(docID) + 1) * sizeof(char)), "Error: failed to allocate memory for document ID\n");
    strcpy(documentID, docID);
    document->docID = documentID;
    document->docScore = docScore;
    return document;
}

char* document_getID(document_t* doc){
    if (!doc) return NULL;
    return doc->docID;
}

void document_setScore(document_t* doc, int score){
    if (doc && score > 0){
        doc->docScore = score;
    }
}


int document_getScore(document_t* doc){
    if (!doc) return -1;
    return doc->docScore;
}

char* document_getURL(document_t* doc, char* pageDir){
    char* path = mem_assert(mem_malloc((strlen(pageDir) + 3) * sizeof(char)), "Error: Failed to allocate memory for path.\n");
    sprintf(path, "%s/%s", pageDir, document_getID(doc));
    FILE* fp = fopen(path, "r");
    char* URL = file_readLine(fp);
    fclose(fp);
    mem_free(path);
    return URL;
}


void document_delete(document_t* doc){
    if (doc) {
        mem_free(doc->docID);
        mem_free(doc);
    }
}