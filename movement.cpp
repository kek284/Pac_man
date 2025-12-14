//movement.cpp
#include "movement.h"
#include "game_controller.h"
#include <iostream>
#include "components.h"

namespace Action {
    
    void Movement::set_game(Action::Game_Controller* game) {
        m_game = game;
    }
    
    Components::Position Movement::move_left(const Components::Position& current_pos) {
        if (!m_game) {
            std::cout << "        move_left: ERROR - no game controller\n";
            return current_pos;
        }
        
        Components::Position new_pos = current_pos;
        new_pos.X_pos -= 1;
        
        if (new_pos.X_pos < 0) {
            std::cout << "        move_left: at left edge\n";
            return current_pos;
        }
        
        if (m_game->is_position_valid(new_pos)) {
            return new_pos;
        } else {
            return current_pos;
        }
    }
    
    Components::Position Movement::move_right(const Components::Position& current_pos) {
        if (!m_game) {
            std::cout << "        move_right: ERROR - no game controller\n";
            return current_pos;
        }
        
        Components::Position new_pos = current_pos;
        new_pos.X_pos += 1;
        
        if (m_game->is_position_valid(new_pos)) {
            return new_pos;
        } else {
            return current_pos;
        }
    }
    
    Components::Position Movement::move_up(const Components::Position& current_pos) {
        if (!m_game) {
            std::cout << "        move_up: ERROR - no game controller\n";
            return current_pos;
        }
        
        Components::Position new_pos = current_pos;
        new_pos.Y_pos -= 1;
        
        if (new_pos.Y_pos < 0) {
            std::cout << "        move_up: at top edge\n";
            return current_pos;
        }
        
        if (m_game->is_position_valid(new_pos)) {
            return new_pos;
        } else {
            return current_pos;
        }
    }
    
    Components::Position Movement::move_down(const Components::Position& current_pos) {
        if (!m_game) {
            std::cout << "        move_down: ERROR - no game controller\n";
            return current_pos;
        }
        
        Components::Position new_pos = current_pos;
        new_pos.Y_pos += 1;
        
        if (m_game->is_position_valid(new_pos)) {
            return new_pos;
        } else {
            return current_pos;
        }
    }
    
    bool Movement::is_movable(const Components::Position& pos) const {
        if (!m_game) return false;
        return m_game->is_position_valid(pos);
    }
}