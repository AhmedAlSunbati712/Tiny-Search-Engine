/**
 * word.c    Ahmed  Al Sunbati    May 21, 2025
 *
 * Description: Module to used to normalize words (lowercase), normalize input (remove 
 *              extra space, and convert all characters to lowercase). It also has uti-
 *              lities to split a line of words into an array, and also a functionality
 *              to free that split line after finishing using it.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "word.h"
#include "mem.h"
/***
 * Description: Creates a lowercase copy of the input word. Allocates memory for the new word,
 *              character-by-character copies and converts the input word to lowercase.
 * @param word: A constant string representing the word to be normalized.
 * @return normalizedWord: A new string that is a lowercase copy of the input word,
 *                         or NULL if the input is NULL.
 */
char* normalizeWord(const char* word){
    if (word == NULL){
        return NULL;
    }
    char *normalizedWord = mem_assert(mem_malloc((strlen(word) + 1) * sizeof(char)), "Error: Couldn't allocate memory for normalized word.");

    for (int i = 0; word[i] != '\0'; i++) {
        normalizedWord[i] = tolower(word[i]);
    }
    normalizedWord[strlen(word)] = '\0';
    return normalizedWord;
}

/***
 * Description: Creates a lowercase copy of the input string while normalizing whitespace.
 *              Consecutive whitespace characters are replaced by a single space,
 *              and leading/trailing whitespace is removed. Allocates memory for
 *              the new normalized string. Converts all characters to lowercase.
 * @param line: A string representing the input line to be normalized.
 * @return result: A newly allocated string with normalized whitespace and lowercase characters,
 *                 or NULL if the input is NULL or memory allocation fails.
 */
char* normalizeInput(char* line) {
    if (line == NULL) {
        return NULL;
    }

    size_t len = strlen(line);
    char* result = mem_assert(mem_malloc(len + 1), "Error: Failed to allocate memory.");

    int i = 0, j = 0;
    int inWord = 0;

    while (line[i]) {
        if (isspace(line[i])) {
            if (inWord) {
                result[j++] = ' ';
                inWord = 0;
            }
        } else {
            result[j++] = tolower(line[i]);
            inWord = 1;
        }
        i++;
    }

    // Remove trailing space if any
    if (j > 0 && result[j - 1] == ' ') {
        j--;
    }

    result[j] = '\0';
    return result;
}

/**
 * Description: Splits a line of text into individual words, allocating memory for each word 
 *              and returning them as a NULL-terminated array of strings. Words are separated 
 *              by whitespace characters.
 * @param line: The input string to be split into words. Must be a non-NULL, null-terminated string.
 * @return A NULL-terminated array of strings (char**) where each element is a dynamically allocated 
 *   word from the input line. Returns NULL if the input line is NULL.
 */
char** deconstructLine(char* line){
    if (!line) return NULL;
    int len = strlen(line);
    char** listOfWords = mem_assert(mem_malloc(128 * sizeof(char*)), "Error: Failed to allocate memory for listOfWords.\n");
    int idx = 0;
    for (int i = 0; i < len; i++){
        listOfWords[idx] = mem_assert(mem_malloc(46), "Error: Failed to allocate memory for wordBuffer.\n");
        int j = 0;
        // Scanning one word at a time till we hit a space
        while (i < len && !isspace(line[i])){
            listOfWords[idx][j++] = line[i++];
        }
        listOfWords[idx++][j] = '\0';
    }
    listOfWords[idx] = NULL;
    return listOfWords;
}

/**
 * Description: Frees the memory allocated by `deconstructLine` for the list of words.
 * @param listOfWords: A NULL-terminated array of dynamically allocated strings to be freed.
 * @return void.
 */
void freeDeconstructedLine(char** listOfWords){
    if (!listOfWords) return;
    int idx = 0;
    while (listOfWords[idx] != NULL){
        mem_free(listOfWords[idx++]);
    }
    mem_free(listOfWords);
}