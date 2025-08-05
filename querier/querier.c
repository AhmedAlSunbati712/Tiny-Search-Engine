/**
 * querier.c    Ahmed Al Sunbati    May 21st, 2025
 *
 * Description: Implements a command-line tool that loads an index created by an indexer,
 *              accepts user queries, validates and normalizes them, performs search
 *              operations on the index to find matching documents, sorts results by score,
 *              and displays the ranked documents with their scores and URLs.
 *
 * Usage: ./querier pageDirectory indexFilename
 */
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


#define MAX_QUERY_LENGTH 128

void parseArgs(const int argc, const char* argv[], char** pageDirectory, char** indexFilename);
query_t* querierProcess(char* normalizedQuery, index_t* index, char* pageDir);
bool isInputValid(char* normalizedQuery);
void printDocuments(FILE* fp, query_t* qresults, char* pageDir);
static int compareDocs(const void* a, const void* b);
static document_t** extractDocumentsSorted(query_t* qresults);
static void queryExtractHelper(document_t** docsArray, query_t* qresults);
static void printDocumentsHelper(FILE* fp, document_t* doc, char* pageDir);


int main(const int argc, const char* argv[]){
    char* pageDirectory; char* indexFilename;
    // Parse CLI input into pageDirectoy and indexFilename
    parseArgs(argc, argv, &pageDirectory, &indexFilename);
    // Load the index from the given pathname for index
    index_t* index = index_load(indexFilename);
    // Prompt the user
    char line[MAX_QUERY_LENGTH];
    printf("Query: ");
    fflush(stdout);
    // While the stdin hasn't reached EOF, keep on taking input from stdin
    while (fgets(line, MAX_QUERY_LENGTH, stdin) != NULL) {
        // Strip off newline
        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        // Skip empty queries
        if (strlen(line) == 0) {
            printf("-----------------------------------------------\n");
            printf("Query: ");
            fflush(stdout);
            continue;
        }
        // Normalize the user input
        char* normalizedQuery = normalizeInput(line);
        
        // Print out the query normalized
        fprintf(stdout, "%s\n", normalizedQuery);
        fflush(stdout);

        // Check if its valid
        bool isInputValidResult = isInputValid(normalizedQuery);
        // If it's not, skip searching up this query and prompt the user again.
        if (!isInputValidResult) {
            fprintf(stderr, "Error: Invalid query syntax.\n");
            printf("-----------------------------------------------\n");
            printf("Query: ");
            fflush(stdout);
            mem_free(normalizedQuery);
            continue;
        }

        // Find documents that match the search query
        query_t* queryResult = querierProcess(normalizedQuery, index, pageDirectory);
        
        // Check if the resulting documents are empty
        int querySize = query_size(queryResult);
        if (querySize == 0){
            printf("No documents matched.\n");
        } else {
            // If not, print out the documents in descending order of their scores
            printDocuments(stdout, queryResult, pageDirectory);
            // Free the query and its contents
            query_delete(queryResult);
        }
        mem_free(normalizedQuery);
        printf("-----------------------------------------------\n");
        // Prompt again
        printf("Query: ");
        fflush(stdout);
    }
    printf("\n"); // For clean newline after EOF
    index_delete(index);  // Cleanup

    return 0;
}

/**
 * Description: Parses (and validates) arguments of of the CLI into the pointers pageDirectory & indexFilename.
 * @param argc: Number of arguments on the CLI.
 * @param argv: The arguments on the CLI.
 * @param pageDirectory: Pointer to the string that will hold the pathname for the crawler directory.
 * @param indexFilename: Poitner to the string that will hold the pathname for the index file.
*/

void parseArgs(const int argc, const char* argv[], char** pageDirectory, char** indexFilename){
    // Exit with non-zero code in case the number of arguments supplied is not enough
    if (argc != 3){
        fprintf(stderr, "Error: ./querier pageDir indexFilename\n");
        exit(1);
    }

    // Validating that argv[1] actually points to a crawler directory
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
    *pageDirectory = (char*) argv[1];

    // Validating that argv[2] actually points to a readable index file.
    FILE* fp2 = fopen(argv[2], "r");
    if (fp2 == NULL){
        fprintf(stderr, "Error: Can't find readable index file.\n");
        exit(1);
    }
    fclose(fp2);
    *indexFilename = (char*) argv[2];
}

/***
 * Description: Searches up documents that matches the normalized query using the index and the crawler pageDir
 * @param normalizedQuery: The string that represents the normalized and valid query to be searched.
 * @param index: Pointer to the index object to be searched.
 * @param pageDir: Pathname for a valid crawler Directory.
 * @returns A pointer to a query object with the matching documents.
*/
query_t* querierProcess(char* normalizedQuery, index_t* index, char* pageDir){
    // Deconstruct the normalized query to words to be searched individually
    char** listOfWords = deconstructLine(normalizedQuery);
    // The query object that will hold the final result
    query_t* queryFinalResults = query_new();
    
    // A query object that will hold the matching resutls for a current <and sequence>
    // Reinitialized once we hit an "or"
    query_t* currQueryResult = query_new();

    // Looping through each word
    int idx = 0;
    while (listOfWords[idx] != NULL){
        char* word = listOfWords[idx++];
        // Checking if the word is an operator
        bool isOperator = ((strcmp(word, "or") == 0) || (strcmp(word, "and") == 0));
        // If it's an "or", then take the union of (the last <and sequence> stored in currQueryResult) and (finalQueryResult)
        if ((strcmp(word, "or") == 0)){
            query_t* tempUnion = query_union(queryFinalResults, currQueryResult);
            query_delete(queryFinalResults);
            query_delete(currQueryResult);
            queryFinalResults = tempUnion;

            // Prep currQueryResult to take another <and sequence>
            currQueryResult = query_new();
        } else if (!isOperator){
            // If it's not an operator, then
            query_t* tempQueryResult = query_new();
            // Search up matching documents for current word
            query_search_index(tempQueryResult, index, word);
            // And intersect it with the growing <and sequence> in currQueryResult
            query_t* queryIntersect = query_intersect(tempQueryResult, currQueryResult);
            
            query_delete(tempQueryResult);
            query_delete(currQueryResult);
            currQueryResult = queryIntersect;
        }
    }

    // Do one last union operation [(<and sequence> or <and sequence> or <and sequence>) or currQueryResult]
    query_t* tempUnion = query_union(queryFinalResults, currQueryResult);
    query_delete(queryFinalResults);
    query_delete(currQueryResult);
    queryFinalResults = tempUnion;
    freeDeconstructedLine(listOfWords);

    return queryFinalResults;
}

/**
 * Description: Validates the syntax of a query.
 * @param line: The normalized query to be validated.
 * @returns true if the query is valid, false otherwise
*/
bool isInputValid(char* line){
    if (line == NULL) return false;
    
    // Empty query
    int len = strlen(line);
    if (len == 0) return false;

    // Check if the first word is "and" or "or"
    char* firstWord = mem_assert(mem_malloc(128), "Error: Failed to allocate memory for firstWord.\n");
    int j = 0;
    int i = 0;
    while (i < len && !isspace(line[i]) && j < 127) {
        firstWord[j++] = line[i++];
    }
    firstWord[j] = '\0';

    // Checking....
    if (strcmp(firstWord, "and") == 0 || strcmp(firstWord, "or") == 0) {
        mem_free(firstWord);
        return false;
    }
    mem_free(firstWord); // Freeing the memory buffer we used

    bool isLastWordOperator = false; // Used to check for consecutive operators or operator at end of line
    // Scanning for whether there's two consecutive operators
    for (i = 0; i < len; i++){
        char* wordBuffer = mem_assert(mem_malloc(128), "Error: Failed to allocate memory for wordBuffer.\n");
        int j = 0;
        // Scanning one word at a time till we hit a space
        while (i < len && !isspace(line[i])){
            wordBuffer[j++] = line[i++];
        }
        wordBuffer[j] = '\0';

        // Check if the word is an operator
        bool isOperator = ((strcmp(wordBuffer, "and") == 0) || (strcmp(wordBuffer, "or") == 0));
        // If it's an operator and the last word was also an operator
        if (isOperator && isLastWordOperator){
            mem_free(wordBuffer);
            return false; // Terminate and return false
        }
        // If it's an operator but last (previous) word is not operator
        if (isOperator && !isLastWordOperator){
            isLastWordOperator = true; // Change the condition for isLastWordOperator
        } else {
            // If it's not an operator, the next word isn't preceded by an operator
            isLastWordOperator = false;
        }
        mem_free(wordBuffer);
    }
    if (isLastWordOperator) {
        return false;
    }
    return true;
}

/**
 * Description: Comparison function for sorting documents by score in descending order.
 *              Used with `qsort` to sort an array of `document_t*` based on document scores.
 * @param a: A pointer to the first element in the array being sorted.
 * @param b: A pointer to the second element in the array being sorted.
 * @return 
 *   Returns 1 if the score of documentA is less than documentB (to sort in descending order),
 *   -1 if documentA's score is greater,
 *   and 0 if the scores are equal.
 */
static int compareDocs(const void* a, const void* b) {
    const document_t* documentA = *(const document_t**)a;
    const document_t* documentB = *(const document_t**)b;

    int scoreA = document_getScore((document_t*)documentA);
    int scoreB = document_getScore((document_t*)documentB);

    if (scoreA < scoreB) return 1;
    if (scoreA > scoreB) return -1;
    return 0;
}

/**
 * Description: Extracts documents from a query result, sorts them by score using `qsort`, 
 *              and returns the sorted array.
 * @param qresults: The query result set (query_t*) containing document matches.
 * @return  A dynamically allocated array of document_t* pointers sorted by score in descending order.
 */
static document_t** extractDocumentsSorted(query_t* qresults){
    int querySize = query_size(qresults);
    document_t** docsArray = mem_assert(mem_malloc(querySize * sizeof(document_t*)), "Error: Failed to allocate memory for documentsArray.\n");
    queryExtractHelper(docsArray, qresults);
    qsort(docsArray, querySize, sizeof(document_t*), compareDocs);
    return docsArray;
}

/**
 * Description: Helper function that extracts all documents from a query result and populates 
 *              the given array of document_t*.
 * @param docsArray: A preallocated array of document_t* to store extracted documents.
 * @param qresults: The query result set to extract documents from.
 */
static void queryExtractHelper(document_t** docsArray, query_t* qresults){
    int idx = 0;
    document_t* doc;
    while ((doc = query_extract(qresults)) != NULL){
        docsArray[idx++] = doc;
    }
}

/**
 * Description: Prints the sorted list of documents from a query result to the provided file stream.
 *              Each document's score, ID, and URL are printed. Also handles cleanup of allocated memory.
 * @param fp: The output file stream to print the documents to.
 * @param qresults: The query result set to be printed.
 * @param pageDir: The directory path used to locate or generate URLs for the documents.
 */
void printDocuments(FILE* fp, query_t* qresults, char* pageDir){
    int querySize = query_size(qresults);
    document_t** docsSorted = extractDocumentsSorted(qresults);

    for(int i = 0; i < querySize; i++){
        printDocumentsHelper(fp, docsSorted[i], pageDir);
    }
    mem_free(docsSorted);
}

/**
 * Description: Helper function to print a single document's score, ID, and URL to the file stream.
 *              Also frees memory associated with the document and its generated URL.
 * @param fp: The output file stream to print the document information to.
 * @param doc: The document to be printed and deleted.
 * @param pageDir: The directory used to generate the document's URL.
 */
static void printDocumentsHelper(FILE* fp, document_t* doc, char* pageDir){
    char* URL = document_getURL(doc, pageDir);
    char* docID = document_getID(doc);
    int docScore = document_getScore(doc);
    fprintf(fp, "Score: %d, ID: %s, URL:%s\n", docScore, docID, URL);
    mem_free(URL);
    document_delete(doc);
}