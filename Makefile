CC=clang
BUILD_PATH=build
C_SOURCES_PATH=src
C_OBJECTS_PATH=$(BUILD_PATH)/obj
C_SOURCES=$(wildcard $(C_SOURCES_PATH)/*.c)
C_OBJECTS=$(patsubst $(C_SOURCES_PATH)/%.c,$(C_OBJECTS_PATH)/%.o,$(C_SOURCES))
C_COMPILE_FLAGS=-Iinclude -I$(C_SOURCES_PATH)
C_LINK_FLAGS=-Llib/windows/SDL/x64 -lSDL2
TARGET_PATH=$(BUILD_PATH)/bin
TARGET=$(TARGET_PATH)/main.exe

.PHONY: build
build: makedir $(TARGET)

.PHONY: rebuild
rebuild: makedir clean $(TARGET)

.PHONY: run
run: build dlls
	./$(TARGET)

.PHONY: makedir
makedir:
	@mkdir -p $(TARGET_PATH) $(C_OBJECTS_PATH)

.PHONY: clean
clean:
	@rm -rf $(BUILD_PATH)

.PHONY: dlls
dlls: build/bin/SDL2.dll build/bin/README-SDL.txt

build/bin/SDL2.dll: lib/windows/SDL/x64/SDL2.dll
	cp $< $@

build/bin/README-SDL.txt: licenses/README-SDL.txt
	cp $< $@

$(TARGET): $(C_OBJECTS)
	$(CC) -o $(TARGET) $(C_OBJECTS) $(C_LINK_FLAGS)

$(C_OBJECTS_PATH)/%.o: $(C_SOURCES_PATH)/%.c
	$(CC) -c -o $@ $< $(C_COMPILE_FLAGS)

