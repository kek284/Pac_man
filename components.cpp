//cpmponents.cpp
#include "components.h"
#include "game_controller.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <climits>
#include <cstdlib>

namespace Components {

    void Objects::update_position(const Position& pos) {
        current_Pos = pos; 
    }

    void Objects::reset_to_default() {
        current_Pos = default_Pos;
    }
    Position Objects::get_current_position() const {
        return current_Pos;
    }
    Position* Objects::get_position_ptr() {
        return &current_Pos;
    }

        
    uint8_t Pacman::get_life() const {
        return m_life;
    }

    void Pacman::load_config(const std::string& filename) {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Pacman config not found: " << filename << "\n";
                return;
            }

            std::string line;
            while (std::getline(file, line)) {
                if (line.empty()) continue;

                std::istringstream ss(line);

                std::string key = line.substr(0, line.find('='));
                std::string value = line.substr(line.find('=') + 1);
                
                if (key == "life") m_life = std::stoi(value);
                if (key == "super_duration") super_duration = std::stoi(value);
            }
    }

    void Pacman::activate_super_mode() {
        super_mode_active = true;
        super_timer = super_duration;
    }

    void Pacman::update_super_mode() {
        if (super_mode_active && super_timer > 0) {
            --super_timer;
            if (super_timer == 0)
                super_mode_active = false;
        }
    }
    
    bool Pacman::is_super_mode() const {
        return super_mode_active;
    }
    
    void Pacman::lose_life() {
        if (m_life > 0)
            --m_life;
    }

    void Ghost::load_config_block(const std::vector<std::string>& block) {
        speed = 3;
        for (const auto& line : block) {
            std::istringstream iss(line);
            std::string key;
            if (std::getline(iss, key, '=')) {
                std::string value_str;
                if (std::getline(iss, value_str)) {
                    int value = std::stoi(value_str);
                    if (key.find("speed") != std::string::npos) speed = value;
                    else if (key.find("start_x") != std::string::npos) {
                        current_Pos.X_pos = value;
                        default_Pos.X_pos = value;
                    }
                    else if (key.find("start_y") != std::string::npos) {
                        current_Pos.Y_pos = value;
                        default_Pos.Y_pos = value;
                    }                
                }
            }
        }   
    }
        
    uint32_t Ghost::get_speed() const {
            return speed;
    }

    void Ghost::init_movement(Action::Game_Controller* game) {
        movement.set_game(game);
        movement.set_position(&current_Pos);
        std::cout << "Ghost init pos: " << current_Pos.X_pos << "," << current_Pos.Y_pos << "\n";
        if (!game->is_position_valid(current_Pos)) {
            std::cout << "Warning: ghost starting position is invalid!\n";
        }
    }
    

    void Ghost::update_gh_mov(const Position& pac_pos, bool pac_super) {
        tick_counter++;
        if (speed <= 0) return;
        if (tick_counter % speed != 0) return;
        
        std::cout << "Ghost attempting to move from ("
                  << current_Pos.X_pos << "," << current_Pos.Y_pos << ")\n";
        
        // Сохраняем исходную позицию
        Position original_pos = current_Pos;
        bool moved = false;
        
        // Пробуем двигаться в направлении Pacman (или от него)
        if (!pac_super) {
            // Преследование
            if (pac_pos.X_pos > current_Pos.X_pos) {
                moved = movement.move_right();
                if (moved) std::cout << "  -> moved right\n";
            }
            if (!moved && pac_pos.X_pos < current_Pos.X_pos) {
                moved = movement.move_left();
                if (moved) std::cout << "  -> moved left\n";
            }
            if (!moved && pac_pos.Y_pos > current_Pos.Y_pos) {
                moved = movement.move_down();
                if (moved) std::cout << "  -> moved down\n";
            }
            if (!moved && pac_pos.Y_pos < current_Pos.Y_pos) {
                moved = movement.move_up();
                if (moved) std::cout << "  -> moved up\n";
            }
        } else {
            // Убегание (обратная логика)
            if (pac_pos.X_pos > current_Pos.X_pos) {
                moved = movement.move_left();
                if (moved) std::cout << "  -> moved left (fleeing)\n";
            }
            if (!moved && pac_pos.X_pos < current_Pos.X_pos) {
                moved = movement.move_right();
                if (moved) std::cout << "  -> moved right (fleeing)\n";
            }
            if (!moved && pac_pos.Y_pos > current_Pos.Y_pos) {
                moved = movement.move_up();
                if (moved) std::cout << "  -> moved up (fleeing)\n";
            }
            if (!moved && pac_pos.Y_pos < current_Pos.Y_pos) {
                moved = movement.move_down();
                if (moved) std::cout << "  -> moved down (fleeing)\n";
            }
        }
        
        // Если не удалось двигаться к цели, пробуем случайные направления
        if (!moved) {
            std::cout << "  -> trying random directions\n";
            // Пробуем все 4 направления в случайном порядке
            int directions[4] = {0, 1, 2, 3};
            
            // Перемешиваем массив направлений
            for (int i = 0; i < 4; i++) {
                int j = rand() % 4;
                std::swap(directions[i], directions[j]);
            }
            
            for (int i = 0; i < 4 && !moved; i++) {
                // Восстанавливаем исходную позицию перед каждой попыткой
                current_Pos = original_pos;
                
                switch(directions[i]) {
                    case 0:
                        moved = movement.move_left();
                        if (moved) std::cout << "  -> random left worked\n";
                        break;
                    case 1:
                        moved = movement.move_right();
                        if (moved) std::cout << "  -> random right worked\n";
                        break;
                    case 2:
                        moved = movement.move_up();
                        if (moved) std::cout << "  -> random up worked\n";
                        break;
                    case 3:
                        moved = movement.move_down();
                        if (moved) std::cout << "  -> random down worked\n";
                        break;
                }
            }
        }
        
        // Если так и не смогли двинуться
        if (!moved) {
            current_Pos = original_pos;
            std::cout << "  -> cannot move, staying put\n";
        } else {
            std::cout << "Ghost moved to (" 
                      << current_Pos.X_pos << "," << current_Pos.Y_pos << ")\n";
        }
    }


    //Чтобы призрак не ходил обратно то есть назад
    Direction Ghost::opposite(Direction d) {
        switch (d) {
            case Direction::Left:  return Direction::Right;
            case Direction::Right: return Direction::Left;
            case Direction::Up:    return Direction::Down;
            case Direction::Down:  return Direction::Up;
            default: return Direction::None;
        }
    }
    
    int Ghost::dist(const Position& a, const Position& b) {
        return abs(a.X_pos - b.X_pos) + abs(a.Y_pos - b.Y_pos);  // Убрали приведение типов
    } 
}