#pragma once

#include <random>
#include <string>
#include <vector>

// Toda a lógica usa uma tela virtual 1280 x 720, com origem no canto
// superior esquerdo: x cresce para a direita e y cresce para baixo.
// A janela pode mudar de tamanho; o Renderer mantém esta proporção.
constexpr float WORLD_W = 1280.f;
constexpr float WORLD_H = 720.f;
constexpr float GROUND_Y = 548.f;
constexpr float DOG_SIZE = 144.f;
constexpr float PI = 3.14159265359f;

struct Rect { float x, y, w, h; };
bool overlaps(const Rect& a, const Rect& b);

struct Config {
    int windowWidth = 1280, windowHeight = 720, lives = 3;
    float startSpeed = 310.f, maxSpeed = 560.f, acceleration = 2.f;
    float gravity = 1700.f, jumpSpeed = 780.f;
    float animationFps = 10.f, invulnerability = 1.35f;
    std::string dogTexture = "assets/dog.png";
    std::string skyTexture = "assets/sky.png";
    std::string hillsTexture = "assets/hills.png";
    std::string treesTexture = "assets/trees.png";
    std::string groundTexture = "assets/ground.png";
    std::string itemsTexture = "assets/items.png";
};
Config loadConfig(const std::string& path);

enum class Screen { Menu, Playing, Paused, GameOver };
enum class ObstacleType { Log, Rock, Puddle };

struct Player {
    float x = 150.f;
    float height = 0.f; // Altura acima do chão; positiva para cima.
    float velocity = 0.f;
    float jumpBuffer = 0.f;
    float immuneTime = 0.f;
    bool grounded = true;
    Rect bounds() const;
};
struct Obstacle {
    ObstacleType type;
    float x;
    bool hit = false; // O mesmo obstáculo só tira uma vida.
    Rect bounds() const;
};
struct Treat {
    float x, y, phase;
    Rect bounds(float time) const;
};
struct Particle {
    float x, y, vx, vy, life, maxLife;
};

// Game não conhece OpenGL. Guarda as regras e os dados da partida.
// Isso permite testar física e colisões sem precisar abrir uma janela.
class Game {
public:
    explicit Game(const Config& config, unsigned seed = 42);
    Config config;
    Screen screen = Screen::Menu;
    Player player;
    std::vector<Obstacle> obstacles;
    std::vector<Treat> treats;
    std::vector<Particle> particles;
    float time = 0.f, distance = 0.f, speed = 0.f;
    float spawnTimer = 1.4f, hitFlash = 0.f;
    int lives = 3, collected = 0, bestScore = 0;
    bool debugHitboxes = false;

    void start();
    void jump();
    void togglePause();
    void update(float dt, float horizontalInput = 0.f);
    int score() const;
    int dogFrame() const;

private:
    std::mt19937 random;
    float randomBetween(float low, float high);
    void spawnObstacle();
    void burst(float x, float y);
    void resolveCollisions();
};
