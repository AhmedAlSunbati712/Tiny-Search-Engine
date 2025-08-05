#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "webpage.h"
#include "mem.h"


/**
 * Description: Initiates a directory with the given name pageDirectory and creates a file
 *              called ./crawler inside of it.
 * @param pageDirectory: Name of the directory in which webpages are going to be saved.
 * @return true: If the directory was initiated successfully and the file was created.
 * @return false: Otherwise.
*/
bool
pagedir_init(const char* pageDirectory){
    // Allocate memory dynamically for the path string
    char* path = mem_assert(mem_malloc(strlen(pageDirectory) + strlen("/.crawler") + 1), "Error: Couldn't allocate memory for path");
    // Initializing the path to be pageDirectory/.crawler
    sprintf(path, "%s/.crawler", pageDirectory);
    // Opening a file in that path (.crawler) to write to
    FILE* fp = fopen(path, "w");
    if (fp == NULL) {
        mem_free(path);
        return false;
    }
    fclose(fp);
    mem_free(path);
    return true;

}

/**
 * Description: Saving the content of a page inside the directory pageDirectory with name docID
 * @param page: Pointer to the webpage to be saved.
 * @param pageDirectory: Pointer to the directory name where pages will be saved.
 * @param docID: The id of the document (page).
*/
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID){
    // String buffer to convert the integer to a string for use in path
    char strdocID[20];
    sprintf(strdocID, "%d", docID);
    // Allocate memory dynamically for the path string
    char* path = mem_assert(mem_malloc(strlen(pageDirectory) + strlen(strdocID) + 2), "Error: Couldn't allocate memory for path");
    sprintf(path, "%s/%s", pageDirectory, strdocID);
    // Opening a file in that path to write to
    FILE* fp = fopen(path, "w");
    if (fp == NULL){
        mem_free(path);
        exit(1);
    }
    // Printing information into that document
    char* webpageURL = webpage_getURL(page);
    int pageDepth = webpage_getDepth(page);
    char* pageContent = webpage_getHTML(page);
    fprintf(fp, "%s", webpageURL);
    fprintf(fp, "\n%d", pageDepth);
    fprintf(fp, "\n%s", pageContent);
    fclose(fp);
    mem_free(path);

}