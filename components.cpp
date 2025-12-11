//cpmponents.cpp
#include "components.h"
#include "movement.h"
#include <fstream>
#include <sstream>
#include <iostream>
namespace Components {

        void Objects::set_movement(Action::Movement* mov) {
            movement = mov;
        }
        void Objects::update_position(Position pos) {
            current_Pos = pos; 
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
        #include "components.h"
#include "movement.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace Components {

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
                std::string key, eq;
                ss >> key >> eq;

                if (eq != "=") continue; // защита

                if (key == "life") {
                    ss >> m_life;
                }
                else if (key == "start_x") {
                    ss >> current_Pos.X_pos;
                }
                else if (key == "start_y") {
                    ss >> current_Pos.Y_pos;
                }
            }
    }

    void Ghost::load_config_block(const std::vector<std::string>& block) {
        for (const auto& line : block) {
            std::istringstream iss(line);
            std::string key;
            if (std::getline(iss, key, '=')) {
                std::string value_str;
                if (std::getline(iss, value_str)) {
                    int value = std::stoi(value_str);
                    if (key.find("speed") != std::string::npos) speed = value;
                    else if (key.find("start_x") != std::string::npos) current_Pos.X_pos = value;
                    else if (key.find("start_y") != std::string::npos) current_Pos.Y_pos = value;
                }
            }
        }
    }
    
    

    void Game_Controller::load_ghosts_config(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return;
    
        std::vector<std::string> block;
        std::string line;
    
        while (std::getline(file, line)) {
            if (line.find("ghost:") != std::string::npos) {
                if (!block.empty()) { // предыдущий блок
                    Components::Ghost g;
                    g.load_config_block(block);
                    ghosts.push_back(g);
                    block.clear();
                }
            } else if (!line.empty()) {
                block.push_back(line);
            }
        }
        if (!block.empty()) { // последний блок
            Components::Ghost g;
            g.load_config_block(block);
            ghosts.push_back(g);
        }
    }
    


        }

        uint8_t Ghost::get_speed() const {
                return speed;
        }
       
    
    
}