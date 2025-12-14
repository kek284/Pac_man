//movement.h
#pragma once
#include <cstdint>


namespace Action { class Game_Controller; }
namespace Components { struct Position; }

namespace Action {

    class Movement {

        Action::Game_Controller* m_game = nullptr; //доступ к карте и проверкам
        Components::Position* m_pos = nullptr;

        public:
            Movement() = default;
            Movement(Game_Controller* game, Components::Position* pos) : m_game(game), m_pos(pos) {}
            
            void set_game(Game_Controller* game);
            void set_position(Components::Position* pos);

            bool move_left();
            bool move_right();
            bool move_up();
            bool move_down();

            bool is_movable(const Components::Position& pos) const;
    };
}