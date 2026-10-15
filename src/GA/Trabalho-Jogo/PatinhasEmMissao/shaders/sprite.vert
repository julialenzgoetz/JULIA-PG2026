#version 330 core

// Cada vértice tem posição (x,y) e coordenada de textura (u,v).
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;
uniform mat4 projection; // Tela virtual -> coordenadas de recorte.
uniform mat4 model;      // Quad unitário -> posição, rotação e tamanho.
uniform vec4 uvRect;     // x,y = início; z,w = tamanho do recorte no atlas.
out vec2 TexCoord;
out vec2 LocalCoord;

void main() {
    gl_Position = projection * model * vec4(aPosition, 0.0, 1.0);
    TexCoord = uvRect.xy + aTexCoord * uvRect.zw;
    LocalCoord = aTexCoord;
}
