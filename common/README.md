# Implementation
## pagedir
`pagedir.c` implements two functions that facilitate the creation of the `./crawler` file and the saving of the webpages
contents. We implement the following two functions:
```java
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
```
`pagedir_init` Takes a pointer to the name of the directory (that already exists) and creates the file ./crawler
`pagedir_save` Takes a pointer to a page, the name of the directory to save the page to, and the document ID to save
a file with the first line being URL of the page, the second being its depth, and whatever comes after is the html content
## word
Provides functions for processing and normalizing words and lines of text. Includes utilities to normalize individual words, normalize entire input lines, split lines into words, and free memory allocated for word lists.
It has the following prototype:
```c
char* normalizeWord(const char* word);
char* normalizeInput(char* line);
char** deconstructLine(char* line);
void freeDeconstructedLine(char** listOfWords);
```
## query
Implements query result handling for the querier system. Uses bags of documents (query_t) to represent query results. Provides functions to create, add to, search, intersect, union, and delete query result sets. Includes helper functions for managing document insertion, iteration, and intersection logic. It has the following prototype:
```c
typedef bag_t query_t;
query_t* query_new(void);
void query_add_document(query_t* qresults, document_t* doc);
void query_iterate(query_t* qresults, void* arg, void (*itemfunc)(void* arg, void* doc));
int query_size(query_t* qresults);
document_t* query_extract(query_t* qresults);
void query_search_index(query_t* qresults, index_t* index, char* word);
query_t* query_intersect(query_t* qresults1, query_t* qresults2);
query_t* query_union(query_t* qresults1, query_t* qresults2);
void query_delete(query_t* qresults);
```
## index
Implementation of an inverted index data structure using a hashtable, where each word maps to a set of counters. Each counter tracks the number of times a word appears in a specific document. Implements the following functionality; creating a new index with a fixed number of slots; inserting word-document-count entries; looking up counters for a given word; saving an index to a file in a readable format; loading an index from a file. It has the following prototype:
```c
typedef hashtable_t index_t;
index_t *index_new(const int num_slots);
bool index_insert(index_t *index, const char *word, const int docID, const int count);
bool index_save(index_t *index, const char *filename);
index_t *index_load(const char *filename);
counters_t *index_find(index_t* index, const char* word);
void index_delete(index_t *index);
```