//movement.h
// movement.h
#pragma once
#include <cstdint>

namespace Components {
    struct Position;
}

namespace Action { 
    class Game_Controller; 
}

namespace Action {
    class Movement {
        Game_Controller* m_game = nullptr;
    public:
        Movement() = default;
        Movement(Game_Controller* game) : m_game(game) {}
        
        void set_game(Game_Controller* game);
        
        Components::Position move_left(const Components::Position& current_pos);
        Components::Position move_right(const Components::Position& current_pos);
        Components::Position move_up(const Components::Position& current_pos);
        Components::Position move_down(const Components::Position& current_pos);
        
        bool is_movable(const Components::Position& pos) const;
    };
}