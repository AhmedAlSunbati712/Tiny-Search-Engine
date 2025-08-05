#!/bin/bash
# testing.sh - script for the indexer program to test

rm -f testing.out

# Testing on various invalid arguments
echo "===== Testing on invalid arguments =====" >> testing.out

echo "- No arguments supplied" >> testing.out
./indexer >> testing.out 2>&1

echo "- One argument" >> testing.out
./indexer >> testing.out 2>&1

echo "- Two arguments" >> testing.out
./indexer "$HOME/cs50-dev/shared/tse/output/crawler/pages-letters-depth-10" >> testing.out 2>&1

echo "- Four or more arguments" >> testing.out
./indexer "$HOME/cs50-dev/shared/tse/output/crawler/pages-letters-depth-10" "indexFileName" "another arg" >> testing.out 2>&1

echo "- Not a valid crawler directory" >> testing.out
./indexer "../crawler" "indexFileName">> testing.out 2>&1

echo "- Non-existen path for indexFileName" >> testing.out
./indexer "$HOME/cs50-dev/shared/tse/output/crawler/pages-letters-depth-10" "../nonExistentDirectory/indexFileName" >> testing.out 2>&1

echo "===== Done Testing arguments =====" >> testing.out

echo "===== Testing multiple pageDirectories and comparing with correct index =====" >> testing.out


declare -a DIRS=(
    "$HOME/cs50-dev/shared/tse/output/crawler/pages-letters-depth-10"
    "$HOME/cs50-dev/shared/tse/output/crawler/pages-letters-depth-2"
    "$HOME/cs50-dev/shared/tse/output/crawler/pages-toscrape-depth-1"
    
)
declare -a OUTPUTS=(
    "test-index-letters-10"
    "test-index-letters-2"
    "test-index-toscrape-1"
)
declare -a CORRECT=(
    "$HOME/cs50-dev/shared/tse/output/indexer/index-letters-10"
    "$HOME/cs50-dev/shared/tse/output/indexer/index-letters-2"
    "$HOME/cs50-dev/shared/tse/output/indexer/index-toscrape-1"
)

for i in ${!DIRS[@]}; do
    echo "===== Test $((i + 1)) =====" >> testing.out
    echo "Running indexer on ${DIRS[$i]}" >> testing.out

    ./indexer "${DIRS[$i]}" "${OUTPUTS[$i]}" >> testing.out

    if [ $? -eq 0 ]; then
        echo "Indexer completed successfully. Comparing with correct output..." >> testing.out
        $HOME/cs50-dev/shared/tse/indexcmp "${OUTPUTS[$i]}" "${CORRECT[$i]}" >> testing.out
    else
        echo "Indexer failed on ${DIRS[$i]}" >> testing.out
    fi
    echo "" >> testing.out
done
echo >> testing.out
echo "===== Testing indextest.c on" "${OUTPUTS[1]}" "=====" >> testing.out

echo "Running indextest on  ${OUTPUTS[1]}" >> testing.out
./indextest "${OUTPUTS[1]}" "newIndexFile" >> testing.out

echo "Comparing newIndexFile with" "${OUTPUTS[1]}" >> testing.out
$HOME/cs50-dev/shared/tse/indexcmp "${OUTPUTS[1]}" "newIndexFile" >> testing.out

echo "===== Test with Valgrind =====" >> testing.out
echo "Running indexer on ${DIRS[0]} with Valgrind" >> testing.out

valgrind --leak-check=full --show-leak-kinds=all ./indexer "${DIRS[0]}" "${OUTPUTS[0]}" >> testing.out 2>&1


echo >> testing.out
echo "Running indextest on ${OUTPUTS[0]} with Valgrind" >> testing.out
valgrind --leak-check=full --show-leak-kinds=all ./indextest "${OUTPUTS[1]}" "newIndexFile" >> testing.out 2>&1

