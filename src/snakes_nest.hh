#ifndef SNAKES_NEST_H
#define SNAKES_NEST_H

#ifdef _WIN32
#define CLEAR "cls"
#include <windows.h>
#elif linux
#define CLEAR "clear"
#include <unistd.h>
#endif

#define KEY_QUIT 27 // ESC
#define KEY_PAUSE 'P'
#define KEY_RESET 'R'
#define CHAR_BLOCK "🮐" // 🮐
#define CHAR_SNAKE "●" // ●
#define CHAR_MOUSE "◯" // ◯
#define DELAY_DEFAULT 250000

#define TITLE "ASCIINAKE"

#include <iostream>
#include <ncurses.h>
#include <vector>
#include <cctype>

const std::string pause_text[] = {
    "+-----------------------------+",
    "|         << PAUSED >>        |",
    "| (P to Resume / ESC to Quit) |",
    "+-----------------------------+"};

const std::string game_over_text[] = {
    "+----------------------------+",
    "|      << GAME OVER! >>      |",
    "| (R to Reset / ESC to Quit) |",
    "+----------------------------+"};

const std::string win_text[] = {
    "+----------------------------+",
    "|       << YOU WIN! >>       |",
    "| (R to Reset / ESC to Quit) |",
    "+----------------------------+"};

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

enum Colors
{
    Black,
    Blue,
    Cyan,
    Green,
    Margenta,
    Red,
    White,
    Yellow
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
        size_t height = getmaxy(stdscr) - 3;

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
        mvaddch(size.height + 2, 0, ACS_LLCORNER);
        mvaddch(size.height + 2, size.width - 1, ACS_LRCORNER);

        for (size_t i = 1; i < size.width - 1; i++)
        {
            mvaddstr(0, i, CHAR_BLOCK);
            mvaddstr(size.height - 1, i, CHAR_BLOCK);

            mvaddch(size.height, i, ACS_HLINE);
            mvaddch(size.height + 2, i, ACS_HLINE);
        }

        for (size_t i = 1; i < size.height - 1; i++)
        {
            mvaddstr(i, 0, CHAR_BLOCK);
            mvaddstr(i, size.width - 1, CHAR_BLOCK);
        }

        for (int i = 1; i < 2; i++)
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

    void initial_pos(PlaneSize _plane, size_t _initial_len)
    {
        body.clear();
        for (size_t i = 0; i < _initial_len; i++)
        {
            body.push_back(UnitType(_plane.width / 2, _plane.height / 2));
        }
    }

    void update(Direction _dir)
    {
        mvaddstr(body[body.size() - 1].pos_y, body[body.size() - 1].pos_x, " ");
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

    void draw(bool redraw = false)
    {
        attron(COLOR_PAIR(Cyan));
        if (!redraw)
        {
            mvaddstr(body[0].pos_y, body[0].pos_x, CHAR_SNAKE);
            return;
        }
        mvaddstr(body[body.size() - 1].pos_y, body[body.size() - 1].pos_x, " ");
        for (size_t i = 0; i < body.size(); i++)
        {
            mvaddstr(body[i].pos_y, body[i].pos_x, CHAR_SNAKE);
        }
        attron(COLOR_PAIR(Green));
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
            if (_snake->body[i].collision(random))
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
        attron(COLOR_PAIR(Margenta));
        mvaddstr(mouse.pos_y, mouse.pos_x, CHAR_MOUSE);
        attron(COLOR_PAIR(Green));
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

        if (has_colors() == FALSE)
        {
            endwin();
            printf("Your terminal does not support colors.\n");
            exit(1);
        }

        start_color();
        cbreak();
        nodelay(stdscr, true);
        keypad(stdscr, true);
        noecho();
        curs_set(false);

        init_pair(Red, COLOR_RED, COLOR_BLACK);
        init_pair(Green, COLOR_GREEN, COLOR_BLACK);
        init_pair(Cyan, COLOR_CYAN, COLOR_BLACK);
        init_pair(Yellow, COLOR_YELLOW, COLOR_BLACK);
        init_pair(Margenta, COLOR_MAGENTA, COLOR_BLACK);

        srand(time(NULL));
    }

    void init_game()
    {
        plane = new Plane();
        snake = new Snake(plane->get_size());
        mouse = new Mouse();
        mouse->insert(plane->get_size(), snake);
        status = GameStatus::Running;
        attron(COLOR_PAIR(Green));
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

        int KEY = getch();
        KEY = toupper(KEY);

        switch (KEY)
        {
        case KEY_QUIT:
            status = GameStatus::Leave;
            break;
        case KEY_PAUSE:
            (status == GameStatus::Paused) ? resume() : pause();
            break;
        }

        switch (status)
        {
        case GameStatus::Running:
            update_direction(KEY);
            snake->update(direction);

            if (check_collision())
                status = GameStatus::Gameover;

            catch_mouse();

            if (delay == 50000)
                status = GameStatus::Completed;

            break;
        default:
            if (KEY == KEY_RESET)
                init_game();
            return;
        }
    }

    void update_direction(int _key)
    {
        switch (_key)
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

    void pause()
    {
        status = GameStatus::Paused;
    }

    void resume()
    {
        status = GameStatus::Running;
        clear();
        snake->draw(true);
    }

    void draw_score()
    {
        attron(COLOR_PAIR(Yellow));
        std::string msg = "SCORE: " + std::to_string(score) + " / LEN: " + std::to_string((int)snake->body.size());
        mvaddstr(
            plane->get_size().height + 1,
            (plane->get_size().width - msg.length()) / 2,
            msg.c_str());
        attron(COLOR_PAIR(Green));
    }

    void draw()
    {
        plane->draw();
        switch (status)
        {
        case Running:
            mouse->draw();
            snake->draw();
            break;
        case Paused:
            attron(A_BLINK);
            attron(COLOR_PAIR(Yellow));
            for (size_t i = 0; i < (sizeof(pause_text) / sizeof(pause_text[0])); i++)
            {
                mvaddstr(
                    ((plane->get_size().height / 2) - ((sizeof(pause_text) / sizeof(pause_text[0])) / 2)) + i,
                    (plane->get_size().width - pause_text[i].length()) / 2,
                    pause_text[i].c_str());
            }
            attroff(A_BLINK);
            attron(COLOR_PAIR(Green));
            break;
        case Gameover:
            attron(COLOR_PAIR(Red));
            for (size_t i = 0; i < (sizeof(game_over_text) / sizeof(game_over_text[0])); i++)
            {
                mvaddstr(
                    ((plane->get_size().height / 2) - ((sizeof(game_over_text) / sizeof(game_over_text[0])) / 2)) + i,
                    (plane->get_size().width - game_over_text[i].length()) / 2,
                    game_over_text[i].c_str());
            }
            attron(COLOR_PAIR(Green));
            break;
        case Completed:
            for (size_t i = 0; i < (sizeof(win_text) / sizeof(win_text[0])); i++)
            {
                mvaddstr(
                    ((plane->get_size().height / 2) - ((sizeof(win_text) / sizeof(win_text[0])) / 2)) + i,
                    (plane->get_size().width - win_text[i].length()) / 2,
                    win_text[i].c_str());
            }
            break;
        default:
            break;
        }

        draw_score();

        refresh();
    }

    void start_game()
    {
        init_game();
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
        curs_set(true);
        nodelay(stdscr, false);
        endwin();
        echo();
    }
};

#endif