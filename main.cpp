//main.cpp
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include "movement.h"
#include "game_controller.h"

char getch() {
    termios oldt{}, newt{};
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    char c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return c;
}

int main() {
    Action::Game_Controller game;

    if (!game.load_map_from_file("configs/map.txt"))
        return 1;

    while (!game.is_game_over()) {
        game.redraw_map();

        char c = getch();
        auto& mov = game.get_pacman().get_movement();  // теперь getter

        
        if (c == 'a') mov.move_left();
        if (c == 'd') mov.move_right();
        if (c == 'w') mov.move_up();
        if (c == 's') mov.move_down();
        
        if (c == 'q') break;

        game.update_map();
        usleep(100000); // 100 ms
    }

    std::cout << "GAME OVER\n";
}
