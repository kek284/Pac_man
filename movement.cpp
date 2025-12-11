//movement.cpp
#include"movement.h"
#include"game_controller.h"
#include"components.h"

namespace Action {
    
    void Movement::set_game(Action::Game_Controller* game) {
        m_game = game;
    }

    void Movement::set_position(Components::Position* pos) {
        m_pos = pos;
    }

    bool Movement::move_left() {
        if (!m_game || !m_pos) return false;

        Components::Position new_pos = *m_pos;
        if (new_pos.X_pos == 0) return false
        new_pos.X_pos -= 1;

        if (m_game->is_position_valid(new_pos)) {
            *m_pos = new_pos;
            return true;
        }
        return false;
    }
    bool Movement::move_right() {
        if (!m_game || !m_pos) return false;
        Components::Position new_pos = *m_pos;
        
        new_pos.X_pos += 1;

        if (m_game->is_position_valid(new_pos)) {
            *m_pos = new_pos;
            return true;
        }
        return false;
    }
    bool Movement::move_up() {
        if (!m_game || !m_pos) return false;

        Components::Position new_pos = *m_pos;
        if (new_pos.Y_pos == 0) return false

        new_pos.Y_pos -= 1;

        if (m_game->is_position_valid(new_pos)) {
            *m_pos = new_pos;
            return true;
        }
        return false;
    }

    bool Movement::move_down() {
        if (!m_game || !m_pos) return false;

        Components::Position new_pos = *m_pos;
        new_pos.Y_pos += 1;

        if (m_game->is_position_valid(new_pos)) {
            *m_pos = new_pos;
            return true;
        }
        return false;
    }

    bool Movement::is_movable(const Components::Position& pos) const {
        if (!m_game) return false;
        return m_game->is_position_valid(pos);
    }
}