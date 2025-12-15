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

    int Pacman::get_life() const {
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

    void Pacman::init_movement(Action::Game_Controller* game) {
        movement = std::make_unique<Action::Movement>(game); // unique_ptr
    }

    bool Pacman::move_left() {
        if (!movement) return false;
        Position new_pos = movement->move_left(current_Pos);
        if (new_pos.X_pos != current_Pos.X_pos || new_pos.Y_pos != current_Pos.Y_pos) {
            current_Pos = new_pos;
            return true;
        }
        return false;
    }

    bool Pacman::move_right() {
        if (!movement) return false;
        Position new_pos = movement->move_right(current_Pos);
        if (new_pos.X_pos != current_Pos.X_pos || new_pos.Y_pos != current_Pos.Y_pos) {
            current_Pos = new_pos;
            return true;
        }
        return false;
    }

    bool Pacman::move_up() {
        if (!movement) return false;
        Position new_pos = movement->move_up(current_Pos);
        if (new_pos.X_pos != current_Pos.X_pos || new_pos.Y_pos != current_Pos.Y_pos) {
            current_Pos = new_pos;
            return true;
        }
        return false;
    }

    bool Pacman::move_down() {
        if (!movement) return false;
        Position new_pos = movement->move_down(current_Pos);
        if (new_pos.X_pos != current_Pos.X_pos || new_pos.Y_pos != current_Pos.Y_pos) {
            current_Pos = new_pos;
            return true;
        }
        return false;
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
        speed = 3; // значение по умолчанию
        for (const auto& line : block) {
            std::istringstream iss(line);
            std::string key;
            if (std::getline(iss, key, '=')) {
                std::string value_str;
                if (std::getline(iss, value_str)) {
                    int value = std::stoi(value_str);
                    if (key == "speed") speed = value;
                }
            }
        }
    }
        
    int Ghost::get_speed() const {
        return speed;
    }

    void Ghost::init_movement(Action::Game_Controller* game) {
        movement = std::make_unique<Action::Movement>(game); // unique_ptr
        if (!game->is_position_valid(current_Pos)) {
            reset_to_default();
        }
    }
    

    void Ghost::update_gh_mov(const Position& pac_pos, bool pac_super) {
        // проверяем, что координаты инициализированы
        if (current_Pos.X_pos < -1000 || current_Pos.X_pos > 1000 || 
            current_Pos.Y_pos < -1000 || current_Pos.Y_pos > 1000) {
            reset_to_default();
            return;
        }
        
        tick_counter++;
        if (speed <= 0) return;
        if (tick_counter % speed != 0) return;
        
        if (!movement) return;
                
        Position new_pos = current_Pos;
        bool moved = false;
        if (behavior_chase) {
        if (!pac_super) {
            // Преследование
            if (pac_pos.X_pos > current_Pos.X_pos) {
                new_pos = movement->move_right(current_Pos);
                if (new_pos.X_pos != current_Pos.X_pos) moved = true;
            }
            if (!moved && pac_pos.X_pos < current_Pos.X_pos) {
                new_pos = movement->move_left(current_Pos);
                if (new_pos.X_pos != current_Pos.X_pos) moved = true;
            }
            if (!moved && pac_pos.Y_pos > current_Pos.Y_pos) {
                new_pos = movement->move_down(current_Pos);
                if (new_pos.Y_pos != current_Pos.Y_pos) moved = true;
            }
            if (!moved && pac_pos.Y_pos < current_Pos.Y_pos) {
                new_pos = movement->move_up(current_Pos);
                if (new_pos.Y_pos != current_Pos.Y_pos) moved = true;
            }
        } else {
            // Убегание 
            if (pac_pos.X_pos > current_Pos.X_pos) {
                new_pos = movement->move_left(current_Pos);
                if (new_pos.X_pos != current_Pos.X_pos) moved = true;
            }
            if (!moved && pac_pos.X_pos < current_Pos.X_pos) {
                new_pos = movement->move_right(current_Pos);
                if (new_pos.X_pos != current_Pos.X_pos) moved = true;
            }
            if (!moved && pac_pos.Y_pos > current_Pos.Y_pos) {
                new_pos = movement->move_up(current_Pos);
                if (new_pos.Y_pos != current_Pos.Y_pos) moved = true;
            }
            if (!moved && pac_pos.Y_pos < current_Pos.Y_pos) {
                new_pos = movement->move_down(current_Pos);
                if (new_pos.Y_pos != current_Pos.Y_pos) moved = true;
            }
        } }
        else { //рандомное движение
            int directions[4] = {0, 1, 2, 3};
            
            for (int i = 0; i < 4; i++) {
                int j = rand() % 4;
                std::swap(directions[i], directions[j]);
            }
            
            for (int i = 0; i < 4 && !moved; i++) {
                switch(directions[i]) {
                    case 0:
                        new_pos = movement->move_left(current_Pos);
                        if (new_pos.X_pos != current_Pos.X_pos) moved = true;
                        break;
                    case 1:
                        new_pos = movement->move_right(current_Pos);
                        if (new_pos.X_pos != current_Pos.X_pos) moved = true;
                        break;
                    case 2:
                        new_pos = movement->move_up(current_Pos);
                        if (new_pos.Y_pos != current_Pos.Y_pos) moved = true;
                        break;
                    case 3:
                        new_pos = movement->move_down(current_Pos);
                        if (new_pos.Y_pos != current_Pos.Y_pos) moved = true;
                        break;
                }
            }
        }
    
        
        if (moved) {
            current_Pos = new_pos;
        }
    }

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
        return abs(a.X_pos - b.X_pos) + abs(a.Y_pos - b.Y_pos);
    } 
}