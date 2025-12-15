#include "game_controller.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <unistd.h>

namespace Action {

// Конструктор
Game_Controller::Game_Controller() : score(0), level(1), base_ghost_speed(35), game_won(false){
    // Загружаем конфиг Pacman
    Pac_man.load_config("configs/pacman.cfg");
    // Инициализируем движение Pacman
    Pac_man.init_movement(this);
}

void Game_Controller::reset_pacman_position() {
    if (!Map.empty()) {
        Pac_man.reset_to_default();
    }
}

bool Game_Controller::is_game_over() const {
    return Pac_man.get_life() == 0;
}
bool Game_Controller::is_game_won() const {
    return game_won;
}
// Загружаем конфиг призраков
void Game_Controller::load_ghosts_config(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;

    std::vector<std::string> block;
    std::string line;

    while (std::getline(file, line)) {
        if (line.find("ghost:") != std::string::npos) {
            if (!block.empty()) {
                Components::Ghost g;               
                g.load_config_block(block);
                //скорость в завим=симости от уровня
                int speed_for_level = std::max(1, base_ghost_speed - (level - 1));
                g.set_speed(speed_for_level);
                g.init_movement(this);
                ghosts.push_back(std::move(g));                       // <-- добавили push_back

                //назначем поведение. Пусть четные призраки приследуют а нечетные рандомно дивгаются
                ghosts.back().set_behavior(((int)ghosts.size() - 1) % 2 == 0) ;    
                block.clear();
            }
        } else if (!line.empty()) {
            block.push_back(line);
        }
    }

    if (!block.empty()) {
        Components::Ghost g;
        g.load_config_block(block);
        g.init_movement(this);
        ghosts.push_back(std::move(g));                           // <-- добавили push_back

        ghosts.back().set_behavior(((int)ghosts.size() - 1) % 2 == 0);  // переместить последний блок
    }
}

bool Game_Controller::load_map_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Map not found: " << filename << "\n";
        return false;
    }

    Map.clear();
    ghosts.clear();  // Очищаем перед загрузкой

    std::string line;
    int y = 0;

    while (std::getline(file, line)) {
        std::vector<char> row;

        for (int x = 0; x < static_cast<int>(line.size()); ++x) {
            char c = line[static_cast<size_t>(x)];

            if (c == 'P') {
                // Явно инициализируем позицию Pacman
                Components::Position pac_pos{x, y};
                Pac_man.set_default_position(pac_pos);
                Pac_man.reset_to_default();  // Сбрасываем на стартовую позицию
                Pac_man.init_movement(this); // Инициализируем движение
                row.push_back(' ');
                } 
            else if (c == 'G') {
                // Создаем призрака и явно инициализируем его позицию
                Components::Ghost g;
                Components::Position ghost_pos{x, y};
                g.set_default_position(ghost_pos);
                g.reset_to_default();  // Сбрасываем на стартовую позицию
                g.init_movement(this);
                
                int speed_for_level = std::max(1, base_ghost_speed - (level - 1));
                g.set_speed(speed_for_level);
                ghosts.push_back(std::move(g)); // move
                
                ghosts.back().set_behavior(((int)ghosts.size() - 1) % 2 == 0);
                row.push_back(' ');
                
                std::cout << "Ghost placed at: (" << x << "," << y << ")\n";
            } 
            else {
                row.push_back(c);
            }
        }

        Map.push_back(row);
        y++;
    }
    OriginalMap = Map;
    return true;
}

// Перерисовываем карту
void Game_Controller::redraw_map() {
    if (Map.empty()) {
        std::cout << "Map is empty\n";
        return;
    }

    std::vector<std::vector<char>> Map_screen = Map;
    Components::Position p_pos = Pac_man.get_current_position();

    if (p_pos.Y_pos >= 0 && p_pos.Y_pos < (int)Map_screen.size() &&  // Добавили >= 0
        p_pos.X_pos >= 0 && p_pos.X_pos < (int)Map_screen[p_pos.Y_pos].size()) {  // Добавили >= 0
        Map_screen[p_pos.Y_pos][p_pos.X_pos] = 'P';
    }

    for (size_t i = 0 ; i < ghosts.size(); ++i) {
        Components::Position gh_pos = ghosts[i].get_current_position();
        if (gh_pos.Y_pos >= 0 && gh_pos.Y_pos < (int)Map_screen.size() &&  // Добавили >= 0
            gh_pos.X_pos >= 0 && gh_pos.X_pos < (int)Map_screen[gh_pos.Y_pos].size()) {  // Добавили >= 0
            if (Map_screen[gh_pos.Y_pos][gh_pos.X_pos] == 'P') {
                Map_screen[gh_pos.Y_pos][gh_pos.X_pos] = 'X';
            }
            Map_screen[gh_pos.Y_pos][gh_pos.X_pos] = 'G';
        }
    }

    
        // Перемещаем курсор в начало экрана (не трогаем скроллбэк)
        std::cout << "\x1B[H";

        bool pac_super = Pac_man.is_super_mode();
        int lives = Pac_man.get_life();
    
        // Построим hearts_display как у вас
        const char* heart = u8"♥";
        const char* heart_red_prefix = "\033[1;31m";
        const char* color_reset = "\033[0m";
        std::string hearts_display;
        if (lives > 0) {
            for (int i = 0; i < lives; ++i) {
                hearts_display += heart_red_prefix;
                hearts_display += heart;
                hearts_display += color_reset;
                if (i + 1 < lives) hearts_display += ' ';
            }
        } else {
            hearts_display = "0";
        }
    
        // Очистить и вывести строку статуса
        std::cout << "\033[2K"; // очистить текущую строку полностью
        std::cout << "Score: " << score << "  Level: " << level << "  Lives: " << hearts_display << "\n";
    
        // Индикатор супер-режима — очистим следующую строку и выведем
        std::cout << "\033[2K";
        if (pac_super) {
            std::cout << "\033[1;33m*** SUPER MODE ACTIVE! ***\033[0m\n\n";
        } else {
            std::cout << "\n";
        }
    
        // Теперь каждую строку карты очищаем перед печатью, чтобы не оставались хвосты старых строк
        for (int y = 0; y < (int)Map_screen.size(); ++y) {
            std::cout << "\033[2K";
            for (int x = 0; x < (int)Map_screen[y].size(); ++x) {
                char c = Map_screen[y][x];
                if (c == 'P' && pac_super) {
                    std::cout << "\033[1;32mP\033[0m";
                } else if (c == 'P') {
                    std::cout << "\033[1;33mP\033[0m";
                } else if (c == 'G') {
                    std::cout << "\033[1;31mG\033[0m";
                } else if (c == 'X') {
                    std::cout << "\033[1;35mX\033[0m";
                } else if (c == 'o') {
                    std::cout << "\033[1;34mo\033[0m";
                } else {
                    std::cout << c;
                }
            }
            std::cout << "\n";
        }
    
        std::cout.flush();
    }


void Game_Controller::update_ghosts() {
    if (Map.empty()) return;

    Components::Position p_pos = Pac_man.get_current_position();
    bool pac_super = Pac_man.is_super_mode();

    for (size_t i = 0; i < ghosts.size(); ++i) {
        ghosts[i].update_gh_mov(p_pos, pac_super);
    }

    check_collisions();
}
// Обновление состояния карты связь только с пакманом 
void Game_Controller::update_map() {
    if (Map.empty()) return;

    Components::Position p_pos = Pac_man.get_current_position();
   
    if (Map[p_pos.Y_pos][p_pos.X_pos] == '.') {
        score += 200;
        Map[p_pos.Y_pos][p_pos.X_pos] = ' ';
    }

    if (Map[p_pos.Y_pos][p_pos.X_pos] == 'o') {
        Map[p_pos.Y_pos][p_pos.X_pos] = ' ';
        score += 1000;
        Pac_man.activate_super_mode();
    }

    check_collisions();

    // Проверка победы
    bool has_cookies = false;
    for (auto &row : Map) {
        for (char c : row) {
            if (c == '.') {
                has_cookies = true;
                break;
            }
        }
        if (has_cookies) break;
    }

    if (!has_cookies && !game_won) {
        next_level();
    } 
    Pac_man.update_super_mode();
}

void Game_Controller::check_collisions() {
    Components::Position p_pos = Pac_man.get_current_position();
    for (size_t i = 0; i < ghosts.size(); ++i) {
        Components::Position gh_pos = ghosts[i].get_current_position();
        if (p_pos.Y_pos == gh_pos.Y_pos && p_pos.X_pos == gh_pos.X_pos) {
            if (Pac_man.is_super_mode()) {
                score += 100000;
                reset_ghost_position((int)i);
            } else {
                Pac_man.lose_life();
                reset_pacman_position();
                reset_ghost_position((int)i);
            }
        }
    }
}

void Game_Controller::reset_map() {
    score = 0;
}

// Сброс позиции конкретного призрака
void Game_Controller::reset_ghost_position(int num_of_ghost) {
    if (!Map.empty() && num_of_ghost >= 0 && num_of_ghost < static_cast<int>(ghosts.size())) {
        ghosts[num_of_ghost].reset_to_default();
    }
}

// Проверка наличия cookie
bool Game_Controller::has_cookie(const Components::Position &atPosition) {
    if (Map.empty()) return false;
    if (atPosition.Y_pos < 0 || atPosition.Y_pos >= (int)Map.size()) return false;  // Добавили < 0
    if (atPosition.X_pos < 0 || atPosition.X_pos >= (int)Map[atPosition.Y_pos].size()) return false;  // Добавили < 0
    return Map[atPosition.Y_pos][atPosition.X_pos] == '.';
}

void Game_Controller::increase_ghost_speed() {
    int speed_for_level = std::max(1, base_ghost_speed - (level - 1));
    for (auto &g :ghosts) {
        g.set_speed(speed_for_level);
    }
}

void Game_Controller::next_level() {
    if (level < max_levels) {
        level++;
        std::cout << "Starting level " << level << "\n";
        increase_ghost_speed();
        // можно просто восстановить cookies на текущей карте
        refill_cookies();
        reset_pacman_position();
        for (size_t i = 0; i < ghosts.size(); ++i) reset_ghost_position((int)i);
    } else {
        std::cout << "YOU WIN!\n";
        redraw_map();
        sleep(2); 
        game_won = true;
    }
    
}

// Проверка валидности позиции
bool Game_Controller::is_position_valid(const Components::Position &atPosition) {
    if (Map.empty()) {
        return false;
    }
    if (atPosition.Y_pos < 0 || atPosition.Y_pos >= (int)Map.size()) {  // Добавили проверку на < 0
        return false;
    }
    if (atPosition.X_pos < 0 || atPosition.X_pos >= (int)Map[atPosition.Y_pos].size()) {  // Добавили проверку на < 0
       return false;
    }
    
    char c = Map[atPosition.Y_pos][atPosition.X_pos];
    bool valid = (c != '#');
    return valid;
}

void Game_Controller::refill_cookies() {
    if (OriginalMap.empty()) {
        // fallback: простая логика (заполняем пробелы точками)
        for (auto &row : Map) {
            for (char &c : row) {
                if (c == ' ') c = '.';
            }
        }
        return;
    }

    // Сначала восстановим обычные печеньки и супер-печеньки из OriginalMap
    for (size_t y = 0; y < Map.size() && y < OriginalMap.size(); ++y) {
        for (size_t x = 0; x < Map[y].size() && x < OriginalMap[y].size(); ++x) {
            char orig = OriginalMap[y][x];
            if (orig == '.' || orig == 'o') {
                Map[y][x] = orig;
            } else if (Map[y][x] == ' ') {
                // при желании можно восстановить все пробелы в '.'
                Map[y][x] = Map[y][x]; // оставляем как есть
            }
        }
    }
}

}