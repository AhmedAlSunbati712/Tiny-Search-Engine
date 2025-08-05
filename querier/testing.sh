#!/bin/bash
# testing.sh - script for the querier program to test


VALGRIND="valgrind --leak-check=full --show-leak-kinds=all"

indexFileName="$HOME/cs50-dev/shared/tse/output/indexer/index-letters-10"
pageDirectory="$HOME/cs50-dev/shared/tse/output/crawler/pages-letters-depth-10"

: '
the index

playground 1 1 3 1
page 1 1 3 1
coding 7 1
this 1 1 3 1
home 1 2 2 1 3 2 4 1 5 1 6 1 7 1 8 1 9 1 10 1
depth 9 1
eniac 5 1
the 1 1 3 1
for 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1 10 1
breadth 4 1
tse 1 1 3 1
biology 10 1
first 4 1 9 1
search 4 1 9 1
huffman 7 1
traversal 6 1
transform 8 1
fourier 8 1
graph 6 1
algorithm 2 1
fast 8 1
computational 10 1

'


echo "===== Testing on invalid crawler directory ====="
./querier ../crawler/notACrawlerDir "$indexFileName"

echo
echo "-------------------------------------------------"


echo "===== Testing on invalid index pathname ====="
./querier "$pageDirectory" "../notAnActualIndexPath"

echo
echo "-------------------------------------------------"

echo "===== Testing invalid queries ====="
./querier "$pageDirectory" "$indexFileName" <<EOF
and word
word or
or
and
word and or
word and and word
word or or word
word and
or word
word or
EOF

echo
echo "-------------------------------------------------"

echo "===== Testing empty query ====="
./querier "$pageDirectory" "$indexFileName" <<EOF

EOF

echo
echo "-------------------------------------------------"


echo "===== Testing out nonnormalized queries ====="
./querier "$pageDirectory" "$indexFileName" <<EOF
playGrOuNd        or    paGe
       playgrOund or paGe   
playground or page
EOF

echo
echo "-------------------------------------------------"


echo "===== Testing out some queries ====="
./querier "$pageDirectory" "$indexFileName" <<EOF
playground
playground or page
playground page
playground and page
coding and playground
coding or playground
playground page or the tse
playground page and the tse
EOF

echo
echo "-------------------------------------------------"


echo "===== Testing valgrind ====="
$VALGRIND ./querier "$pageDirectory" "$indexFileName" <<EOF
coding or playground
EOF
