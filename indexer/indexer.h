#ifndef __INDEXER_H
#define __INDEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"
#include "mem.h"
#include "file.h"
#include "webpage.h"
#include "hashtable.h"
#include "word.h"

/**Builds an inverted index from documents it finds in pageDirectory*/
index_t* indexBuild(const char* pageDirectory);

/**Scans a webpage to find words and indexes them*/
void indexPage(webpage_t* webpage, index_t* index);

/**Parses inputs into variables*/
void parseArgs(const int argc, const char* argv[], const char** pageDirectory, const char** indexFileName);




#endif