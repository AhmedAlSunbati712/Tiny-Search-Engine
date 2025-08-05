/**
 * querier.h    Ahmed Al Sunbati    May 21st, 2025
 *
 * Description: Header file for the querier module. Declares functions used for
 *              parsing command line arguments, processing normalized queries,
 *              validating query syntax, and printing query results.
 *
 * Usage: Included by querier.c.
 */
#ifndef __QUERIER_H
#define __QUERIER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "querier.h"
#include "query.h"
#include "index.h"
#include "bag.h"
#include "word.h"
#include "mem.h"
#include "file.h"

void parseArgs(const int argc, const char* argv[], char** pageDirectory, char** indexFilename);
query_t* querierProcess(char* normalizedQuery, index_t* index, char* pageDir);
bool isInputValid(char* line);
void printDocuments(FILE* fp, query_t* qresults, char* pageDir);

#endif