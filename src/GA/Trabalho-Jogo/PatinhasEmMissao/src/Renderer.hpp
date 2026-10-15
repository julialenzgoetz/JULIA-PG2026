#pragma once
#include "Game.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <array>
#include <filesystem>
#include <map>
#include <string>

struct GLFWwindow;

// Renderer transforma os dados do Game em desenhos. Todos os elementos
// (cachorro, cenário, itens e letras) usam o MESMO quadrilátero e shader.
class Renderer {
public:
    Renderer(const std::filesystem::path& root, const Config& config);
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    void render(const Game& game, GLFWwindow* window, float menuTime);
    void screenshot(const std::string& filename, GLFWwindow* window);

private:
    GLuint shader = 0, vao = 0, vbo = 0, ebo = 0, white = 0;
    GLint uModel, uProjection, uUV, uTint, uRadius, uRectSize;
    std::map<std::string, GLuint> textures;
    std::array<float, 256> advances{};

    GLuint loadTexture(const std::filesystem::path& filename);
    void draw(GLuint texture, Rect rect, glm::vec4 uv = {0,0,1,1},
              glm::vec4 tint = {1,1,1,1}, float angle = 0, float radius = 0);
    void box(Rect rect, glm::vec4 color, float radius = 0);
    void outline(Rect rect, glm::vec4 color);
    void panel(Rect rect);
    void text(const std::string& value, float x, float y, float size, glm::vec4 color);
    float textWidth(const std::string& value, float size) const;
    void centered(const std::string& value, float centerX, float y, float size, glm::vec4 color);
    void item(int frame, Rect rect, glm::vec4 tint = {1,1,1,1}, float angle = 0);
    void background(float scroll);
    void hud(const Game& game);
    void menu(float t);
};
