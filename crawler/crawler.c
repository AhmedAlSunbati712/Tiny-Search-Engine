#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "webpage.h"
#include "bag.h"
#include "hashtable.h"
#include "pagedir.h"
#include "mem.h"

int NUM_SLOTS = 200;

static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);
static void deleteDepth(void* item);


int 
main(const int argc, char* argv[]){
    // Declaring our arguments for crawling a seedURL
    char* seedURL; char* pageDirectory; int maxDepth;
    // Parsing the arguments from the command line into the variables
    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
    // Crawling until maxDepth and saving the webpages to pageDirectory
    crawl(seedURL, pageDirectory, maxDepth);
    exit(0);
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
parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth)
{
    // Ensuring user inputted enough arguments.
    if (argc < 4){
        fprintf(stderr, "Error: Not enough arguments supplemented.\n");
        exit(1);
    }
    // Ensuring the seedURL is internal.
    char* normalizedURL = normalizeURL(argv[1]);
    if (!isInternalURL(normalizedURL)){
        fprintf(stderr, "Error: The URL is not internal.\n");
        mem_free(normalizedURL);
        exit(1);
    } else {
        *seedURL = normalizedURL;
    }
    // In case of failure of initiating the page directory.
    if (!pagedir_init((*pageDirectory = argv[2]))){
        fprintf(stderr, "Error: Can't write on file.\n");
        exit(1);
    }
    // Ensuring max depth is a non-negative integer.
    if (sscanf(argv[3], "%d", maxDepth) != 1 || *maxDepth < 0) {
        fprintf(stderr, "Error: Max depth must be a non-negative integer.\n");
        exit(1);
    }

}

/**
* Description: Implements the core crawling logic. Fetches pages up to a given depth, scans and saves them.
* @param seedURL: seedURL for the crawl.
* @param pageDirectory: Directory where pages are saved.
* @param maxDepth: Maximum depth allowed.
* @return void
*/
static void 
crawl(char* seedURL, char* pageDirectory, const int maxDepth){
    // Initializing a hashtable to hold (URLs, URLDepth) seen
    hashtable_t* pagesSeen = hashtable_new(NUM_SLOTS);

    // Allocating memory for the depth of the seedURL
    // Adding the seedURL and its depth to the pages seen hashtable
    int* seedDepth = (int*)mem_malloc(sizeof(int));
    *seedDepth = 0;
    hashtable_insert(pagesSeen, seedURL, seedDepth);

    // Initializing a bag to hold pages that need to be crawled
    bag_t* pagesToCrawl = bag_new();
    // Initializin a webpage with out seedURL and adding it to the bag
    webpage_t* seedURLWebpage = webpage_new(seedURL, *(int*)hashtable_find(pagesSeen, seedURL), NULL);
    bag_insert(pagesToCrawl, seedURLWebpage);
    // Declaring a webpage variable to hold the webpage to be extracted from the bag
    webpage_t* webpage;
    int docID = 0;
    // While there are still pages to crawl...
    while ((webpage = bag_extract(pagesToCrawl)) != NULL) {
        // Extract a page and try to fetch it contents
        if(webpage_fetch(webpage)){
            fprintf(stdout, "Fetched: %s\n", webpage_getURL(webpage));
            // Save the page with the corresponding document ID and increment 
            pagedir_save(webpage, pageDirectory, docID);
            docID++;
            // Check if we are the maximum depth and don't go any further searching for links.
            if (*(int*)hashtable_find(pagesSeen, webpage_getURL(webpage)) < maxDepth){
                pageScan(webpage, pagesToCrawl, pagesSeen);
            }
        }
        // After finshing scanning & saving the page, delete it since we don't need it anymore
        webpage_delete(webpage);
    }
    // Delete the bag and the hashtable
    bag_delete(pagesToCrawl, NULL);
    hashtable_delete(pagesSeen, deleteDepth);
}

/**
* Description: Scans a webpage for internal links, normalizes and adds unseen URLs to crawl queue.
* @param page: The current page to be scanned.
* @param pagesToCrawl: Bag of pages to be crawled.
* @param pagesSeen: Hashtable of URLs already seen with their depths.
* @return void
*/
static void 
pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen){
    fprintf(stdout, "Scanning: %s\n", webpage_getURL(page));
    char* URL; // String to hold the URL for the next link on the page.
    int pos = 0;
    // Check if there's another link to be grabbed in the current page.
    while ((URL = webpage_getNextURL(page, &pos)) != NULL){
        // Normalize the URL for the URL grabbed.
        char* normalizedURL = normalizeURL(URL);
        // Check if its internal and if it hasn't been seen before
        if (isInternalURL(normalizedURL) && hashtable_find(pagesSeen, normalizedURL) == NULL){
            // Allocate memory for its depth
            int* currentDepth = (int*)mem_malloc(sizeof(int)); 
            *currentDepth = *(int*)hashtable_find(pagesSeen, webpage_getURL(page)) + 1; // It's one level deeper than its parent
            hashtable_insert(pagesSeen, normalizedURL, currentDepth); // Add it to pages seen
            // Initialize a new webpage with the URL and add it to our collection of pages to be crawled
            webpage_t* webpage = webpage_new(normalizedURL, *currentDepth, NULL);
            bag_insert(pagesToCrawl, webpage);
            fprintf(stdout, "Found: %s\n", normalizedURL);
        } else {
            mem_free(normalizedURL);
        }
        mem_free(URL);
    }
    
}

/**
* Description: Frees the dynamically allocated memory for an integer (used for depth values in hashtable).
* @param item: A void pointer to the dynamically allocated memory to free.
* @return void
*/
static void 
deleteDepth(void* item){
    mem_free(item);
}