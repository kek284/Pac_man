#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <memory>

// Предварительное объявление
namespace Action {
    class Movement;
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
        int m_life;   
        int super_timer = 0;
        int super_duration = 30;
        std::unique_ptr<Action::Movement> movement;

    public:
        Pacman() : m_life(3), movement(nullptr) {}
        explicit Pacman(int life) : m_life(life), movement(nullptr) {}
        ~Pacman() = default;

        // Явные перемещающие операции (чтобы vector мог noexcept-перемещать объекты)
        Pacman(Pacman&&) noexcept = default;
        Pacman& operator=(Pacman&&) noexcept = default;

        // копирование по-прежнему запрещено (из-за unique_ptr)
        Pacman(const Pacman&) = delete;
        Pacman& operator=(const Pacman&) = delete;
        
        void load_config(const std::string& filename);  
        int get_life() const;
        void activate_super_mode();
        void update_super_mode();
        bool is_super_mode() const;
        void lose_life();
        
        // Новые методы движения
        bool move_left();
        bool move_right();
        bool move_up();
        bool move_down();
        
        void init_movement(Action::Game_Controller* game);
        Action::Movement* get_movement() { return movement.get(); }
    };

    enum class Direction {
        Left, Right, Up, Down, None
    };

    class Ghost : public Objects {
    private:
        int speed = 1;
        unsigned int tick_counter = 0; 
        Direction last_dir = Direction::None;
        std::unique_ptr<Action::Movement> movement;
        
    public:
        Ghost() : Objects(), speed(1), tick_counter(0), last_dir(Direction::None), movement(nullptr) {
            current_Pos = {0, 0};
            default_Pos = {0, 0};
        }
        
        explicit Ghost(int s) : Objects(), speed(s), tick_counter(0), last_dir(Direction::None), movement(nullptr) {
            current_Pos = {0, 0};
            default_Pos = {0, 0};
        }
        
        ~Ghost() = default;

        // Явные перемещающие операции (для noexcept-перемещения)
        Ghost(Ghost&&) noexcept = default;
        Ghost& operator=(Ghost&&) noexcept = default;

        // копирование запрещено
        Ghost(const Ghost&) = delete;
        Ghost& operator=(const Ghost&) = delete;
        
        void load_config_block(const std::vector<std::string>& block);
        int get_speed() const;
        void init_movement(Action::Game_Controller* game);
        void update_gh_mov(const Position& p_pos, bool pac_super);
        int dist(const Position& a, const Position& b);
        Direction opposite(Direction d);
        Action::Movement* get_movement() { return movement.get(); }
        
        
    };
}