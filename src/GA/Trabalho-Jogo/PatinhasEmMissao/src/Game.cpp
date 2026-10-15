#include "Game.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <stdexcept>

// AABB = Axis-Aligned Bounding Box. Os retângulos precisam se sobrepor
// em AMBOS os eixos para ocorrer colisão. Contato na borda também conta.
bool overlaps(const Rect& a, const Rect& b) {
    return a.x <= b.x + b.w && a.x + a.w >= b.x &&
           a.y <= b.y + b.h && a.y + a.h >= b.y;
}

static std::string trim(std::string text) {
    const auto first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    return text.substr(first, text.find_last_not_of(" \t\r\n") - first + 1);
}

Config loadConfig(const std::string& path) {
    Config c;
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Nao foi possivel abrir " + path);
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line.substr(0, line.find('#')));
        if (line.empty()) continue;
        const auto equal = line.find('=');
        if (equal == std::string::npos)
            throw std::runtime_error("Configuracao: falta '=' na linha " + std::to_string(lineNumber));
        const auto key = trim(line.substr(0, equal));
        const auto value = trim(line.substr(equal + 1));
        auto number = [&]() {
            size_t used = 0;
            float v = std::stof(value, &used);
            if (used != value.size() || !std::isfinite(v))
                throw std::runtime_error("Valor numerico invalido: " + key);
            return v;
        };
        auto integer = [&]() {
            float v = number();
            if (v < 0 || v > 10000 || std::floor(v) != v)
                throw std::runtime_error("Valor inteiro invalido: " + key);
            return static_cast<int>(v);
        };
        if (key == "window_width") c.windowWidth = integer();
        else if (key == "window_height") c.windowHeight = integer();
        else if (key == "lives") c.lives = integer();
        else if (key == "start_speed") c.startSpeed = number();
        else if (key == "max_speed") c.maxSpeed = number();
        else if (key == "acceleration") c.acceleration = number();
        else if (key == "gravity") c.gravity = number();
        else if (key == "jump_speed") c.jumpSpeed = number();
        else if (key == "animation_fps") c.animationFps = number();
        else if (key == "invulnerability") c.invulnerability = number();
        else if (key == "dog_texture") c.dogTexture = value;
        else if (key == "sky_texture") c.skyTexture = value;
        else if (key == "hills_texture") c.hillsTexture = value;
        else if (key == "trees_texture") c.treesTexture = value;
        else if (key == "ground_texture") c.groundTexture = value;
        else if (key == "items_texture") c.itemsTexture = value;
        else throw std::runtime_error("Chave desconhecida: " + key);
    }
    if (c.windowWidth < 640 || c.windowWidth > 3840 || c.windowHeight < 360 ||
        c.windowHeight > 2160 || c.lives < 1 || c.lives > 5 ||
        c.startSpeed < 180 || c.startSpeed > 430 || c.maxSpeed < c.startSpeed ||
        c.maxSpeed > 600 || c.acceleration < 0 || c.acceleration > 5 ||
        c.gravity < 1200 || c.gravity > 2600 || c.jumpSpeed < 600 ||
        c.jumpSpeed > 950 || c.animationFps < 1 || c.animationFps > 30 ||
        c.invulnerability < .5f || c.invulnerability > 5 ||
        c.jumpSpeed * c.jumpSpeed / (2 * c.gravity) < 105)
        throw std::runtime_error("Configuracao fora dos limites. Consulte config/game.cfg.");
    return c;
}

Rect Player::bounds() const {
    // A imagem inclui áreas transparentes, o rabo e as orelhas.
    // Usamos um retângulo menor, correspondente ao corpo do cachorro.
    return {x + 42.f, GROUND_Y - DOG_SIZE * .83f - height + 36.f, 86.f, 78.f};
}
Rect Obstacle::bounds() const {
    switch (type) {
        case ObstacleType::Log: return {x + 6, GROUND_Y - 48, 82, 48};
        case ObstacleType::Rock: return {x + 8, GROUND_Y - 48, 54, 48};
        case ObstacleType::Puddle: return {x + 12, GROUND_Y - 12, 100, 12};
    }
    return {};
}
Rect Treat::bounds(float t) const {
    const float bob = std::sin(t * 4.f + phase) * 5.f;
    return {x + 7, y + 7 + bob, 30, 30};
}

Game::Game(const Config& c, unsigned seed) : config(c), random(seed) {
    speed = c.startSpeed;
    lives = c.lives;
}
float Game::randomBetween(float low, float high) {
    return std::uniform_real_distribution<float>(low, high)(random);
}
void Game::start() {
    // Reinicia tudo que pertence à partida, preservando o recorde da sessão.
    screen = Screen::Playing;
    player = {};
    obstacles.clear(); treats.clear(); particles.clear();
    time = distance = hitFlash = 0;
    speed = config.startSpeed;
    lives = config.lives;
    collected = 0;
    spawnTimer = 1.5f;
    for (int i = 0; i < 4; ++i) treats.push_back({720.f + i * 85.f, 477.f, i * .5f});
}
void Game::jump() {
    if (screen == Screen::Playing) player.jumpBuffer = .13f;
}
void Game::togglePause() {
    if (screen == Screen::Playing) screen = Screen::Paused;
    else if (screen == Screen::Paused) screen = Screen::Playing;
}
int Game::score() const { return static_cast<int>(distance / 50.f) + collected * 10; }
int Game::dogFrame() const {
    // Spritesheet: 4 colunas x 2 linhas. Índices na ordem de leitura.
    // 0..3 corrida; 4 parado; 5 subida; 6 descida; 7 pose feliz.
    if (screen == Screen::Menu) return 7;
    if (screen == Screen::GameOver) return 4;
    if (!player.grounded) return player.velocity > 0 ? 5 : 6;
    return static_cast<int>(time * config.animationFps) % 4;
}

void Game::spawnObstacle() {
    const auto type = static_cast<ObstacleType>(std::uniform_int_distribution<int>(0, 2)(random));
    obstacles.push_back({type, WORLD_W + 70, false});
    // Petiscos formam um arco sobre o obstáculo, sugerindo a trajetória do pulo.
    treats.push_back({WORLD_W - 40, GROUND_Y - 126, 0});
    treats.push_back({WORLD_W + 50, GROUND_Y - 199, 1});
    treats.push_back({WORLD_W + 140, GROUND_Y - 126, 2});
    // Intervalo maior que o tempo de voo: sempre há espaço para aterrissar.
    const float flight = 2.f * config.jumpSpeed / config.gravity;
    spawnTimer = std::max(1.50f, flight + .70f) + randomBetween(0, .60f);
}
void Game::burst(float x, float y) {
    for (int i = 0; i < 9; ++i) {
        float angle = randomBetween(0, 2 * PI);
        float force = randomBetween(45, 145);
        particles.push_back({x, y, std::cos(angle) * force, std::sin(angle) * force, .50f, .50f});
    }
}
void Game::resolveCollisions() {
    const Rect dog = player.bounds();
    treats.erase(std::remove_if(treats.begin(), treats.end(), [&](const Treat& treat) {
        if (overlaps(dog, treat.bounds(time))) {
            ++collected;
            burst(treat.x + 22, treat.y + 22);
            return true;
        }
        return treat.x < -60;
    }), treats.end());

    for (auto& obstacle : obstacles) {
        if (!obstacle.hit && player.immuneTime <= 0 && overlaps(dog, obstacle.bounds())) {
            obstacle.hit = true;
            --lives;
            player.immuneTime = config.invulnerability;
            hitFlash = .22f;
            if (lives <= 0) {
                screen = Screen::GameOver;
                bestScore = std::max(bestScore, score());
            }
            break;
        }
    }
    obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(),
        [](const Obstacle& o) { return o.x < -150; }), obstacles.end());
}

void Game::update(float dt, float horizontalInput) {
    if (screen != Screen::Playing || dt <= 0) return;
    time += dt;
    speed = std::min(config.maxSpeed, config.startSpeed + time * config.acceleration);
    distance += speed * dt;
    hitFlash = std::max(0.f, hitFlash - dt);
    player.immuneTime = std::max(0.f, player.immuneTime - dt);
    player.x = std::clamp(player.x + horizontalInput * 250.f * dt, 70.f, 400.f);

    // Buffer de pulo: um toque pouco antes de tocar o chão é aproveitado.
    // Impede pulo duplo, pois a velocidade só muda se grounded == true.
    if (player.jumpBuffer > 0 && player.grounded) {
        player.velocity = config.jumpSpeed;
        player.grounded = false;
        player.jumpBuffer = 0;
    }
    player.jumpBuffer = std::max(0.f, player.jumpBuffer - dt);
    if (!player.grounded) {
        // Integração exata do movimento com aceleração constante neste passo.
        player.height += player.velocity * dt - .5f * config.gravity * dt * dt;
        player.velocity -= config.gravity * dt;
        if (player.height <= 0) {
            player.height = player.velocity = 0;
            player.grounded = true;
        }
    }

    spawnTimer -= dt;
    if (spawnTimer <= 0) spawnObstacle();
    for (auto& o : obstacles) o.x -= speed * dt;
    for (auto& t : treats) t.x -= speed * dt;
    for (auto& p : particles) {
        p.x += p.vx * dt; p.y += p.vy * dt;
        p.vy += 160 * dt; p.life -= dt;
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle& p) { return p.life <= 0; }), particles.end());
    resolveCollisions();
    bestScore = std::max(bestScore, score());
}
