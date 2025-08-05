/**
 * word.h    Ahmed Al Sunbati    May 21st, 2025
 * 
 * Provides functions for processing and normalizing words and lines of text.
 * Includes utilities to normalize individual words, normalize entire input lines,
 * split lines into words, and free memory allocated for word lists.
 */
#ifndef __WORD_H
#define __WORD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

char* normalizeWord(const char* word);
char* normalizeInput(char* line);
char** deconstructLine(char* line);
void freeDeconstructedLine(char** listOfWords);

#endif