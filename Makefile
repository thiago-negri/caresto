CC=clang
BUILD_PATH=build
C_SOURCES_PATH=src
C_OBJECTS_PATH=$(BUILD_PATH)/obj
C_SOURCES=$(wildcard $(C_SOURCES_PATH)/*.c)
C_OBJECTS=$(patsubst $(C_SOURCES_PATH)/%.c,$(C_OBJECTS_PATH)/%.o,$(C_SOURCES))
C_COMPILE_FLAGS=-Iinclude -I$(C_SOURCES_PATH)
C_LINK_FLAGS=-Llib/windows/SDL/x64 -Llib/windows/glew/x64 -lSDL2 -lglew32 -lglu32 -lopengl32
TARGET_PATH=$(BUILD_PATH)/bin
TARGET=$(TARGET_PATH)/main.exe

.PHONY: build
build: makedir $(TARGET)

.PHONY: rebuild
rebuild: makedir clean $(TARGET)

.PHONY: run
run: build dlls
	./$(TARGET)

.PHONY: dev
dev: compile_flags.txt

.PHONY: compile_flags.txt
compile_flags.txt:
	rm -f compile_flags.txt
	for flag in $(C_COMPILE_FLAGS); do echo $$flag >> compile_flags.txt; done

.PHONY: makedir
makedir:
	@mkdir -p $(TARGET_PATH) $(C_OBJECTS_PATH)

.PHONY: clean
clean:
	@rm -rf $(BUILD_PATH)

.PHONY: dlls
dlls: dll_sdl dll_glew

.PHONY: dll_sdl
dll_sdl: build/bin/SDL2.dll build/bin/README-SDL.txt

.PHONY: dll_glew
dll_glew: build/bin/glew32.dll build/bin/LICENSE-glew.txt

build/bin/SDL2.dll: lib/windows/SDL/x64/SDL2.dll
	cp $< $@

build/bin/README-SDL.txt: licenses/README-SDL.txt
	cp $< $@

build/bin/glew32.dll: lib/windows/glew/x64/glew32.dll
	cp $< $@

build/bin/LICENSE-glew.txt: licenses/LICENSE-glew.txt
	cp $< $@

$(TARGET): $(C_OBJECTS)
	$(CC) -o $(TARGET) $(C_OBJECTS) $(C_LINK_FLAGS)

$(C_OBJECTS_PATH)/%.o: $(C_SOURCES_PATH)/%.c
	$(CC) -c -o $@ $< $(C_COMPILE_FLAGS)

