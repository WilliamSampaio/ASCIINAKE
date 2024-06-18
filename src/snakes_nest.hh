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

class Food
{
protected:
    UnitType food;

public:
    Food()
    {
        srand(time(NULL));
    }

    void insert(PlaneSize size, std::vector<UnitType> *snake)
    {
        while (true)
        {
            int tmpx = rand() % size.width;
            if (tmpx == 0)
                tmpx++;

            int tmpy = rand() % size.height;
            if (tmpy == 0)
                tmpy++;

            for (int i = 0; i < ((int)snake->size()); ++i)
            {
                if (snake->at(i).pos_x == tmpx && snake->at(i).pos_y == tmpy)
                {
                    continue;
                }
            }

            food.pos_x = tmpx;
            food.pos_y = tmpy;
            break;
        }
    }

    void draw()
    {
        mvprintw(food.pos_y, food.pos_x, FOOD_CHAR);
        refresh();
    }
};

void __quit__();

#endif