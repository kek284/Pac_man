//game_controller.h
//сделать разную логику призраков, что то не так со сьедением,
// мб разные цвета, уровни, увеличение скорости призраков при супер режиме и при разных уровнях, три уровня и победа
#pragma once
#include <vector>
#include <cstdint>
#include "components.h"
#include "movement.h"

namespace Action {

    class Game_Controller {

        int score;
        std::vector<std::vector<char>> Map;
        std::vector<std::vector<char>> OriginalMap; // сохранённая исходная карта для восстановления
        Components::Pacman Pac_man; 
        std::vector<Components::Ghost> ghosts;

        int level = 1;
        const int max_levels = 3;
        int base_ghost_speed = 3; // ticks per move at level 1
        bool game_won = false;

    public:

        Game_Controller(); //конструктор

        void load_ghosts_config(const std::string& filename);
        bool load_map_from_file(const std::string& filename);
        void redraw_map(void);
        void update_map(void); //обнновление состояние связанного с пакманом
        void update_ghosts(void);
        void reset_map(void);
        void reset_pacman_position(void);
        void reset_ghost_position(int num_of_ghost);
        void refill_cookies(void);


        bool has_cookie(const Components::Position &atPosition);
        bool is_position_valid(const Components::Position &atPosition);
        
        void next_level();
        void increase_ghost_speed();

        const std::vector<std::vector<char>>& get_map() const {
            return Map;
        }
        Components::Pacman& get_pacman() {
            return Pac_man;
        }
        bool is_game_over() const;
        bool is_game_won() const;
        void check_collisions();

    };
}