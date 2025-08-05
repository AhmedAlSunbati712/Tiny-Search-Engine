# Design Specs
## User Interface
The user interacts with the querier through the CLI using the following format:
```
./querier pageDirectory indexFilename
```
Where `pageDirectory` is a pathname for a valid crawler directory, and `indexFilename` is a pathname for a correspoding index.
The querier loads the index file and is ready for search queries from the user. The user is prompted with a query
```
Query ?
```
And the user types down their search query

## Inputs & Outputs
- **Inputs:** The user is responsible for inputting a valid pageDirectory and indexFilename. After the query is prompted, the user inputs their search query to the standard input.
- **Outputs:** After the user input their search query, the query is printed out back to the standard output _(normalized)_. If no documents were found, the querier outputs:
```
Query ? a query WITh nO match
a query with no match
No documents were found
Query ?
```
It's ready to take another query search. However, if matching documents were found, it outputs them in the following format in descending order of their score:
```
Score1: docScore1 ID: docID1 URL: docURL1
Score2: docScore2 ID: docID2 URL: docURL2
Score3: docScore3 ID: docID3 URL: docURL3
Score4: docScore4 ID: docID4 URL: docURL4
```

## Functional decomposition into modules
We anticipate the following modules or functions:
1. `main`, which parses arguments and initializes other modules.
2. `parsArgs`, which parses input for CLI and validates it.
3. `isInputValid`, which validates the syntax of a query search
4. `querierProcess`, which takes a normalized query, an index, and valid crawler directory to find matching documents
5. `printDocuments`, which takes the resulting matching documents from `querierProcess`and outputs them in descending order of their score.

And some helper modules that provide data structures:
- query, a module providing the data structure to hold the matching documents for a query. Has useful functions that take union and intersection of two queries.
- document, a module providing the data structure to hold a document id and its score for a given query search.
- index, a module providing the data structure to represent the in-memory index, and functions to read and write index files.
- word, a module providing a function to normalize a word, normalizeQuery, and deconstruct a query down into an array of words to be processed individually.

## Pseudo code for logic/algorithmic flow
The querier will run as following:
```
parse CLI arguments into pageDirectory and indexFilename
load index using the pathname indexFilename
prompt user with Query
while the program hasn't reach EOF:
    normalize query
    validate query
    if query is valid:
        find matching documentes using querierProcess
        print out matching documents (if found) in descending order of their score
    prompt user with Query
free all dynamically allocated memory
```
`querierProcess` works in the following way:
```
deconstuct the query into a list of words
intialize a (finalQueryResult) query_t* object to hold the matching documents for the query
intialize a (currQueryResult) query_t* object to hold the matching documents for the current <and sequence being processed>
loop through the query words:
    if word is "or":
        take the union of the current <and sequence> with the final query result
        intialize a new <and sequence> to processs
    if word is not an operator:
        find matching documents for the word
        intersect it with the current <and sequence> that is building up
take union of the last <and sequence> processed and the finalQueryResult
return finalQueryResult
```
`printDocuments` works in the following way:
```
Extract the matching documents from the finalQueryResult into an array
sort the array
Loop through each document in the array:
    print out the doucment relevant information in the desired format
```
## Major data structures
The key data structure is `query`, which is an alias of `bag`, and `document` which hold document id and score. `query` is a bag of documents that match a certain query.

## Testing plan
_Integration Testing_ The querier as a complete program will be tested using the following methods:
- Empty query.
- Invalid queries (consecutive operators, operators at the start, operators at the end..etc).
- Non-matching queries.