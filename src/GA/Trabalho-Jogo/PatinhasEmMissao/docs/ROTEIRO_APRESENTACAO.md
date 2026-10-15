# 🐾 Roteiro de apresentação — Patinhas em Missão

Este roteiro acompanha o código entregue. 

## 1. Proposta e funcionamento

O jogo é um endless runner 2D: o cachorro permanece em uma região da tela e o mundo se desloca para a esquerda.
O jogador pula obstáculos e coleta petiscos. Há três vidas, pontuação por distância e por coleta, dificuldade crescente,
pausa, reinício e recorde temporário da sessão.

Não há uma fase final: o objetivo é melhorar a pontuação.

## 2. Estrutura geral e game loop

`main.cpp` cria a janela e mantém o laço principal. Em cada frame:

1. `glfwPollEvents()` processa os eventos de teclado/janela.
2. As teclas A/D e setas são consultadas para o deslocamento contínuo.
3. O acumulador de tempo determina quantos passos de simulação executar.
4. `Game::update()` atualiza física, cenário lógico, colisões e pontuação.
5. `Renderer::render()` desenha o estado resultante.
6. `glfwSwapBuffers()` apresenta o frame.

As ações discretas, como pulo, são tratadas por `onKey` com `GLFW_PRESS`.
A repetição automática do teclado é ignorada para não produzir pulos involuntários.

A simulação tem passo fixo de **1/120 segundo**. O frame pode ter duração diferente, mas a lógica recebe sempre o mesmo passo.
Uma interrupção longa é limitada a 0,1 segundo de atualização por frame.

`Game` guarda as regras e os dados. `Renderer` cuida do OpenGL. Essa separação permite testar as regras sem abrir uma janela.

## 3. Como a geometria chega à GPU

Consulte o construtor de `Renderer`:

- O **VBO** armazena quatro vértices.
- Cada vértice contém **quatro floats**: posição X/Y e coordenada de textura U/V.
- O **EBO** armazena os índices `0, 1, 2, 2, 3, 0`, formando dois triângulos.
- O **VAO** registra a configuração dos atributos e o vínculo com os buffers.

O atributo 0 recebe a posição; o atributo 1 recebe a coordenada UV.
O stride de cada vértice é `4 * sizeof(float)`. O atributo de textura começa após dois floats.

O quadrilátero básico ocupa de `(0, 0)` a `(1, 1)`. Sua geometria é enviada uma vez para a GPU com `GL_STATIC_DRAW`.
Para desenhar outro objeto, o jogo reutiliza o mesmo quad e altera uniforms e textura.

O comando de desenho é `glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)`.
O deslocamento dos objetos não exige reenviar os vértices: ele é representado na matriz `model`.

## 4. Shaders, matrizes e sistema de coordenadas

O vertex shader recebe as matrizes `model` e `projection`:

```glsl
gl_Position = projection * model * vec4(aPosition, 0.0, 1.0);
```

`model` coloca o quad na posição e no tamanho de cada objeto. A rotação é feita em torno do centro.
Em `Renderer::draw()`, a composição é:

```text
T(posição + metade do tamanho) × R(ângulo) × S(tamanho) × T(-0,5; -0,5)
```

As transformações atuam da direita para a esquerda: centralizar, escalar, rotacionar e posicionar.
No personagem e no cenário o ângulo normalmente é zero; o código aceita rotação e a utiliza no petisco do menu.

A projeção ortográfica é `glm::ortho(0, 1280, 720, 0, -1, 1)`.
A origem é o canto superior esquerdo. X cresce para a direita e Y para baixo.
O topo esquerdo corresponde a `(-1,+1)` em NDC; o canto inferior direito corresponde a `(+1,-1)`.

A altura física do pulo cresce para cima. Por isso, ao desenhar, subtraímos essa altura da coordenada Y da posição no chão.

A janela mantém a proporção virtual 16:9: o viewport centraliza o jogo e cria faixas se a janela tiver outra proporção.

O fragment shader amostra a textura e multiplica por `tint`. O canal alfa controla a transparência.
Para os painéis da interface, uma função adicional suaviza os cantos; nos sprites ela fica desativada.

## 5. Sprites e spritesheets

O arquivo `assets/dog.png` tem quatro colunas e duas linhas.
Os frames 0–3 são a corrida; o 4 é parado; 5 e 6 são subida/descida; 7 é a pose do menu.

`Game::dogFrame()` escolhe o frame com base no estado e na velocidade vertical.
Para a corrida:

```cpp
frame = int(time * animationFps) % 4;
```

Não são carregados PNGs separados para cada pose. O shader recebe um retângulo UV:

```text
coluna = frame % 4
linha = frame / 4
u inicial = coluna / 4
v inicial = linha / 2
largura UV = 1/4
altura UV = 1/2
```

O vertex shader aplica `uvRect.xy + aTexCoord * uvRect.zw`.
O cachorro, os petiscos e as letras usam o mesmo princípio de recortar uma região de um atlas.

As imagens são carregadas com `stb_image` sem inversão vertical. O quad usa V=0 no topo,
compatível com o modo como a primeira linha dos PNGs é utilizada neste projeto.

IDs de textura ficam em `Renderer::textures`. Posição, tipo de objeto e estado de animação ficam nas estruturas de `Game`.

## 6. Camadas e parallax

`Renderer::background()` desenha, nesta ordem:

| Camada | Fator | Efeito |
|---|---:|---|
| Céu | 0 | Permanece parado |
| Colinas | 0,13 | Deslocamento lento, aparência distante |
| Árvores | 0,40 | Deslocamento intermediário |
| Chão | 1,00 | Acompanha a velocidade do mundo |

Para cada camada móvel:

```cpp
x = -fmod(distance * factor, WORLD_W);
```

São desenhadas duas cópias: uma em X e outra em `X + WORLD_W`.
Quando uma sai, a outra já ocupa a tela. Os SVGs do cenário foram desenhados para repetir horizontalmente.

O depth test fica desligado. A ordem de desenho compõe as camadas, e o blending usa
`GL_SRC_ALPHA` e `GL_ONE_MINUS_SRC_ALPHA` para combinar os pixels transparentes.

## 7. Pulo e movimentação

`Player` guarda posição horizontal, altura do salto, velocidade vertical e estado de contato com o chão.
O impulso inicial é 780 unidades/s, e a gravidade é 1700 unidades/s².

Em cada passo:

```text
altura = altura + velocidade × dt − 0,5 × gravidade × dt²
velocidade = velocidade − gravidade × dt
```

Ao chegar a uma altura menor ou igual a zero, a posição é ajustada ao chão e a velocidade é zerada.
Na configuração inicial, o ápice é de aproximadamente 179 unidades e o voo dura cerca de 0,92 segundo.

Só é possível iniciar o salto no chão. Há um pequeno buffer de 0,13 segundo para aproveitar
um toque dado imediatamente antes de aterrissar. Não existe pulo duplo.

As setas laterais/A/D movem o cachorro dentro de uma região limitada.
Obstáculos e petiscos são deslocados para a esquerda usando `x -= speed * dt`.

## 8. Colisões AABB

Cada elemento relevante fornece um retângulo com X, Y, largura e altura.
Uma colisão exige interseção no eixo X **e** no eixo Y:

```cpp
a.x <= b.x + b.w && a.x + a.w >= b.x &&
a.y <= b.y + b.h && a.y + a.h >= b.y
```

A hitbox do cachorro é menor que o quad, porque o PNG inclui transparência, rabo e orelhas.
Os obstáculos também têm retângulos ajustados à região de contato.

A coleta remove o petisco, soma 10 pontos e cria partículas.
O impacto tira uma vida e ativa invulnerabilidade. O obstáculo registra `hit=true`, impedindo repetição do dano pelo mesmo objeto.

Use **H** na demonstração: azul indica o cachorro, vermelho os obstáculos e amarelo os petiscos.
As hitboxes não giram com a arte; permanecem alinhadas aos eixos, como o nome AABB indica.

## 9. Estados, configurações e recursos

Os estados são `Menu`, `Playing`, `Paused` e `GameOver`.
`Game::update()` só avança a partida em `Playing`. Assim, a pausa congela física, animação e pontuação.
`Game::start()` reinicia os dados da partida e mantém apenas o recorde da sessão e a opção de visualizar hitboxes.

O arquivo de configuração permite trocar caminhos de texturas e parâmetros sem recompilar.
As imagens são carregadas uma vez. Texturas, buffers e programa de shader são liberados antes de destruir o contexto.

O texto da interface usa um atlas pré-gerado da fonte DejaVu Sans Bold. Não exige FreeType em tempo de execução.
Som e armazenamento permanente de recorde ficam como possíveis melhorias.

## Referências usadas

- Enunciado do trabalho Grau A e materiais fornecidos sobre camadas/sprites e colisões AABB.
- [GLFW: Getting started](https://www.glfw.org/docs/3.3/quick.html).
- [LearnOpenGL: Rendering Sprites](https://learnopengl.com/In-Practice/2D-Game/Rendering-Sprites).
- [LearnOpenGL: Collision detection](https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection).
