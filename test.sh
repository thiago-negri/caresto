#!/bin/bash

TEST_C=test/test.c

rm -rf test
mkdir -p test

echo "#include <stdio.h>" > $TEST_C

for source_file in src/*.c; do
    grep 'T_TEST' "$source_file" >/dev/null
    has_test=$?
    module=$(basename $source_file)
    if [ $has_test -eq 0 ]; then
        echo "#include <${module}>" >> $TEST_C
    fi
done

echo "int main(void) {" >> $TEST_C

for source_file in src/*.c; do
    grep 'T_TEST' "$source_file" | sed 's/T_TEST(\(.*\)) {/t_\1_();/' >> $TEST_C
done

echo "return 0; }" >> $TEST_C

clang src/t_test.c -c -o test/t_test.o -Isrc -Iinclude -Isrc-gen
clang test/test.c -c -o test/test.o -Isrc -Iinclude -Isrc-gen

clang test/*.o -o test/test.exe

./test/test.exe

