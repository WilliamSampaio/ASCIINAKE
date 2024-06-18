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
#define CHAR_BLOCK '#' // 🮐
#define CHAR_SNAKE 'O' // ●
#define CHAR_MOUSE 'X' // ◯
#define DELAY_DEFAULT 250000

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
    size_t width, height;
    PlaneSize(size_t _width = 10, size_t _height = 10)
    {
        width = _width;
        height = _height;
    }
};

struct UnitType
{
    size_t pos_x, pos_y;
    UnitType(size_t _pos_x = 0, size_t _pos_y = 0)
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
    Plane()
    {
        resize();
    }

    PlaneSize get_size()
    {
        return size;
    }

    bool resize()
    {
        size_t width = getmaxx(stdscr);
        size_t height = (getmaxy(stdscr) / 5) * 4;

        if (width != size.width || height != size.height)
        {
            size.width = width;
            size.height = height;
            return true;
        }
        return false;
    }

    void draw()
    {
        mvaddch(0, 0, CHAR_BLOCK);
        mvaddch(0, size.width - 1, CHAR_BLOCK);
        mvaddch(size.height - 1, 0, CHAR_BLOCK);
        mvaddch(size.height - 1, size.width - 1, CHAR_BLOCK);

        for (size_t i = 1; i < size.width - 1; i++)
        {
            mvaddch(0, i, CHAR_BLOCK);
            mvaddch(size.height - 1, i, CHAR_BLOCK);
        }

        for (size_t i = 1; i < size.height - 1; i++)
        {
            mvaddch(i, 0, CHAR_BLOCK);
            mvaddch(i, size.width - 1, CHAR_BLOCK);
        }
    }
};

class Snake
{
public:
    std::vector<UnitType> body;

    Snake(PlaneSize _plane, Direction _initial_dir = Right, int _initial_len = 3)
    {
        initial_pos(_plane, _initial_dir, _initial_len);
    }

    void initial_pos(PlaneSize _plane, Direction _initial_dir = Right, int _initial_len)
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
        mvaddch(body[0].pos_y, body[0].pos_x, CHAR_SNAKE);
    }
};

class Mouse
{
public:
    UnitType mouse;

    void insert(PlaneSize _plane, Snake *_snake)
    {
        UnitType random = random_pos(_plane);

        for (size_t i = 0; i < _snake->body.size(); ++i)
        {
            if (_snake->body[i].pos_x == random.pos_x && _snake->body[i].pos_y == random.pos_y)
            {
                insert(_plane, _snake);
            }
        }

        mouse = random;
    }

    UnitType random_pos(PlaneSize _plane)
    {
        size_t tmpx = rand() % (_plane.width - 1);
        if (tmpx == 0)
            tmpx++;

        size_t tmpy = rand() % (_plane.height - 1);
        if (tmpy == 0)
            tmpy++;
        return UnitType(tmpx, tmpy);
    }

    void draw()
    {
        mvaddch(mouse.pos_y, mouse.pos_x, CHAR_MOUSE);
    }
};

class SnakeGame
{
protected:
    int score = 0;

    int delay = DELAY_DEFAULT;

    Direction direction = Right;

    Plane *plane;
    Snake *snake;
    Mouse *mouse;

    bool quit = false;
    bool game_over = false;
    bool win = false;

public:
    SnakeGame()
    {
        initscr();
        nodelay(stdscr, true);
        keypad(stdscr, true);
        noecho();
        curs_set(0);
        srand(time(NULL));

        plane = new Plane();

        snake = new Snake(plane->get_size(), direction);

        mouse = new Mouse();
        mouse->insert(plane->get_size(), snake);

        draw();
    }

    void update()
    {
        if (plane->resize())
        {
            clear();
            mouse->insert(plane->get_size(), snake);
        }

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
            quit = true;
            break;
        }

        if (game_over)
            return;

        snake->update(direction);

        if (check_collision())
        {
            game_over = true;
        }

        catch_mouse();

        if (delay == 0)
        {
            win = true;
        }
    }

    bool check_collision()
    {
        if (snake->body[0].pos_x == 0 || snake->body[0].pos_y == 0 || snake->body[0].pos_x == plane->get_size().width - 1 || snake->body[0].pos_y == plane->get_size().height - 1)
        {
            return true;
        }

        for (size_t i = 3; i < snake->body.size(); i++)
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
            mouse->insert(plane->get_size(), snake);
            score += 10;
            delay -= 2000;
        }
    }

    void draw_score()
    {
        move(plane->get_size().height, 2);
        printw("SCORE: %d / LEN: %d", score, (int)snake->body.size());
    }

    void draw()
    {
        plane->draw();
        if (!game_over && !win)
        {
            mouse->draw();
            snake->draw();
            draw_score();
        }
        else if (game_over)
        {
            mvprintw((plane->get_size().height / 2) - 1, (plane->get_size().width / 2) - 5, "Game Over!");
            mvprintw(plane->get_size().height / 2, (plane->get_size().width / 2) - 8, "Type ESC to exit");
        }
        else if (win)
        {
            mvprintw((plane->get_size().height / 2) - 1, (plane->get_size().width / 2) - 4, "You Win!");
            mvprintw(plane->get_size().height / 2, (plane->get_size().width / 2) - 8, "Type ESC to exit");
        }
        refresh();
    }

    void start_game()
    {
        while (true)
        {
            if (quit)
                break;
            update();
            draw();
            usleep(delay);
        }
    }

    ~SnakeGame()
    {
        curs_set(1);
        nodelay(stdscr, false);
        endwin();
        std::cout << "Bye!\n";
    }
};

#endif