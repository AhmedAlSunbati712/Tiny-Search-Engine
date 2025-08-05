/**
 * indextest.c    Ahmed  Al Sunbati    May 13, 2025
 *
 * Description: Reads index from an old file, saves it into an index struct
 *              and then saves this index in a new file.
 *
 * Usage: ./indextest oldIndexFilename newIndexFilename
 */

#include <stdio.h>
#include <stdlib.h>
#include "indexer.h"
#include "index.h"
#include "file.h"

static void parseArgs2(const int argc, const char* argv[], 
                    const char** oldIndexFilename, const char** newIndexFilename)
{
    if (argc < 3){
        fprintf(stderr, "Error: Not enough arguments supplied.");
        exit(1);
    }
    *oldIndexFilename = argv[1];
    *newIndexFilename = argv[2];
}
int main(const int argc, const char* argv[]){
    const char* oldIndexFilename;
    const char* newIndexFilename;

    parseArgs2(argc, argv, &oldIndexFilename, &newIndexFilename);
    index_t* index;
    index = index_load(oldIndexFilename);
    index_save(index, newIndexFilename);
    index_delete(index);
    return 0;
}