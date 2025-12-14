//movement.cpp
#include"movement.h"
#include"game_controller.h"
#include<iostream>
#include"components.h"

namespace Action {
    
    void Movement::set_game(Action::Game_Controller* game) {
        m_game = game;
    }

    void Movement::set_position(Components::Position* pos) {
        m_pos = pos;
    }

    bool Movement::move_left() {
        if (!m_game || !m_pos) {
            std::cout << "        move_left: ERROR - no game controller or position\n";
            return false;
        }
    
        Components::Position new_pos = *m_pos;
        std::cout << "        move_left: from (" << new_pos.X_pos << "," << new_pos.Y_pos << ") ";
        
        // Для int проверка на отрицательные значения вместо 0
        new_pos.X_pos -= 1;
        
        if (new_pos.X_pos < 0) {  // Изменили проверку
            std::cout << "FAIL - at left edge (X < 0)\n";
            return false;
        }
        
        std::cout << "to (" << new_pos.X_pos << "," << new_pos.Y_pos << ") ";
        
        if (m_game->is_position_valid(new_pos)) {
            *m_pos = new_pos;
            std::cout << "SUCCESS\n";
            return true;
        }
        
        std::cout << "FAIL - invalid position\n";
        return false;
    }
    
    bool Movement::move_right() {
        if (!m_game || !m_pos) {
            std::cout << "        move_right: ERROR - no game controller or position\n";
            return false;
        }
    
        Components::Position new_pos = *m_pos;
        std::cout << "        move_right: from (" << new_pos.X_pos << "," << new_pos.Y_pos << ") ";
        
        new_pos.X_pos += 1;
        std::cout << "to (" << new_pos.X_pos << "," << new_pos.Y_pos << ") ";
        
        if (m_game->is_position_valid(new_pos)) {
            *m_pos = new_pos;
            std::cout << "SUCCESS\n";
            return true;
        }
        
        std::cout << "FAIL - invalid position\n";
        return false;
    }
    
    bool Movement::move_up() {
        if (!m_game || !m_pos) {
            std::cout << "        move_up: ERROR - no game controller or position\n";
            return false;
        }
    
        Components::Position new_pos = *m_pos;
        std::cout << "        move_up: from (" << new_pos.X_pos << "," << new_pos.Y_pos << ") ";
        
        new_pos.Y_pos -= 1;
        
        if (new_pos.Y_pos < 0) {  // Изменили проверку
            std::cout << "FAIL - at top edge (Y < 0)\n";
            return false;
        }
        
        std::cout << "to (" << new_pos.X_pos << "," << new_pos.Y_pos << ") ";
        
        if (m_game->is_position_valid(new_pos)) {
            *m_pos = new_pos;
            std::cout << "SUCCESS\n";
            return true;
        }
        
        std::cout << "FAIL - invalid position\n";
        return false;
    }
    
    bool Movement::move_down() {
        if (!m_game || !m_pos) {
            std::cout << "        move_down: ERROR - no game controller or position\n";
            return false;
        }
    
        Components::Position new_pos = *m_pos;
        std::cout << "        move_down: from (" << new_pos.X_pos << "," << new_pos.Y_pos << ") ";
        
        new_pos.Y_pos += 1;
        std::cout << "to (" << new_pos.X_pos << "," << new_pos.Y_pos << ") ";
        
        if (m_game->is_position_valid(new_pos)) {
            *m_pos = new_pos;
            std::cout << "SUCCESS\n";
            return true;
        }
        
        std::cout << "FAIL - invalid position\n";
        return false;
    }

    bool Movement::is_movable(const Components::Position& pos) const {
        if (!m_game) return false;
        return m_game->is_position_valid(pos);
    }
}