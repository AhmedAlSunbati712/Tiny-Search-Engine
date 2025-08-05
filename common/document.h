/**
 * document.h    Ahmed Al Sunbati    May 21st, 2025
 *
 * Description: Declares the document_t struct and associated functions
 *              for creating, accessing, modifying, and deleting document
 *              objects used to represent documents in the querier system.
 */

#ifndef __DOCUMENT_H
#define __DOCUMENT_H

typedef struct document document_t;

document_t* document_new(const char* docID, int docScore);
int document_getScore(document_t* doc);
void document_setScore(document_t* doc, int score);
char* document_getID(document_t* doc);
char* document_getURL(document_t* doc, char* pageDir);
void document_delete(document_t* doc);

#endif