# Créditos e origem dos recursos

## Código e apoio

Protótipo desenvolvido nesta conversa com auxílio de IA, para estudo do trabalho de Processamento Gráfico.
As decisões e os conceitos estão explicados no código e em `ROTEIRO_APRESENTACAO.md`.
Revise e compreenda o projeto antes de apresentar.

Os materiais da disciplina e o enunciado orientaram a implementação de matrizes, spritesheets, camadas e AABB.
Não foi copiado um projeto de jogo completo do repositório da disciplina.

## Bibliotecas incluídas

| Componente | Origem | Licença |
|---|---|---|
| GLFW 3.4 | https://github.com/glfw/glfw/tree/3.4 | zlib/libpng; `third_party/glfw/LICENSE.md` |
| GLM 1.0.1 | https://github.com/g-truc/glm/tree/1.0.1 | Licenciamento descrito em `third_party/glm/LICENSE.txt` |
| GLAD, loader OpenGL | Arquivos distribuídos em https://github.com/JoeyDeVries/LearnOpenGL | Licença do gerador e termos dos arquivos em `third_party/glad` |
| Khronos khrplatform.h | Distribuído com o loader | Licença incluída no cabeçalho |
| stb_image | https://github.com/nothings/stb/tree/f58f558c120e9b32c217290b80bad1a0729fbb2c3 | Domínio público/MIT; termos ao final do cabeçalho |
| DejaVu Sans Bold | DejaVu Fonts | `third_party/font/LICENSE.txt` |

GLAD foi obtido como código pré-gerado nos caminhos `includes/glad/glad.h`, `includes/KHR/khrplatform.h`
e `src/glad.c` do LearnOpenGL. O jogo solicita um contexto 3.3 e usa apenas os recursos necessários ao protótipo.

## Recursos visuais

- `assets/dog.png`: spritesheet gerada com a ferramenta integrada **ImageGen** nesta conversa. A arte foi incorporada ao projeto, sem alterar a imagem original.
- Cenário, petiscos, obstáculos e coração: recursos vetoriais originais definidos em `tools/create_vector_assets.py`, com SVGs em `assets/vector_sources` e exportação PNG via CairoSVG.
- Texto: atlas gerado da fonte DejaVu Sans Bold por Pillow.
- Capturas em `docs/images`: produzidas pelo próprio renderizador OpenGL do jogo.

Não foi usada uma fotografia como referência do cachorro. O personagem é um caramelo fictício, sem pretensão de reproduzir o Vitório.

## Prompt da spritesheet

Ferramenta: ImageGen integrada, com fundo transparente.

> Use case: stylized-concept. Asset type: production 2D platformer sprite sheet for a C++ OpenGL game called Patinhas em Missão. Create exactly ONE PNG sprite sheet with a genuinely transparent background, canvas aspect ratio 2:1, ideally 1536x768 pixels, divided invisibly into EXACTLY 4 equal columns and 2 equal rows, eight equal square cells. Each cell contains the SAME original cute small caramel-colored dog with floppy ears, dark brown ear tips, cream muzzle and belly, black nose and a teal bandana, in strict RIGHT-FACING SIDE VIEW, full body and tail visible. Clean charming hand-drawn 2D game art, bold warm dark-brown outlines, flat color with restrained shading, crisp readable silhouette. Body scale, body center, and placement must be identical in every cell, generous transparent padding on every side, no parts crossing a cell boundary. Feet baseline at 82 percent of each cell's height, body occupying about 76 percent width. Top row cells 1-4: four sequential distinct running poses for a loop, legs alternate extended and gathered, ears subtly bounce. Bottom row cell 1: standing idle, bottom row cell 2: upward jump legs tucked, bottom row cell 3: downward jump landing stretch, bottom row cell 4: happy standing with tail raised. No scenery, no ground, no shadow, no grid, no labels, no text, no extra dogs in a cell. All eight cells need a right-facing dog of the same design and scale. This sheet will be sampled by exact quarter-width and half-height UV rectangles in a game.
