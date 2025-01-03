# Caresto

## TODOs

[GitHub Issues](https://github.com/thiago-negri/caresto/issues)


## Windows

### Compilador

Download & install LLVM: https://releases.llvm.org/ (using 19.1.0)

Download & install Visual Studio Installer: https://visualstudio.microsoft.com

No Visual Studio Installer, seleciona "Game Development with C++".


### Build

Baixa as dependências, executando o `dependencies.sh` se tiver os utilitários
Linux (bash, wget).

Se não tiver, da pra pedir pro ChatGPT converter o script Bash em PowerShell,
salvar como `dependencies.ps1` e rodar num terminal PowerShell.  Eu testei aqui
e funcionou também, só não comitei porque não quero manter os dois scripts.

```sh
./dependencies.sh
```

Com as dependências resolvidas, podes compilar e executar:

```sh
./build.sh
```

**Nota**: Tentei converter o script de build de Bash pra PowerShell via ChatGPT
mas não funcionou.

A saída da compilação vai ser mais ou menos essa na primeira execução:

```
mkdir -p build/debug/obj build/debug/obj/caresto build/debug/bin src-gen/gen
# convert glsl/fragment.glsl to src-gen/gen/glsl_fragment.h
# convert glsl/geometry.glsl to src-gen/gen/glsl_geometry.h
# convert glsl/vertex.glsl to src-gen/gen/glsl_vertex.h
clang -c -o build/debug/obj/caresto/g_graphics.o ./src/caresto/g_graphics.c -Wall -Iinclude -Isrc -Isrc-gen -fsanitize=address -g
clang -c -o build/debug/obj/caresto/mm_memory_management.o ./src/caresto/mm_memory_management.c -Wall -Iinclude -Isrc -Isrc-gen -fsanitize=address -g
clang -c -o build/debug/obj/caresto/t_test.o ./src/caresto/t_test.c -Wall -Iinclude -Isrc -Isrc-gen -fsanitize=address -g
clang -c -o build/debug/obj/main.o ./src/main.c -Wall -Iinclude -Isrc -Isrc-gen -fsanitize=address -g
clang -o build/debug/bin/main.exe build/debug/obj/*.o build/debug/obj/caresto/*.o -Llib/windows/SDL3/x64 -Llib/windows/glew/x64 -lSDL3 -lglew32 -lglu32 -lopengl32 -Xlinker /SUBSYSTEM:CONSOLE -fsanitize=address -g
   Criando biblioteca build\debug\bin\main.lib e objeto build\debug\bin\main.exp
cp lib/windows/SDL3/x64/SDL3.dll build/debug/bin/SDL3.dll
cp licenses/README-SDL.txt build/debug/bin/README-SDL.txt
cp lib/windows/glew/x64/glew32.dll build/debug/bin/glew32.dll
cp licenses/LICENSE-glew.txt build/debug/bin/LICENSE-glew.txt
./build/debug/bin/main.exe
App name: Caresto
App version: 0.0.1-dev
App ID: com.riverstonegames.caresto
SDL revision: preview-3.1.6-0-g78cc5c173 (libsdl.org)
That operation is not supported
GL: Version 4.3
GL: Compiled vertex shader.
GL: Compiled geometry shader.
GL: Compiled fragment shader.
```

Você vai ver uma tela preta com um sprite se mexendo que podes fechar clicando
no botão "X" da janela do Windows ou apertando a tecla Q no seu teclado.

Se alterar algum fonte e rodar o `build.sh` de novo, apenas os fontes alterados
vão ser recompilados.

O executável fica em `build/debug/bin`, junto com as DLLs que precisa e os
arquivos de licença das bibliotecas utilizadas.

Pra compilar uma versão que não abra um console (gera um artefato pra publicar):

```
./build.sh release
```

O executável de release fica em `build/release/bin`.


### Test

```sh
./test.sh
```

```
OK: ortho
OK: arena
```

