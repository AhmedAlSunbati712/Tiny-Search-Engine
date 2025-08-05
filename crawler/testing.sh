#!/bin/bash

# Regression testing on the letters webpage
ROOT_URL="http://cs50tse.cs.dartmouth.edu/tse/"
LETTERS_URL="${ROOT_URL}letters/"
TO_SCRAPE_URL="${ROOT_URL}toscrape/"
WIKIPEDIA_URL="${ROOT_URL}wikipedia/"

LETTERS_0_DIR="./Letters_0"
LETTERS_1_DIR="./Letters_1"
LETTERS_2_DIR="./Letters_2"
LETTERS_10_DIR="./Letters_10"

TO_SCRAPE_0_DIR="./toscrape_0"
TO_SCRAPE_1_DIR="./toscrape_1"
TO_SCRAPE_2_DIR="./toscrape_2"
TO_SCRAPE_3_DIR="./toscrape_3"

WIKIPEDIA_0_DIR="./wikipedia_0"
WIKIPEDIA_1_DIR="./wikipedia_1"
WIKIPEDIA_2_DIR="./wikipedia_2"

MAX_DEPTH_0=0
MAX_DEPTH_1=1
MAX_DEPTH_2=2
MAX_DEPTH_3=3
MAX_DEPTH_10=10

NON_INTERNAL_URL="http://google.com"
NEGATIVE_DEPTH=-4

dirs=(
  "./Letters_0"
  "./Letters_1"
  "./Letters_2"
  "./Letters_10"
  "./toscrape_0"
  "./toscrape_1"
  "./toscrape_2"
  "./toscrape_3"
  "./wikipedia_0"
  "./wikipedia_1"
  "./wikipedia_2"
)

for dir in "${dirs[@]}"; do
  if [ -d "$dir" ]; then
    rm -rf "$dir"
  fi
  mkdir "$dir"
done

# Testing various erroneous inputs
# Not enough arguments
./crawler "$LETTERS_URL" "$LETTERS_0_DIR"
./crawler "$LETTERS_URL" "$MAX_DEPTH_0" 
./crawler "$LETTERS_0_DIR" "$MAX_DEPTH_0" 
./crawler "$LETTERS_URL" 
./crawler 

# Using non-internal URL
./crawler "$NON_INTERNAL_URL" "$LETTERS_0_DIR" "$MAX_DEPTH_0"

# Using a negative depth
./crawler "$LETTERS_URL" "$LETTERS_0_DIR" "$NEGATIVE_DEPTH"

# Using a character instead of an integer
./crawler "$LETTERS_URL" "$LETTERS_1_DIR" "a"

# Using proper input
#Letters with depth 0
./crawler "$LETTERS_URL" "$LETTERS_0_DIR" "$MAX_DEPTH_0"

#Letters with depth 1
./crawler "$LETTERS_URL" "$LETTERS_1_DIR" "$MAX_DEPTH_1"

#Letters with depth 2
./crawler "$LETTERS_URL" "$LETTERS_2_DIR" "$MAX_DEPTH_2"

#Letters with depth 10
./crawler "$LETTERS_URL" "$LETTERS_10_DIR" "$MAX_DEPTH_10"

#toscrape with depth 0
./crawler "$TO_SCRAPE_URL" "$TO_SCRAPE_0_DIR" "$MAX_DEPTH_0"

#toscrape with depth 1
./crawler "$TO_SCRAPE_URL" "$TO_SCRAPE_1_DIR" "$MAX_DEPTH_1"

#toscrape with depth 2
./crawler "$TO_SCRAPE_URL" "$TO_SCRAPE_2_DIR" "$MAX_DEPTH_2"

#toscrape with depth 3
./crawler "$TO_SCRAPE_URL" "$TO_SCRAPE_3_DIR" "$MAX_DEPTH_3"

#wikipedia with depth 0
./crawler "$WIKIPEDIA_URL" "$WIKIPEDIA_0_DIR" "$MAX_DEPTH_0"

#wikipedia with depth 1
./crawler "$WIKIPEDIA_URL" "$WIKIPEDIA_1_DIR" "$MAX_DEPTH_1"

#wikipedia with depth 2
./crawler "$WIKIPEDIA_URL" "$WIKIPEDIA_2_DIR" "$MAX_DEPTH_2"
