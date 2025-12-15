#include <iostream>
#include <termios.h>
#include <clocale>
#include <unistd.h>
#include <sys/select.h>
#include "game_controller.h"

// Неблокирующее чтение с таймаутом в миллисекундах
int getch_timeout(int timeout_ms) {
    fd_set set;
    struct timeval timeout;

    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);

    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    struct termios oldt, newt;
    if (tcgetattr(STDIN_FILENO, &oldt) == -1) {
        return 0;
    }
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 0;
    newt.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) == -1) {
        return 0;
    }

    int rv = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);
    int c = 0;
    if (rv > 0 && FD_ISSET(STDIN_FILENO, &set)) {
        c = getchar();
    }

    // восстанавливаем старые настройки терминала
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return c;
}
int main() {
    srand(time(nullptr)); // Для случайных движений призраков
    std::setlocale(LC_ALL, ""); 
    std::cout << "\x1B[?1049h"; // включить альтернативный экран
    std::cout.flush();
    srand(time(nullptr));

    Action::Game_Controller game;

    if (!game.load_map_from_file("configs/map.txt"))
        return 1;

    const int tick_ms = 100; // базовый тик интервал (каждые 100 ms)
    while (!game.is_game_over() && !game.is_game_won()) {
        game.redraw_map();

        int c = getch_timeout(tick_ms);
        auto& pacman = game.get_pacman();
        
        if (c == 'a') pacman.move_left();
        if (c == 'd') pacman.move_right();
        if (c == 'w') pacman.move_up();
        if (c == 's') pacman.move_down();
        
        if (c == 'q') break;

        // обновляем состояние связанное с Pacman (печеньки и пр.)
        game.update_map();

        // обновляем призраков независимо (вызов каждый tick_ms)
        game.update_ghosts();

        // небольшой сон, чтобы снизить загрузку CPU (необязательно)
        usleep(1000 * 10);
    }
    if (game.is_game_won()) {
        std::cout << "YOU WIN\n";
    } else {
        std::cout << "GAME OVER\n";

    }
     std::cout << "\x1B[?1049l"; // выключить альтернативный экран (вернуть скроллбэк)
    std::cout.flush();
    return 0;
}