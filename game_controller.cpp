#include "game_controller.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

namespace Action {

// Конструктор
Game_Controller::Game_Controller() : score(0) {
    // Загружаем конфиг Pacman
    Pac_man.load_config("configs/pacman.cfg");

  //load_ghosts_config("configs/ghost.cfg");

    // Инициализируем движение Pacman
    Pac_man.get_movement().set_game(this);
    Pac_man.get_movement().set_position(Pac_man.get_position_ptr());
    
    // Инициализируем движение призраков
    for (auto& ghost : ghosts) {
        ghost.init_movement(this);
    }
}

// Возвращаем ссылку на Movement Pacman
Movement& Game_Controller::pacman_movement() {
    return Pac_man.get_movement();
}


// Сбрасываем позицию Pacman
void Game_Controller::reset_pacman_position() {
    if (!Map.empty()) {
        Pac_man.reset_to_default();
        Pac_man.get_movement().set_position(Pac_man.get_position_ptr());
    }
}

// Проверяем, закончена ли игра
bool Game_Controller::is_game_over() const {
    return Pac_man.get_life() == 0;
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
                g.init_movement(this);
                ghosts.push_back(g);
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
        ghosts.push_back(g);
    }
}

// Загружаем карту из файла
// game_controller.cpp - исправленный метод load_map_from_file
bool Game_Controller::load_map_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Map not found: " << filename << "\n";
        return false;
    }

    Map.clear();
    ghosts.clear();

    std::string line;
    int y = 0;

    while (std::getline(file, line)) {
        std::vector<char> row;

        // Используем int для x, но с проверкой
        for (int x = 0; x < static_cast<int>(line.size()); ++x) {
            char c = line[static_cast<size_t>(x)];

            if (c == 'P') {
                Pac_man.set_default_position({x, y});
                Pac_man.get_movement().set_position(Pac_man.get_position_ptr());
                row.push_back(' ');
            } else if (c == 'G') {
                Components::Ghost g;
                g.set_default_position({x, y});
                g.init_movement(this);
                ghosts.push_back(g);
                row.push_back(' ');
            } else {
                row.push_back(c);
            }
        }

        Map.push_back(row);
        y++;
    }

    std::cout << "Map loaded successfully. Size: " 
              << (Map.empty() ? 0 : Map[0].size()) << "x" << Map.size() << "\n";
    return true;
}

// Загружаем карту из вектора
void Game_Controller::load_map(const std::vector<std::vector<char>>& Value) {
    Map = Value;
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

    // Очистка экрана и вывод
    std::cout << "\x1B[2J\x1B[H";
    std::cout << "Score: " << score << "\n\n";

    for (const auto& row : Map_screen) {
        for (char c : row) std::cout << c;
        std::cout << "\n";
    }
}

// В update_map добавить проверки перед доступом к Map:



// Обновление состояния карты
void Game_Controller::update_map() {
    if (Map.empty()) return;

    Components::Position p_pos = Pac_man.get_current_position();
    bool pac_super = Pac_man.is_super_mode();

    std::cout << "Updating ghosts...\n";
    for (size_t i = 0; i < ghosts.size(); ++i) {
        std::cout << "Ghost " << i << " at: (" 
                  << ghosts[i].get_current_position().X_pos << ","
                  << ghosts[i].get_current_position().Y_pos << ")\n";
        ghosts[i].update_gh_mov(p_pos, pac_super);
    }

    if (Map[p_pos.Y_pos][p_pos.X_pos] == '.') {
        score += 200;
        Map[p_pos.Y_pos][p_pos.X_pos] = ' ';
    }

    if (Map[p_pos.Y_pos][p_pos.X_pos] == 'o') {
        Map[p_pos.Y_pos][p_pos.X_pos] = ' ';
        score += 1000;
        Pac_man.activate_super_mode();
    }

    for (size_t i = 0; i < ghosts.size(); ++i) {
        Components::Position gh_pos = ghosts[i].get_current_position();
        if (p_pos.Y_pos == gh_pos.Y_pos && p_pos.X_pos == gh_pos.X_pos) {
            if (Pac_man.is_super_mode()) {
                score += 500;
                reset_ghost_position(i);
            } else {
                Pac_man.lose_life();
                reset_pacman_position();
                reset_ghost_position(i);
            }
        }
    }

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
    if (!has_cookies) {
        std::cout << "YOU WIN!\n";
    }

    Pac_man.update_super_mode();
}

// Сброс карты (например, очки)
void Game_Controller::reset_map() {
    score = 0;
}

// Сброс позиции конкретного призрака
void Game_Controller::reset_ghost_position(uint8_t num_of_ghost) {
    if (!Map.empty() && num_of_ghost < ghosts.size()) {
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


// Проверка валидности позиции
bool Game_Controller::is_position_valid(const Components::Position &atPosition) {
    if (Map.empty()) {
        std::cout << "        is_position_valid: Map is empty\n";
        return false;
    }
    if (atPosition.Y_pos < 0 || atPosition.Y_pos >= (int)Map.size()) {  // Добавили проверку на < 0
        std::cout << "        is_position_valid: Y out of bounds (" 
                  << atPosition.Y_pos << " not in [0, " << Map.size() << "))\n";
        return false;
    }
    if (atPosition.X_pos < 0 || atPosition.X_pos >= (int)Map[atPosition.Y_pos].size()) {  // Добавили проверку на < 0
        std::cout << "        is_position_valid: X out of bounds (" 
                  << atPosition.X_pos << " not in [0, " << Map[atPosition.Y_pos].size() << "))\n";
        return false;
    }
    
    char c = Map[atPosition.Y_pos][atPosition.X_pos];
    bool valid = (c != '#');
    
    std::cout << "        is_position_valid: (" << atPosition.X_pos << "," << atPosition.Y_pos 
              << ") = '" << c << "' -> " << (valid ? "VALID" : "INVALID (wall)") << "\n";
    
    return valid;
}

} // namespace Action
