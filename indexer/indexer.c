/**
 * indexer.c    Ahmed  Al Sunbati    May 13th, 2025
 *
 * Description: Reads page documents from a pagedirectory produced by a crawler
 *              and indexes the words into an index struct and saves it to a file
 *              under the name filename.
 *
 * Usage: ./indexer pageDirectory indexFilename
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"
#include "mem.h"
#include "file.h"
#include "webpage.h"
#include "hashtable.h"
#include "word.h"

#define TYPICAL_INDEX_SIZE 500

// Defined internal to the module only. Use it to pass as an arg to hashtable_iterate
typedef struct indexDocumentPair {
    index_t* index;
    int docID;
} indexDocumentPair_t;

static void parseArgs(const int argc, const char* argv[], const char** pageDirectory, const char** indexFileName);
index_t* indexBuild(const char* pageDirectory);
void indexPage(webpage_t* webpage, index_t* index, int docID);
static char* formatPath(const char* pageDirectory, int docID);
static void insertWordIntoIndex(void* indexAndDocument, const char* word, void* count);



int main(const int argc, const char* argv[]){
    const char* pageDirectory;
    const char* indexFileName;
    // Parse the commandline args
    parseArgs(argc, argv, &pageDirectory, &indexFileName);
    // Build the index using the page documents from the pageDirectory directory
    index_t* index = indexBuild(pageDirectory);
    // Check if saving failed for any reason
    if(!index_save(index, indexFileName)){
        fprintf(stderr, "Failed to save.\n");
        return 1;
    } else {
        printf("Saved Index Successfully\n");
    }
    // Free the index and all of its keys
    index_delete(index);
    return 0;
}

/**
* Description: Parses and validates command-line arguments for the crawler.
* @param argc: Number of arguments.
* @param argv: Argument values.
* @param seedURL: Pointer to the seedURL to be initialized.
* @param pageDirectory: Pointer to the directory name where pages will be saved.
* @param maxDepth: Pointer to the maximum depth.
* @return void
*/
static void
parseArgs(const int argc, const char* argv[], const char** pageDirectory, const char** indexFileName){
    // Ensuring user inputted enough arguments.
    if (argc != 3){
        fprintf(stderr, "Error: Not the right number of arguments supplied.\n");
        exit(1);
    }
    char* path = mem_assert(mem_malloc(128 * sizeof(char)), "Error: Failed to allocate memory for path");
    
    sprintf(path, "%s/.crawler", argv[1]);
    FILE* fp1 = fopen(path, "r");
    if (fp1 == NULL){
        fprintf(stderr, "Error: Not a crawler directory.\n");
        mem_free(path);
        exit(1);
    } 
    fclose(fp1);
    mem_free(path);  
    *pageDirectory = argv[1];
    FILE* fp2 = fopen(argv[2], "w");
    if (fp2 == NULL){
        fprintf(stderr, "Error: Non-existing path/read-only file.\n");
        exit(1);
    }
    fclose(fp2);
    *indexFileName = argv[2];
      
    
}

/***
 * Description: Builds an index from a collection of webpages stored in the specified page directory.
 *              It reads each page file, extracts and fetches the webpage, then indexes its words.
 * 
 * @param pageDirectory: Path to the directory containing crawler-generated webpage files.
 * @return A pointer to the built index
 */
index_t* indexBuild(const char* pageDirectory){
    // Variable to hold the path for a document. It's memory is allocated in formatPath
    char* path;
    // Initializing the index struct 
    index_t* index = index_new(TYPICAL_INDEX_SIZE);
    int docID = 1;
    FILE* fp;
    // As long as we are able to find a file with name "pageDirectory/docID"
    while ((fp = fopen(path = formatPath(pageDirectory, docID), "r")) != NULL){
        // Read the URL and depth of the page during crawling (first two lines)
        char* pageURL = file_readLine(fp);
        char* depthStr = file_readLine(fp);
        int depth = atoi(depthStr); // Convert depth to integer

        // Create a page with the given URL & Depth and fetch its html content
        webpage_t* page = webpage_new(pageURL, depth, NULL);
        webpage_fetch(page);

        // Scan the page for words to insert into the index
        indexPage(page, index, docID);

        // Free all memory allocated that we no longer need
        webpage_delete(page);
        mem_free(depthStr);
        mem_free(path);
        fclose(fp); // Close the file
        docID++; // Move on to the next document
    }
    mem_free(path); // Freeing this after exiting the while loop one more time
    return index;
}

/***
 * Description: Reads words from a webpage, extract their count and inserts the pair (docID, count) into
 *              the counterset associated with the word in the index. Only words longer than 3 characters
 *              are considered.
 *
 * @param webpage: Pointer to a webpage_t containing the page content to be indexed.
 * @param index: Pointer to the index_t where words will be indexed.
 * @param docID: ID of the document associated with that webpage
 * @return void
 */
void indexPage(webpage_t* webpage, index_t* index, int docID) {
    // Create a hashtable to track words seen so far and their count
    hashtable_t* seenWords = hashtable_new(TYPICAL_INDEX_SIZE);
    
    int pos = 0;
    char* word;
    while ((word = webpage_getNextWord(webpage, &pos)) != NULL) {
        if (strlen(word) >= 3) {
            // Normalize the word
            char* normWord = normalizeWord(word);
            int* count = hashtable_find(seenWords, normWord);
            if (count == NULL) {
                // Word not seen before
                int* newCount = mem_assert(mem_malloc(sizeof(int)), "Error: Failed to allocate memory for count");
                *newCount = 1;
                hashtable_insert(seenWords, normWord, newCount);
            } else {
                // Word seen before, increment count
                (*count)++;
            }
            // Free normWord since it was copied already inside the hashtable
            mem_free(normWord);
        }
        // Free each word after usage
        mem_free(word);
    }

    // Passed to hashtable_iterate as arg
    indexDocumentPair_t* indexDoc = mem_assert(mem_malloc(sizeof(indexDocumentPair_t)), "Error: Failed to allocate memory for indexDoc");
    indexDoc->index = index;
    indexDoc->docID = docID;

    // Iterate each word through the hashtable, fetch its count & the docID from indexDoc
    // & add them as a counter to the counterset of the word in the index.
    hashtable_iterate(seenWords, indexDoc, insertWordIntoIndex);
    hashtable_delete(seenWords, mem_free); // Clean up hashtable and counts
    mem_free(indexDoc); // Clean up the indexDoc
}

/***
 * Description: Formats the file path for a specific document page ID within the given page directory.
 *              Allocates memory for the resulting path string.
 *
 * @param pageDirectory: Path to the base directory containing all document page files.
 * @param docID: ID of the page document of interest.
 * @return path: String of the full path "pageDirectory/docID"
 */
static char* formatPath(const char* pageDirectory, int docID){
    char* path = mem_assert(mem_malloc(128 * sizeof(char)), "Error: Failed to allocate memory for path.");
    sprintf(path, "%s/%d", pageDirectory, docID);
    return path;
}

/***
 * Description: Inserts a counter pair (docID, count) [docID obtained from indexAndDocument->docID]
 *              into a word's counterset found in the index indexAndDocument->index.
 * @param indexAndDocument: The struct that stores the current document's id and the index object
 * @param word: A string of the word that we want to insert its count in the document with the id
 *              indexAndDocument->docID
 * @param count: The number of occurneces of @param word in the doucment with id indexAndDocument->docID
 * 
 * @returns void
*/
static void insertWordIntoIndex(void* indexAndDocument, const char* word, void* count) {
    indexDocumentPair_t* indexDoc = indexAndDocument;
    index_insert(indexDoc->index, word, indexDoc->docID, *(int*)count);
}

