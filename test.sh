#!/bin/bash

TEST_C=test/test.c

rm -rf test
mkdir -p test

# make sure project is built
./build.sh build

# generate test.c
echo "#include <stdio.h>" > $TEST_C

for source_file in $(find src/ -type f -name '*.c'); do
    grep 'ET_TEST' "$source_file" >/dev/null
    has_test=$?
    if [ $has_test -eq 0 ]; then
        module=$(basename $source_file .c)
        path=$(dirname $source_file | cut -f 2- -d '/')
        echo "#include <${path}/${module}.h>" >> $TEST_C
        grep 'ET_TEST' "$source_file" | \
            sed 's/ET_TEST(\(.*\)) {/void et_\1_(const char *name, int *done);/' >> $TEST_C
    fi
done

echo "int main(void) {" >> $TEST_C
echo "int done = 0;" >> $TEST_C

for source_file in $(find src/ -type f -name '*.c'); do
    grep 'ET_TEST' "$source_file" | \
        sed 's/ET_TEST(\(.*\)) {/done = 0; et_\1_("\1", \&done); if (done != 1) { fprintf(stderr, "missing ET_DONE on \1\\n"); }/' >> $TEST_C
done

echo "return 0; }" >> $TEST_C

# compile test.c
clang test/test.c -c -o test/test.o -Isrc -Iinclude -Isrc_gen -fsanitize=address -g

# link test binary
objs=$(find build/debug/obj -mindepth 2 -type f -name '*.o')
clang $objs test/*.o \
    -o test/test.exe -fsanitize=address -g \
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

