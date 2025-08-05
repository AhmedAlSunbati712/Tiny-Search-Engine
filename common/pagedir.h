/************************ __PAGEDIR_H***********************/
/* Interace for functions used to initiate directory to save
 * webpages scanned and fetched.*/
#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

#endif