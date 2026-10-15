#include "Game.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

void check(bool value,const char* message) {
    if(!value) throw std::runtime_error(message);
}
void advance(Game& g,float duration) {
    const int steps=static_cast<int>(std::round(duration*120));
    for(int i=0;i<steps;++i) g.update(1.f/120);
}
int main(int argc,char** argv) {
    try {
        Config c=argc>1?loadConfig(argv[1]):Config{};
        check(overlaps({0,0,10,10},{5,5,10,10}),"AABB: intersecao");
        check(!overlaps({0,0,10,10},{12,5,10,10}),"AABB: separado em X");
        check(!overlaps({0,0,10,10},{5,12,10,10}),"AABB: separado em Y");
        check(overlaps({0,0,10,10},{10,0,10,10}),"AABB: contato na borda");
        Game g(c);
        g.start(); g.treats.clear(); g.spawnTimer=100;
        g.jump();
        float peak=0;
        for(int i=0;i<120;++i) {g.update(1.f/120);peak=std::max(peak,g.player.height);}
        const float expected=c.jumpSpeed*c.jumpSpeed/(2*c.gravity);
        check(std::abs(peak-expected)<.1f,"Pulo: altura maxima incorreta");
        check(g.player.grounded && g.player.height==0,"Pulo: nao aterrissou");
        g.jump(); advance(g,.10f); float velocity=g.player.velocity;
        g.jump(); g.update(1.f/120);
        check(g.player.velocity<velocity,"Pulo duplo indevido");
        advance(g,1);

        const float distance=g.distance, height=g.player.height;
        g.togglePause(); advance(g,1);
        check(g.distance==distance && g.player.height==height,"Pausa alterou a simulacao");
        g.togglePause();

        g.treats.push_back({g.player.x+50,477,0});
        g.update(1.f/120);
        check(g.collected==1 && g.treats.empty(),"Coleta do petisco");
        const int score=g.score(); advance(g,.1f);
        check(g.collected==1 && g.score()>=score,"Coleta duplicada");

        const int lives=g.lives;
        g.obstacles.push_back({ObstacleType::Rock,g.player.x+70,false});
        g.update(1.f/120);
        check(g.lives==lives-1 && g.player.immuneTime>0,"Colisao nao removeu vida");
        g.obstacles.push_back({ObstacleType::Log,g.player.x+70,false});
        g.update(1.f/120);
        check(g.lives==lives-1,"Invulnerabilidade falhou");
        g.obstacles.clear();
        while(g.lives>0) {
            g.player.immuneTime=0;
            g.obstacles.push_back({ObstacleType::Rock,g.player.x+70,false});
            g.update(1.f/120);
            g.obstacles.clear();
        }
        check(g.screen==Screen::GameOver,"Game over nao ativou");
        const int best=g.bestScore;
        g.start();
        check(g.lives==c.lives && g.collected==0 && g.distance==0 && g.obstacles.empty(),"Reinicio incompleto");
        check(g.bestScore==best,"Reinicio apagou recorde da sessao");

        // Simulação longa na velocidade máxima: autoplayer usa uma antecipação
        // de 0,23 s, e os obstáculos precisam permitir concluir os saltos.
        for (int mode=0;mode<2;++mode) {
            Config runConfig=c;
            if(mode==1) {runConfig.startSpeed=430;runConfig.maxSpeed=560;runConfig.acceleration=5;}
            Game runner(runConfig,13); runner.start();
            for(int i=0;i<120*100;++i) {
                if(runner.player.grounded) for(const auto& o:runner.obstacles) {
                    const float gap=o.bounds().x-(runner.player.bounds().x+runner.player.bounds().w);
                    if(gap>0 && gap<runner.speed*.23f) {runner.jump();break;}
                }
                runner.update(1.f/120);
            }
            check(runner.lives==runConfig.lives,"Obstaculos nao transponiveis no teste de 100 s");
            check(runner.collected>15,"Coleta em movimento nao funcionou");
            if(mode==1) check(runner.speed==runConfig.maxSpeed,"Limite de velocidade incorreto");
        }
        std::cout << "OK: AABB, pulo, aterrissagem, pausa, coleta, dano, imunidade, game over, reinicio e duas corridas de 100 s.\n";
        return 0;
    } catch(const std::exception& e) {
        std::cerr << "FALHOU: " << e.what() << '\n'; return 1;
    }
}
