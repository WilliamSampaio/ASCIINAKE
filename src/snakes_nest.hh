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
#define CHAR_BLOCK "🮐" // 🮐
#define CHAR_SNAKE "●" // ●
#define CHAR_MOUSE "◯" // ◯
#define DELAY_DEFAULT 250000

#define TITLE "ASCIINAKE"

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

enum GameStatus
{
    Running,
    Gameover,
    Paused,
    Completed,
    Leave
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
        {
            return true;
        }
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
        size_t height = getmaxy(stdscr) - 5;

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
        mvaddstr(0, 0, CHAR_BLOCK);
        mvaddstr(0, size.width - 1, CHAR_BLOCK);
        mvaddstr(size.height - 1, 0, CHAR_BLOCK);
        mvaddstr(size.height - 1, size.width - 1, CHAR_BLOCK);

        mvaddch(size.height, 0, ACS_BSSB);
        mvaddch(size.height, size.width - 1, ACS_BBSS);
        mvaddch(size.height + 4, 0, ACS_LLCORNER);
        mvaddch(size.height + 4, size.width - 1, ACS_LRCORNER);

        for (size_t i = 1; i < size.width - 1; i++)
        {
            mvaddstr(0, i, CHAR_BLOCK);
            mvaddstr(size.height - 1, i, CHAR_BLOCK);

            mvaddch(size.height, i, ACS_HLINE);
            mvaddch(size.height + 4, i, ACS_HLINE);
        }

        for (size_t i = 1; i < size.height - 1; i++)
        {
            mvaddstr(i, 0, CHAR_BLOCK);
            mvaddstr(i, size.width - 1, CHAR_BLOCK);
        }

        for (int i = 1; i < 4; i++)
        {
            mvaddch(size.height + i, 0, ACS_VLINE);
            mvaddch(size.height + i, size.width - 1, ACS_VLINE);
        }
    }
};

class Snake
{
public:
    std::vector<UnitType> body;

    Snake(PlaneSize _plane, int _initial_len = 3)
    {
        initial_pos(_plane, _initial_len);
    }

    void initial_pos(PlaneSize _plane, int _initial_len)
    {
        body.clear();
        for (int i = 0; i < _initial_len; i++)
        {
            body.push_back(UnitType(_plane.width / 2, _plane.height / 2));
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
        mvaddstr(body[0].pos_y, body[0].pos_x, CHAR_SNAKE);
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
        mvaddstr(mouse.pos_y, mouse.pos_x, CHAR_MOUSE);
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

    GameStatus status;

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

        snake = new Snake(plane->get_size());

        mouse = new Mouse();
        mouse->insert(plane->get_size(), snake);

        status = GameStatus::Running;

        draw();
    }

    void update()
    {
        if (plane->resize())
        {
            clear();
            snake->initial_pos(plane->get_size(), (int)snake->body.size());
            mouse->insert(plane->get_size(), snake);
        }

        int tmp = getch();

        if (tmp == 'p' || tmp == 'P')
        {
            if (status == GameStatus::Paused)
            {
                status = GameStatus::Running;
                clear();
            }
            else
            {
                status = GameStatus::Paused;
            }
        }

        if (status == GameStatus::Paused)
            return;

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
            status = GameStatus::Leave;
            break;
        }

        if (status == GameStatus::Gameover)
            return;

        snake->update(direction);

        if (check_collision())
        {
            status = GameStatus::Gameover;
        }

        catch_mouse();

        if (delay == 0)
        {
            status = GameStatus::Completed;
        }
    }

    bool check_collision()
    {
        if (snake->body[0].pos_x == 0 || snake->body[0].pos_y == 0 || snake->body[0].pos_x == plane->get_size().width - 1 || snake->body[0].pos_y == plane->get_size().height - 1)
        {
            return true;
        }

        for (size_t i = 1; i < snake->body.size(); i++)
        {
            if (snake->body[0].collision(snake->body[i]))
            {
                return true;
            }
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
        std::string msg = "SCORE: " + std::to_string(score) + " / LEN: " + std::to_string((int)snake->body.size());
        mvaddstr(
            plane->get_size().height + 2,
            (plane->get_size().width - msg.length()) / 2,
            msg.c_str());
    }

    void draw()
    {
        if (status == GameStatus::Running)
        {
            plane->draw();
            mouse->draw();
            snake->draw();
            draw_score();
        }

        if (status == GameStatus::Paused)
        {
            plane->draw();
            std::string pause_text[] = {
                "+---------------------------+",
                "|          PAUSED!          |",
                "|    (Press P to Resume)    |",
                "+---------------------------+"};

            for (int i = 0; i < 4; i++)
            {
                mvaddstr(
                    ((plane->get_size().height / 2) - 2) + i,
                    (plane->get_size().width - pause_text[i].length()) / 2,
                    pause_text[i].c_str());
            }
            draw_score();
        }

        else if (status == GameStatus::Gameover)
        {
            mvprintw((plane->get_size().height / 2) - 1, (plane->get_size().width / 2) - 5, "Game Over!");
            mvprintw(plane->get_size().height / 2, (plane->get_size().width / 2) - 8, "Type ESC to exit");
        }
        else if (status == GameStatus::Completed)
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
            if (status == GameStatus::Leave)
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
        system(CLEAR);
    }
};

#endif