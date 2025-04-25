# Minecraft, mas eu fiz em 48 horas*

\* Na verdade eu atualizei ele depois — [veja este commit para a versão de 48 horas](https://github.com/jdah/minecraft-weekend/tree/cb19738305804b5734faa7118c1c784f26ff9463).

![screenshot](screenshots/1.png)

#### Funcionalidades:
- Mundo infinito, gerado proceduralmente
- Altura/profundidade infinitas
- Ciclo de dia e noite
- Biomas
- Jogador e entidades baseadas em ECS, com colisão e movimento completos
- Iluminação RGB completa
- Suporte total a transparência e translucidez
- Blocos com sprites (flores)
- Blocos animados (água e lava)
- Névoa de distância
- Muitos tipos diferentes de blocos
- E mais

#### Compilação

##### Sistemas Unix-like (Linux, macOS)
```bash
$ git clone --recurse-submodules https://github.com/jdah/minecraft-weekend.git
$ make
```

As seguintes bibliotecas estáticas dentro de `lib/` devem ser compiladas antes do projeto principal:

- GLAD: `lib/glad/src/glad.o`
- CGLM: `lib/cglm/.libs/libcglm.a`
- GLFW: `lib/glfw/src/libglfw3.a`
- libnoise: `lib/noise/libnoise.a`

Todas essas bibliotecas possuem seus próprios arquivos Makefile dentro de seus subdiretórios e podem ser compiladas com:
```bash
$ make libs
```

Se as bibliotecas não forem encontradas, certifique-se de que os submódulos foram clonados corretamente.

O executável do jogo, após a compilação com `$ make`, será encontrado em `./bin/`.

**Tenha certeza** de rodar com:
```bash
$ ./bin/game
```
a partir do diretório raiz do repositório.

Se você estiver recebendo erros como “não foi possível abrir o arquivo” (ex: "cannot find ./res/shaders/*.vs"), provavelmente está executando fora do diretório correto.

##### Windows

Boa sorte 🤷‍♂️ provavelmente tente compilar usando WSL (Subsistema do Windows para Linux) e um ambiente gráfico para exibir os gráficos.


#### PARA FELIPE: As alteracoes que fiz foi apenas traduzir este arquivo do ingles para o portugues
