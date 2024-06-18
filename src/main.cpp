#include <clocale>
#include <ncurses.h>
#include <iostream>
#include <vector>
#include <unistd.h>

#include "snakes_nest.hh"

class SnakeGame
{
protected:
    int max_width;
    int max_height;

    int delay = 100000;

    Direction direction = Left;

    Plane *plane;
    Snake *snake;
    Mouse *mouse;

public:
    SnakeGame()
    {
        initscr();
        nodelay(stdscr, true);
        keypad(stdscr, true);
        noecho();
        curs_set(0);
        srand(time(NULL));

        max_width = getmaxx(stdscr) / 2;
        max_height = getmaxy(stdscr) / 1.8;

        plane = new Plane(max_width, max_height);

        snake = new Snake(plane->get_size());

        mouse = new Mouse();
        mouse->insert(plane->get_size(), &snake->body);

        plane->draw();
        mouse->draw();
    }

    void update()
    {
        int tmp = getch();

        switch (tmp)
        {
        case KEY_LEFT:
            if (direction != Right)
            {
                direction = Left;
            }
            break;
        case KEY_UP:
            if (direction != Down)
            {
                direction = Up;
            }
            break;
        case KEY_DOWN:
            if (direction != Up)
            {
                direction = Down;
            }
            break;
        case KEY_RIGHT:
            if (direction != Left)
            {
                direction = Right;
            }
            break;
        case QUIT_KEY:
            __quit__();
        }

        snake->draw(direction);
    }

    void start_game()
    {
        while (true)
        {
            update();
            usleep(delay);
        }
    }

    ~SnakeGame()
    {
        nodelay(stdscr, false);
        getch();
        endwin();
    }
};

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "");

    SnakeGame s;
    s.start_game();
    return 0;
}
