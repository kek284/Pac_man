//game_controller.h
#pragma once
#include <vector>
#include <cstdint>
#include "components.h"
#include "movement.h"

namespace Action {

    class Game_Controller {

        int score;
        std::vector<std::vector<char>> Map;
        Components::Pacman Pac_man; 
        std::vector<Components::Ghost> ghosts;

    public:

        Game_Controller(); //конструктор

        void load_ghosts_config(const std::string& filename);
        bool load_map_from_file(const std::string& filename);
        void redraw_map(void);
        void update_map(void);
        void reset_map(void);
        void reset_pacman_position(void);
        void reset_ghost_position(int num_of_ghost);

        bool has_cookie(const Components::Position &atPosition);
        bool is_position_valid(const Components::Position &atPosition);
        
        const std::vector<std::vector<char>>& get_map() const {
            return Map;
        }
        Components::Pacman& get_pacman() {
            return Pac_man;
        }
        bool is_game_over() const;

    };
}