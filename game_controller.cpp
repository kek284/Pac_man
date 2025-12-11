#include"game_controller.h"
#include <algorithm>
#include<iostream>

namespace Action {
    //конструктор 
    Game_Controller::Game_Controller () {
        Pac_man.load_config("configs/pacman.cfg");
        for each ghosts:
            ghosts.load_config("configs/ghost.cfg");
        //привязываем движения к обьекту и к контроллеру
        Pac_man.set_movement(&pacman_mov);
        pacman_mov.set_game(this);
        pacman_mov.set_position(Pac_man.get_position_ptr());

        //добавить призраков 
    }

    void Game_Controller::load_map(const std::vector<std::vector<char>>& Value) {
        Map = Value;
    }

    void Game_Controller::redraw_map() {

        if (Map.empty()) {
        std::cout << "Map is empty\n";
        return;
        }

        std::vector<std::vector<char>> Map_screen = Map; //создаем скрин карты и туда будем добавлять пакмана и призраков 
            
        //наклыдваем пакмана
        Components::Position p_pos = Pac_man.get_current_position();

        if (p_pos.Y_pos < Map_screen.size() && p_pos.X_pos < Map_screen[p_pos.Y_pos].size()) {
            Map_screen[p_pos.Y_pos][p_pos.X_pos] = 'P';
        }
        //накладываем призраков
        for (int i = 0 ; i < ghosts.size(); ++i) {

            Components::Position gh_pos = ghosts[i].get_current_position();
            
            if (gh_pos.Y_pos < Map_screen.size() && gh_pos.X_pos < Map_screen[gh_pos.Y_pos].size()) {
                if (Map_screen[gh_pos.Y_pos][gh_pos.X_pos] == 'P') {
                    //минус жизнь
                    Map_screen[gh_pos.Y_pos][gh_pos.X_pos] = 'X';//пока что так 
                }
                Map_screen[gh_pos.Y_pos][gh_pos.X_pos] = 'G';
            }
        }
        // 4) Очистить экран (ANSI escape; нормально работает в Unix-терминалах и в Windows 10+)
        std::cout << "\x1B[2J\x1B[H";

        // 5) Вывести HUD (счёт, и, при наличии, жизни пакмана)
        std::cout << "Score: " << score;
        // Если добавишь метод get_life(), можно показывать: std::cout << "  Lives: " << pacman.get_life();
        std::cout << "\n\n";

        for (const auto& row : Map_screen) { //выводим ваму карту
            for (char c : row) {
                std::cout << c;
            }
            std::cout << "\n";
        }
        
    }

    void Game_Controller::update_map(void) {
        
        std::vector<std::vector<char>> Map_screen = Map; //создаем скрин карты и туда будем добавлять пакмана и призраков 
        Components::Position p_pos = Pac_man.get_current_position();
    
        if (Map_screen[p_pos.Y_pos][p_pos.X_pos] == '.') {
            score += 200;
            Map_screen[p_pos.Y_pos][p_pos.X_pos] = ' ';
        }
        if (Map_screen[p_pos.Y_pos][p_pos.X_pos] == 'o') { 
            Pac_man.Super_mode();//включаем режим когда можно кушать призраков
        }
        for (int i = 0; i < ghosts.size(); ++i) {
            Components::Position gh_pos = ghosts[i].get_current_position();
            if (p_pos.Y_pos == gh_pos.Y_pos && p_pos.X_pos == gh_pos.X_pos) { 
                Pac_man.pacman_hit(i);

                reset_pacman_position();
                reset_ghost_position(i);
            }
        }
        bool has_cookies = false; //точек не осталось
        for (auto &row : Map) {
            for (char c : row) {
                if (c == '.'){
                    has_cookies = true;
                    break;
                }
            } if (has_cookies) break;
        }
        if (!has_cookies) {
            std::cout << "YOU WIN!\n";
            //след уровень
        

    }
}
    
    void Game_Controller::reset_map(void) {
        score += 200;
    }
    void Game_Controller::update_scores(void) {

    }
    void Game_Controller::reset_pacman_position(void) {
        if (!Map.empty()) {
            Pac_man.update_position({0,0});
            pacman_mov.set_position(Pac_man.get_position_ptr());
        }

    }
    void Game_Controller::reset_ghost_position(uint8_t num_of_ghost) {
        if (!Map.empty()) {
            ghosts[num_of_ghost - 1].update_position({0,0});
        }

    }
    void Game_Controller::refill_cookies(void) {

    }
    bool Game_Controller::has_cookie(const Components::Position &atPosition) {
        if (Map.empty()) return false;
        if (atPosition.Y_pos >= Map.size()) return false;
        if (atPosition.X_pos >= Map[atPosition.X_pos].size()) return false;
        return Map[atPosition.Y_pos][atPosition.X_pos] == '.';
    }
    bool Game_Controller::is_position_valid(const Components::Position &atPosition) {
        if (Map.empty()) return false;
        if (atPosition.Y_pos >= Map.size()) return false;
        if (atPosition.X_pos >= Map[atPosition.X_pos].size()) return false;
        char c = Map[atPosition.Y_pos][atPosition.X_pos];
        return c != 'X';
    }
        
}
