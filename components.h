// components.h - исправленная версия
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "movement.h"

namespace Action {
    class Game_Controller;
}

namespace Components {

    struct Position {
        int X_pos = 0;
        int Y_pos = 0;
        
        Position() : X_pos(0), Y_pos(0) {}
        Position(int x, int y) : X_pos(x), Y_pos(y) {}
    };

    class Objects {
    protected:
        Position current_Pos;
        Position default_Pos;
       
    public:
        Objects() : current_Pos(0, 0), default_Pos(0, 0) {}
        virtual ~Objects() = default;
        
        void set_default_position(const Position& pos) {
            default_Pos = pos;
            current_Pos = pos;
        }
        
        void update_position(const Position& pos);
        void reset_to_default();
        Position get_current_position() const; 
        Position* get_position_ptr();
    };

    class Pacman : public Objects {
        bool super_mode_active = false;
        uint8_t m_life;   
        uint32_t super_timer = 0;
        uint32_t super_duration = 30;
        Action::Movement movement;

    public:
        Pacman() : m_life(3) {}
        explicit Pacman(uint8_t life) : m_life(life) {}
        
        void load_config(const std::string& filename);  
        uint8_t get_life() const;
        void activate_super_mode();
        void update_super_mode();
        bool is_super_mode() const;
        void lose_life();
        Action::Movement& get_movement() { return movement; }
    };

    enum class Direction {
        Left, Right, Up, Down, None
    };

    class Ghost : public Objects {
    private:
        uint32_t speed = 1;
        unsigned int tick_counter = 0;  // Изменили uint на unsigned int
        Direction last_dir = Direction::None;
        Action::Movement movement;
        
    public:
        Ghost() : Objects(), speed(1), tick_counter(0), last_dir(Direction::None) {}
        explicit Ghost(uint8_t s) : Objects(), speed(s), tick_counter(0), last_dir(Direction::None) {}
        
        void load_config_block(const std::vector<std::string>& block);
        uint32_t get_speed() const;
        void init_movement(Action::Game_Controller* game);
        void update_gh_mov(const Position& p_pos, bool pac_super);
        int dist(const Position& a, const Position& b);
        Direction opposite(Direction d);
        Action::Movement& get_movement() { return movement; }
    };
}