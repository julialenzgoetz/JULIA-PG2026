# 🐶 Patinhas em Missão

Protótipo de jogo 2D para **Processamento Gráfico: Fundamentos — Grau A**.
Um cachorro caramelo corre pelo parque, coleta petiscos e pula troncos, pedras e poças.
Desenvolvido em **C++ com OpenGL 3.3 Core**, shaders GLSL, GLFW, GLAD e GLM.

![Menu do jogo](docs/images/menu.png)

## ▶️ Jogar no Windows

1. **Extraia toda a pasta do ZIP**. Não execute de dentro do arquivo compactado.
2. Abra **JOGAR.bat**.
3. Pressione **Enter** para começar e **Espaço** para pular.

O pacote inclui `bin/Windows/PatinhasEmMissao.exe`, compilado para Windows 64 bits.
Para usar essa versão, não é necessário instalar compilador, Python ou bibliotecas de desenvolvimento.
Mantenha as pastas `assets`, `config` e `shaders` junto ao projeto.
O computador precisa de driver gráfico com suporte a OpenGL 3.3 ou superior.

## 🎮 Controles

| Tecla | Ação |
|---|---|
| Enter | Começar, jogar novamente ou retomar uma pausa |
| Espaço, W ou seta para cima | Pular quando estiver no chão |
| A / D ou setas esquerda / direita | Ajustar a posição horizontal |
| P | Pausar e continuar |
| R | Reiniciar a partida |
| H | Mostrar ou esconder as hitboxes de colisão |
| Esc | Voltar ao menu; no menu, fechar o jogo |

Ao alternar para outra janela, a partida pausa automaticamente.
Não é preciso apertar uma tecla para correr: o cenário avança sozinho.
O botão desenhado no menu indica o comando de teclado; esta versão não usa clique do mouse.

## 🍖 Regras

- Cada petisco vale **10 pontos**.
- A distância também pontua: cada 50 unidades do mundo virtual valem 1 ponto e são apresentadas como 1 metro fictício.
- Você começa com **3 vidas**; encostar em um obstáculo retira uma vida.
- Depois do impacto, o cachorro pisca e fica **1,35 segundo invulnerável**.
- A velocidade aumenta aos poucos, de 310 até 560 unidades por segundo.
- Os obstáculos são separados por um intervalo que permite concluir o salto.
- Ao perder todas as vidas, aparece a tela de fim de partida.
- O recorde é mantido **durante a sessão** e é zerado ao fechar o programa.

![Uma partida](docs/images/play.png)

## 🛠️ Compilar e editar

Para modificar o código, abra a **pasta inteira** no Visual Studio ou VS Code.
O arquivo `src/main.cpp` tem a inicialização e o laço principal comentados por etapas.
As regras estão em `src/Game.cpp`, e o desenho está em `src/Renderer.cpp`.

### Windows — Visual Studio

Instale o Visual Studio com o componente **Desenvolvimento para desktop com C++** e as ferramentas de CMake.
Também é possível usar o CMake instalado separadamente e o compilador do Visual Studio.
Abra um terminal de desenvolvimento do Visual Studio na pasta do projeto e execute:

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4
.\build\Release\PatinhasEmMissao.exe --assets .
```

O script **COMPILAR_WINDOWS.bat** executa a configuração, a compilação e abre a versão recém-compilada.
O VS Code é um editor: ele precisa de um compilador C++ e CMake instalados para compilar.

### Windows — MinGW

Com MinGW-w64 e CMake disponíveis no terminal:

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build-mingw --parallel 4
.\build-mingw\PatinhasEmMissao.exe --assets .
```

Use uma pasta de build diferente para cada compilador ou gerador CMake.
Se usa Ninja, substitua `-G "MinGW Makefiles"` por `-G Ninja`.

### Linux

Em Ubuntu/Debian, instale as ferramentas de compilação e os cabeçalhos X11 necessários ao GLFW:

```bash
sudo apt install build-essential cmake libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 4
./build/PatinhasEmMissao --assets .
```

### macOS

Com as Command Line Tools do Xcode e CMake instalados:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 4
./build/PatinhasEmMissao --assets .
```

A configuração de macOS está prevista no código, mas não foi validada nesta entrega.

**As bibliotecas de terceiros estão incluídas em `third_party`: o CMake não baixa dependências.**
Não é necessário executar o script Python de autoria de imagens para compilar ou jogar.

## 📂 Organização

| Arquivo/pasta | Função |
|---|---|
| `src/main.cpp` | Janela, carregamento do OpenGL, teclado, tempo e game loop |
| `src/Game.hpp` | Dados do jogador, obstáculos, petiscos, configuração e estados |
| `src/Game.cpp` | Pulo, movimento, coleta, colisões, vidas, pontuação e reinício |
| `src/Renderer.hpp` e `src/Renderer.cpp` | Buffers, texturas, matrizes, sprites, camadas e interface |
| `shaders/sprite.vert` | Aplica `projection * model` e calcula o recorte da textura |
| `shaders/sprite.frag` | Amostra a textura, aplica cor/alfa e arredonda painéis |
| `config/game.cfg` | Parâmetros da partida e caminhos das imagens |
| `assets/dog.png` | Spritesheet do cachorro: 4 colunas × 2 linhas |
| `assets/items.png` | Petisco animado, tronco, pedra, poça e coração |
| `assets/sky.png`, `hills.png`, `trees.png`, `ground.png` | Camadas do parque |
| `assets/font.png` e `font.metrics` | Atlas de letras e avanços de texto |
| `assets/vector_sources` | SVGs originais dos recursos vetoriais |
| `tools/create_vector_assets.py` | Ferramenta opcional para regenerar cenário, itens e atlas da fonte |
| `tests/game_tests.cpp` | Testes das regras, sem dependência de OpenGL |
| `docs/ROTEIRO_APRESENTACAO.md` | Explicação dos conceitos e relação com o enunciado |
| `docs/CREDITOS.md` | Bibliotecas, referências e origem dos recursos visuais |

## ⚙️ Personalização

Edite `config/game.cfg` e reinicie o programa para alterar velocidade, vidas, gravidade, impulso do pulo,
quadros por segundo da animação, janela ou caminhos das imagens.
Os limites aceitos estão comentados no arquivo. Mantenha a altura do pulo suficiente para os obstáculos.
A verificação de configuração evita valores inválidos, mas mudanças de física e velocidade ainda exigem testar a jogabilidade.

Para trocar o personagem, preserve a disposição de 4 colunas × 2 linhas do atlas.
Os frames 0–3 mostram corrida; 4, parado; 5, subida; 6, descida; 7, pose feliz.
As hitboxes foram ajustadas para o personagem incluído. Se a silhueta mudar, ajuste `Player::bounds()`.

## ✅ Relação com o enunciado

| Requisito | Implementação |
|---|---|
| OpenGL 3.3 ou superior e shaders | Contexto Core 3.3 e GLSL `#version 330 core` |
| Buffers de geometria | VAO, VBO com posição/UV e EBO com seis índices |
| Matriz de projeção | `glm::ortho(0, 1280, 720, 0, -1, 1)` |
| Matriz dos objetos | Translação, rotação em torno do centro e escala |
| Texturas sobre polígonos | Cada sprite é um quad formado por dois triângulos |
| Animação por spritesheet | Corrida/pulo do cachorro e rotação dos petiscos |
| Fundo em camadas | Céu, colinas, árvores e chão |
| Parallax | Camadas com fatores de deslocamento 0; 0,13; 0,40; 1,0 |
| Controle do personagem | Teclado: pulo e deslocamento horizontal |
| Colisão por hitboxes | AABB para coleta e obstáculos; tecla H para visualizar |
| Configuração recomendada | `config/game.cfg` |
| Extras implementados | HUD, menu, pausa, game over e partículas de coleta |

Som não foi incluído nesta primeira versão; é opcional no enunciado.
Antes de enviar, confira as instruções de formato e envio disponibilizadas no Moodle.

## 🧪 Testes

Depois de compilar:

```bash
ctest --test-dir build -C Release --output-on-failure
```

Para uma execução gráfica automática de 10 segundos simulados:

```bash
./build/PatinhasEmMissao --smoke-test --frames 1200
```

No Windows, use o caminho do executável gerado, como `build\Release\PatinhasEmMissao.exe`.
A opção `--smoke-test` ativa um jogador automático para validação; não é necessária para jogar.

Se aparecer “arquivo ausente”, abra o jogo pelo `JOGAR.bat` ou use `--assets` com a pasta que contém `assets`, `config` e `shaders`.
Se falhar a criação de contexto OpenGL, confira o driver da placa de vídeo. O terminal mantém a mensagem de erro para diagnóstico.

