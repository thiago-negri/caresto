# Caresto

## Windows

### Compilador

Download & install LLVM: https://releases.llvm.org/ (using 19.1.0)

Download & install Visual Studio Installer: https://visualstudio.microsoft.com

No Visual Studio Installer, seleciona "Game Development with C++".


### Build

Baixa as dependências, executando o `dependencies.sh` se tiver os utilitários Linux (bash, wget).

Se não tiver, da pra pedir pro ChatGPT converter o script Bash em PowerShell, salvar como `dependencies.ps1` e rodar
num terminal PowerShell.  Eu testei aqui e funcionou também, só não comitei porque não quero manter os dois scripts.

```sh
./dependencies.sh
```

Com as dependências resolvidas, podes compilar e executar:

```sh
./build.sh
```

**Nota**: Tentei converter o script de build de Bash pra PowerShell via ChatGPT mas não funcionou.

A saída da compilação vai ser mais ou menos essa na primeira execução:

```
mkdir -p build/obj build/bin src-gen/gen
# convert glsl/fragment.glsl to src-gen/gen/glsl_fragment.h
# convert glsl/vertex.glsl to src-gen/gen/glsl_vertex.h
clang -c -o build/obj/g_opengl.o ./src/g_opengl.c -Iinclude -Isrc -Isrc-gen
clang -c -o build/obj/main.o ./src/main.c -Iinclude -Isrc -Isrc-gen
clang -c -o build/obj/mm_memory_management.o ./src/mm_memory_management.c -Iinclude -Isrc -Isrc-gen
clang -o build/bin/main.exe build/obj/*.o -Llib/windows/SDL3/x64 -Llib/windows/glew/x64 -lSDL3 -lglew32 -lglu32 -lopengl32 -Xlinker /SUBSYSTEM:CONSOLE
cp lib/windows/SDL3/x64/SDL3.dll build/bin/SDL3.dll
cp licenses/README-SDL.txt build/bin/README-SDL.txt
cp lib/windows/glew/x64/glew32.dll build/bin/glew32.dll
cp licenses/LICENSE-glew.txt build/bin/LICENSE-glew.txt
./build/bin/main.exe
```

Você vai ver uma tela preta que podes fechar clicando no botão "X" da janela do Windows ou apertando a tecla Q
no seu teclado.

Se alterar algum fonte e rodar o `build.sh` de novo, apenas os fontes alterados vão ser recompilados.

O executável fica em `build/bin`, junto com as DLLs que precisa e os arquivos de licença das bibliotecas utilizadas.

Pra compilar uma versão que não abra um console:

```
./build.sh clean build release
```

