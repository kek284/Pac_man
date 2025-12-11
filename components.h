//components.h
#pragma once
#include <cstdint>

namespace Action { class Movement; }
namespace Components {

    struct Position {
        uint32_t X_pos;
        uint32_t Y_pos;
    };

    class Objects{

    protected:

        Position current_Pos;
        Action::Movement* movement = nullptr;

    public:

        void set_movement(Action::Movement* mov);
        void update_position(const Position pos);
        Position get_current_position() const; 
        Position* get_position_ptr();
    };

    class Pacman : public Objects {
        bool super_mode_active = false;
        uint32_t super_timer = 0; 
        uint8_t m_life;   
        uint32_t super_duration; 

    public:
        Pacman() = default;// jjjjjj
        explicit Pacman(uint8_t life) : m_life(life) {} ///jjjjj
        void load_config(const std::string& filename);  
        uint8_t get_life() const;
    };

    class Ghost : public Objects  {
    private:
        uint32_t speed;   
    public:
        Ghost() = default;
        explicit Ghost(uint8_t s) : speed(s) {}  
        void load_config_block(const std::vector<std::string>& block);
        void load_config(const std::string& filename);  

        uint32_t get_speed() const;
    };

   
}