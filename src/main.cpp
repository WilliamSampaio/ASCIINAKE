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

    std::vector<UnitType> snake;

    int delay = 100000;

    Direction direction = Left;
    bool tail_stop = false;

    Plane *plane;
    Food *food;

public:
    SnakeGame()
    {
        initscr();
        nodelay(stdscr, true);
        keypad(stdscr, true);
        noecho();
        curs_set(0);

        max_width = getmaxx(stdscr) / 2;
        max_height = getmaxy(stdscr) / 1.8;

        plane = new Plane(max_width, max_height);
        food = new Food();

        food->insert(plane->get_size(), &snake);

        for (int i = 0; i < 3; ++i)
        {
            snake.push_back(UnitType((max_width / 2) - i, max_height / 2));
        }

        for (int i = 0; i < ((int)snake.size()); i++)
        {
            mvprintw(snake[0].pos_y, snake[0].pos_x, SNAKE_CHAR);
        }

        plane->draw();
        food->draw();

        refresh();
    }

    ~SnakeGame()
    {
        nodelay(stdscr, false);
        getch();
        endwin();
    }

    void move_snake()
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

        if (!tail_stop)
        {
            move(snake[snake.size() - 1].pos_y, snake[snake.size() - 1].pos_x);
            printw(" ");
            refresh();
            snake.pop_back();
        }

        if (direction == Left)
        {
            snake.insert(snake.begin(), UnitType(snake[0].pos_x - 1, snake[0].pos_y));
        }

        if (direction == Right)
        {
            snake.insert(snake.begin(), UnitType(snake[0].pos_x + 1, snake[0].pos_y));
        }

        if (direction == Up)
        {
            snake.insert(snake.begin(), UnitType(snake[0].pos_x, snake[0].pos_y - 1));
        }

        if (direction == Down)
        {
            snake.insert(snake.begin(), UnitType(snake[0].pos_x, snake[0].pos_y + 1));
        }

        mvprintw(snake[0].pos_y, snake[0].pos_x, SNAKE_CHAR);
        refresh();
    }

    void start_game()
    {
        while (true)
        {
            move_snake();
            usleep(delay);
        }
    }
};

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "");

    SnakeGame s;
    s.start_game();
    return 0;
}
