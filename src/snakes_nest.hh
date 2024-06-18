#ifndef SNAKES_NEST_H
#define SNAKES_NEST_H

#ifdef _WIN32
#define CLEAR "cls"
#include <windows.h>
#elif linux
#define CLEAR "clear"
#include <unistd.h>
#endif

#define QUIT_KEY 27
#define SNAKE_CHAR "\u25CF" // ●
#define FOOD_CHAR "\u25EF"  // ◯
#define DELAY_DEFAULT 100000

#include <iostream>
#include <ncurses.h>
#include <vector>

enum Direction
{
    Left,
    Up,
    Right,
    Down
};

struct PlaneSize
{
    int width, height;
    PlaneSize(int _width = 10, int _height = 10)
    {
        width = _width;
        height = _height;
    }
};

struct UnitType
{
    int pos_x, pos_y;
    UnitType(int _pos_x = 0, int _pos_y = 0)
    {
        pos_x = _pos_x;
        pos_y = _pos_y;
    }
    bool collision(UnitType _unit)
    {
        if (_unit.pos_x == pos_x && _unit.pos_y == pos_y)
            return true;
        return false;
    }
};

class Plane
{
protected:
    PlaneSize size;

public:
    Plane(int _width, int _height)
    {
        size.width = _width;
        size.height = _height;
    }

    PlaneSize get_size()
    {
        return size;
    }

    void draw()
    {
        mvprintw(0, 0, "🮐");
        mvprintw(0, size.width - 1, "🮐");
        mvprintw(size.height - 1, 0, "🮐");
        mvprintw(size.height - 1, size.width - 1, "🮐");

        for (int i = 1; i < size.width - 1; i++)
        {
            mvprintw(0, i, "🮐");
            mvprintw(size.height - 1, i, "🮐");
        }

        for (int i = 1; i < size.height - 1; i++)
        {
            mvprintw(i, 0, "🮐");
            mvprintw(i, size.width - 1, "🮐");
        }
    }
};

class Mouse
{
public:
    UnitType mouse;

    void insert(PlaneSize _plane, std::vector<UnitType> *_snake_body)
    {
        while (true)
        {
            int tmpx = rand() % _plane.width;
            if (tmpx == 0)
                tmpx++;
            if (tmpx == _plane.width - 2)
                tmpx--;

            int tmpy = rand() % _plane.height;
            if (tmpy == 0)
                tmpy++;
            if (tmpx == _plane.height - 2)
                tmpx--;

            for (int i = 0; i < ((int)_snake_body->size()); ++i)
            {
                if (_snake_body->at(i).pos_x == tmpx && _snake_body->at(i).pos_y == tmpy)
                {
                    continue;
                }
            }

            mouse.pos_x = tmpx;
            mouse.pos_y = tmpy;
            break;
        }
    }

    void draw()
    {
        mvprintw(mouse.pos_y, mouse.pos_x, FOOD_CHAR);
    }
};

class Snake
{
public:
    std::vector<UnitType> body;

    Snake(PlaneSize _plane, Direction _initial_dir = Right, int _initial_len = 3)
    {
        for (int i = 0; i < _initial_len; ++i)
        {
            if (_initial_dir == Left)
            {
                body.push_back(UnitType((_plane.width / 2) + i, _plane.height / 2));
            }
            else if (_initial_dir == Right)
            {
                body.push_back(UnitType((_plane.width / 2) - i, _plane.height / 2));
            }
            else if (_initial_dir == Up)
            {
                body.push_back(UnitType(_plane.width / 2, (_plane.height / 2) - 1));
            }
            else
            {
                body.push_back(UnitType(_plane.width / 2, (_plane.height / 2) + 1));
            }
        }
    }

    void update(Direction _dir)
    {
        move(body[body.size() - 1].pos_y, body[body.size() - 1].pos_x);
        printw(" ");
        refresh();
        body.pop_back();

        switch (_dir)
        {
        case Left:
            body.insert(body.begin(), UnitType(body[0].pos_x - 1, body[0].pos_y));
            break;
        case Right:
            body.insert(body.begin(), UnitType(body[0].pos_x + 1, body[0].pos_y));
            break;
        case Up:
            body.insert(body.begin(), UnitType(body[0].pos_x, body[0].pos_y - 1));
            break;
        case Down:
            body.insert(body.begin(), UnitType(body[0].pos_x, body[0].pos_y + 1));
            break;
        }
    }

    void draw()
    {
        mvprintw(body[0].pos_y, body[0].pos_x, SNAKE_CHAR);
    }
};

class SnakeGame
{
protected:
    int max_width;
    int max_height;

    int score = 0;

    int delay = DELAY_DEFAULT;

    Direction direction = Right;

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

        snake = new Snake(plane->get_size(), direction);

        mouse = new Mouse();
        mouse->insert(plane->get_size(), &snake->body);

        draw();
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
            break;
            // __quit__();
        }

        snake->update(direction);

        if (check_collision())
        {
            exit(1);
        }

        catch_mouse();
    }

    bool check_collision()
    {
        if (snake->body[0].pos_x == 0 || snake->body[0].pos_y == 0 || snake->body[0].pos_x == plane->get_size().width - 1 || snake->body[0].pos_y == plane->get_size().height - 1)
        {
            return true;
        }

        for (int i = 3; i < ((int)snake->body.size()); i++)
        {
            if (snake->body[0].collision(snake->body[i]))
                return true;
        }

        return false;
    }

    void catch_mouse()
    {
        if (mouse->mouse.collision(snake->body[0]))
        {
            snake->body.push_back(snake->body[snake->body.size() - 1]);
            mouse->insert(plane->get_size(), &snake->body);
            score += 10;
        }
    }

    void draw_score()
    {
        move(max_height, 2);
        printw("SCORE: %d / LEN: %d", score, (int)snake->body.size());
    }

    void draw()
    {
        plane->draw();
        mouse->draw();
        snake->draw();
        draw_score();
        refresh();
    }

    void start_game()
    {
        while (true)
        {
            update();
            draw();
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

void __quit__();

#endif