# Créditos e origem dos recursos

## Código e apoio

Protótipo desenvolvido para estudo do trabalho de Processamento Gráfico.
As decisões e os conceitos estão explicados no código e em `ROTEIRO_APRESENTACAO.md`.

Os materiais da disciplina e o enunciado orientaram a implementação de matrizes, spritesheets, camadas e AABB.

## Bibliotecas incluídas

| Componente | Origem | Licença |
|---|---|---|
| GLFW 3.4 | https://github.com/glfw/glfw/tree/3.4 | zlib/libpng; `third_party/glfw/LICENSE.md` |
| GLM 1.0.1 | https://github.com/g-truc/glm/tree/1.0.1 | Licenciamento descrito em `third_party/glm/LICENSE.txt` |
| GLAD, loader OpenGL | Arquivos distribuídos em https://github.com/JoeyDeVries/LearnOpenGL | Licença do gerador e termos dos arquivos em `third_party/glad` |
| Khronos khrplatform.h | Distribuído com o loader | Licença incluída no cabeçalho |
| DejaVu Sans Bold | DejaVu Fonts | `third_party/font/LICENSE.txt` |

GLAD foi obtido como código pré-gerado nos caminhos `includes/glad/glad.h`, `includes/KHR/khrplatform.h`
e `src/glad.c` do LearnOpenGL. O jogo solicita um contexto 3.3 e usa apenas os recursos necessários ao protótipo.

## Recursos visuais

- `assets/dog.png`: spritesheet gerada com a ferramenta integrada **ImageGen**  da inteligência artificial ChatGPT. A arte foi incorporada ao projeto, sem alterar a imagem original.
- Cenário, petiscos, obstáculos e coração: recursos vetoriais originais definidos em `tools/create_vector_assets.py`, com SVGs em `assets/vector_sources` e exportação PNG via CairoSVG.
- Texto: atlas gerado da fonte DejaVu Sans Bold por Pillow.
- Capturas em `docs/images`: produzidas pelo próprio renderizador OpenGL do jogo.

Não foi usada uma fotografia como referência do cachorro. O personagem é um caramelo fictício.
