#version 330 core
in vec2 TexCoord;
in vec2 LocalCoord;
out vec4 FragColor;
uniform sampler2D imageTexture;
uniform vec4 tint;
uniform float cornerRadius;
uniform vec2 rectSize;

void main() {
    FragColor = texture(imageTexture, TexCoord) * tint;
    // Recurso visual dos painéis: suaviza cantos arredondados no mesmo quad.
    // Nos sprites cornerRadius=0, então o recorte não é aplicado.
    if (cornerRadius > 0.0) {
        vec2 q = abs((LocalCoord - 0.5) * rectSize) - (rectSize * 0.5 - cornerRadius);
        float d = length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - cornerRadius;
        FragColor.a *= 1.0 - smoothstep(-0.8, 0.8, d);
    }
}
