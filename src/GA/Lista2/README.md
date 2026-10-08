# Lista 2

Exercícios desenvolvidos para a **Lista de Exercícios 2** da Atividade Acadêmica **Processamento Gráfico: Fundamentos**, utilizando **C++ e OpenGL**.

## 📂 Exercícios

| Exercício | Descrição |
| ---------- | --------- |
| <code>Exercício&nbsp;1</code> | Configuração da **matriz de projeção ortográfica** para definir a janela do mundo (*window/ortho*) com os limites `xmin = -10`, `xmax = 10`, `ymin = -10` e `ymax = 10`. O exercício explora a representação e o posicionamento de objetos em um sistema de coordenadas cartesianas centralizado na origem. |
| <code>Exercício&nbsp;2</code> | Modificação da **projeção ortográfica** para os limites `xmin = 0`, `xmax = 800`, `ymin = 600` e `ymax = 0`. Essa configuração aproxima as coordenadas do mundo das coordenadas de tela, posicionando a origem no canto superior esquerdo e fazendo o eixo vertical crescer para baixo. |
| <code>Exercício&nbsp;3</code> | Desenho de uma **figura geométrica personalizada** utilizando a câmera 2D configurada no exercício anterior. O exercício demonstra como a projeção ortográfica com dimensões equivalentes às da janela facilita a definição das posições e proporções dos objetos diretamente na cena. |
| <code>Exercício&nbsp;4</code> | Alteração da **viewport** para renderizar a cena apenas no **quadrante superior direito** da janela da aplicação. O exercício permite observar como a área de exibição pode ser modificada independentemente da geometria desenhada, explorando o mapeamento da cena para uma região específica da tela. |
| <code>Exercício&nbsp;5</code> | Utilização de **quatro viewports** para reproduzir a mesma cena nos quatro quadrantes da janela. O exercício explora a divisão da área de renderização e a exibição de uma mesma geometria em diferentes regiões da tela. |
| <code>Exercício&nbsp;6</code> | Implementação da **criação interativa de triângulos por meio de cliques do mouse**. Cada clique define um vértice e, a cada três vértices registrados, um novo triângulo é desenhado com uma cor diferente. O exercício integra o tratamento de eventos do mouse com o uso de **VAOs, VBOs e projeção ortográfica**, permitindo gerar novas primitivas geométricas durante a execução. |

## 🎯 Objetivo

Compreender e aplicar os conceitos de **projeção ortográfica e mapeamento com a viewport** no OpenGL, explorando a relação entre os sistemas de coordenadas do mundo e da tela. Os exercícios abordam a configuração da janela de visualização, o posicionamento de objetos em cenas 2D, a renderização em diferentes regiões da aplicação e a criação interativa de primitivas geométricas por meio de eventos do mouse.
