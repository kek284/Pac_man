#include <iostream>
#include <termios.h>
#include <unistd.h>
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
    srand(time(nullptr)); // Для случайных движений призраков
    
    Action::Game_Controller game;

    if (!game.load_map_from_file("configs/map.txt"))
        return 1;

    while (!game.is_game_over()) {
        game.redraw_map();

        char c = getch();
        auto& pacman = game.get_pacman();
        
        if (c == 'a') pacman.move_left();
        if (c == 'd') pacman.move_right();
        if (c == 'w') pacman.move_up();
        if (c == 's') pacman.move_down();
        
        if (c == 'q') break;

        game.update_map();
        usleep(100000); // 100 ms
    }

    std::cout << "GAME OVER\n";
    return 0;
}