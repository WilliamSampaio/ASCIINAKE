#ifndef SNAKES_NEST_H
#define SNAKES_NEST_H

#ifdef _WIN32
#define CLEAR "cls"
#elif linux
#define CLEAR "clear"
#endif

#define QUIT_KEY 27
#define SNAKE_CHAR "\u25CF" // ●
#define FOOD_CHAR "\u25EF"  // ◯

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
        mvprintw(0, 0, "\u250F");
        mvprintw(0, size.width - 1, "\u2513");
        mvprintw(size.height - 1, 0, "\u2517");
        mvprintw(size.height - 1, size.width - 1, "\u251B");

        for (int i = 1; i < size.width - 1; i++)
        {
            mvprintw(0, i, "\u2501");
            mvprintw(size.height - 1, i, "\u2501");
        }

        for (int i = 1; i < size.height - 1; i++)
        {
            mvprintw(i, 0, "\u2503");
            mvprintw(i, size.width - 1, "\u2503");
        }
    }
};

class Mouse
{
protected:
    UnitType mouse;

public:
    void insert(PlaneSize _plane, std::vector<UnitType> *_snake_body)
    {
        while (true)
        {
            int tmpx = rand() % _plane.width;
            if (tmpx == 0)
                tmpx++;

            int tmpy = rand() % _plane.height;
            if (tmpy == 0)
                tmpy++;

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
        refresh();
    }
};

class Snake
{
public:
    std::vector<UnitType> body;

    Snake(PlaneSize _plane, int _initial_len = 3)
    {
        for (int i = 0; i < _initial_len; ++i)
        {
            body.push_back(UnitType((_plane.width / 2) - i, _plane.height / 2));
        }

        for (int i = 0; i < ((int)body.size()); i++)
        {
            mvprintw(body[0].pos_y, body[0].pos_x, SNAKE_CHAR);
        }
    }

    void draw(Direction _dir)
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

        mvprintw(body[0].pos_y, body[0].pos_x, SNAKE_CHAR);
        refresh();
    }
};

void __quit__();

#endif