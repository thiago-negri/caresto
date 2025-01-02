#!/bin/bash

TEST_C=test/test.c

rm -rf test
mkdir -p test

# make sure project is built
./build.sh build

# generate test.c
echo "#include <stdio.h>" > $TEST_C

for source_file in src/caresto/*.c; do
    grep 'T_TEST' "$source_file" >/dev/null
    has_test=$?
    module=$(basename $source_file .c)
    if [ $has_test -eq 0 ]; then
        echo "#include <caresto/${module}.h>" >> $TEST_C
    fi
done

for source_file in src/caresto/*.c; do
    grep 'T_TEST' "$source_file" | sed 's/T_TEST(\(.*\)) {/void t_\1_(int *done);/' >> $TEST_C
done

echo "int main(void) {" >> $TEST_C
echo "int done = 0;" >> $TEST_C

for source_file in src/caresto/*.c; do
    grep 'T_TEST' "$source_file" | sed 's/T_TEST(\(.*\)) {/done = 0; t_\1_(\&done); if (done != 1) { fprintf(stderr, "missing T_DONE on \1\\n"); }/' >> $TEST_C
done

echo "return 0; }" >> $TEST_C

# compile test.c
clang test/test.c -c -o test/test.o -Isrc -Iinclude -Isrc-gen

# link test binary
clang build/obj/caresto/*.o test/*.o -o test/test.exe \
    -Llib/windows/SDL3/x64 \
    -Llib/windows/glew/x64 \
    -lSDL3 \
    -lglew32 \
    -lglu32 \
    -lopengl32 \
    -Xlinker /SUBSYSTEM:CONSOLE

cp lib/windows/SDL3/x64/SDL3.dll test/SDL3.dll
cp lib/windows/glew/x64/glew32.dll test/glew32.dll

./test/test.exe

