# Verificação desta versão

## Compilação

- C++17, GCC 13.3.0, configuração Release, em Linux: compilação concluída.
- MinGW-w64 GCC 13, Windows x86-64, configuração Release: compilação cruzada concluída.
- Executável Windows com runtime C++ vinculado estaticamente. Inspeção PE identificou apenas DLLs de sistema nas dependências diretas: GDI32, KERNEL32, msvcrt, SHELL32 e USER32. A biblioteca gráfica do sistema é carregada pelo GLFW durante a execução.

## Regras do jogo

`ctest` executou o alvo `game_tests`, com verificações de:

- Interseção AABB em X e Y, separação e contato na borda.
- Altura máxima do pulo de acordo com a equação de movimento.
- Aterrissagem e bloqueio de pulo duplo.
- Pausa sem alteração de distância ou altura.
- Coleta com incremento de pontuação e remoção do item.
- Dano, intervalo de invulnerabilidade e bloqueio de dano duplicado.
- Game over ao esgotar vidas.
- Reinício dos dados e preservação do recorde da sessão.
- Duas corridas simuladas de 100 segundos: configuração inicial e progressão acelerada até a velocidade máxima, mantendo as vidas com um jogador automático.

Resultado: **todos os testes passaram**.

## Renderização

O jogo solicitou OpenGL 3.3 Core. O ambiente retornou **OpenGL 4.5 Core, Mesa 25.1.7, llvmpipe / LLVM 20.1.2**.
Os shaders usam `#version 330 core`. Foi usado OSMesa com a plataforma nula do GLFW para renderizar sem janela física.

| Execução | Resultado observado |
|---|---|
| Menu, 1 frame | Tela desenhada em 1280 × 720 |
| Corrida automática, 960 frames / 8 s simulados | 150 pontos, 10 petiscos, 3 vidas |
| Corrida com hitboxes, 1090 frames | 177 pontos, 12 petiscos, 3 vidas |
| Corrida sem pular, 2400 frames | Perda das 3 vidas e tela de game over |
| Cena de pausa | Painel de pausa renderizado |

`glGetError()` foi consultado durante essas execuções, sem erros reportados.
As capturas em `docs/images` foram produzidas pelo renderizador do próprio jogo e inspecionadas visualmente.
A captura de pausa foi obtida selecionando o estado pela opção de teste; o congelamento da lógica foi verificado pelos testes.

## Limites da verificação

- O executável `.exe` foi compilado e inspecionado, mas **não executado em Windows**.
- As teclas físicas, redimensionamento por mouse e mudança de foco de uma janela nativa precisam de uma conferência no computador de apresentação.
- O desempenho gráfico não foi medido no notebook da usuária. Os números de frames acima representam passos de teste, não uma medição de FPS.
- A distribuição para macOS não foi compilada nem executada.

## Conferência rápida no computador da apresentação

1. Extraia o ZIP e abra `JOGAR.bat`.
2. Pressione Enter e teste Espaço, A/D, P, H, R e Esc.
3. Colete um petisco, encoste em um obstáculo e confirme a mudança de pontos/vidas.
4. Redimensione a janela e confira se o jogo mantém a proporção.
5. Deixe as vidas terminarem e use Enter para reiniciar.

## Reprodução do teste sem janela em Linux

Com CMake, compilador e a biblioteca OSMesa disponíveis:

```bash
cmake -S . -B build-headless -DPATINHAS_HEADLESS_ONLY=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build-headless --parallel 4
ctest --test-dir build-headless --output-on-failure
./build-headless/PatinhasEmMissao --headless --smoke-test --frames 960 --capture partida.ppm
```

A opção `PATINHAS_HEADLESS_ONLY` desativa os backends de janela nativa desse build.
Use um build normal para jogar em uma janela no desktop.
