#ifndef SNAKES_NEST_H
#define SNAKES_NEST_H

#ifdef _WIN32
#define CLEAR "cls"
#include <windows.h>
#elif linux
#define CLEAR "clear"
#include <unistd.h>
#endif

#define KEY_QUIT 27    // ESC
#define CHAR_BLOCK "🮐" // 🮐
#define CHAR_SNAKE "●" // ●
#define CHAR_MOUSE "◯" // ◯
#define DELAY_DEFAULT 250000

#define TITLE "ASCIINAKE"

#include <iostream>
#include <vector>

const std::vector<std::string> pause_text = {
    "       << PAUSED! >>       ",
    "(P to Resume / ESC to Quit)"};

const std::vector<std::string> game_over_text = {
    "     << GAME OVER! >>     ",
    "(R to Reset / ESC to Quit)"};

const std::vector<std::string> win_text = {
    "      << YOU WIN! >>      ",
    "(R to Reset / ESC to Quit)"};

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
    Plane();

    PlaneSize get_size();

    bool resize();

    void draw();
};

class Dialog
{
public:
    static void draw(PlaneSize, std::vector<std::string>);
};

class Snake
{
public:
    std::vector<UnitType> body;

    Snake(PlaneSize, int);

    void initial_pos(PlaneSize, size_t);

    void update(Direction);

    void draw(bool);
};

class Mouse
{
public:
    UnitType mouse;

    void insert(PlaneSize, Snake *);

    UnitType random_pos(PlaneSize);

    void draw();
};

class SnakeGame
{
protected:
    int score;
    int delay;
    Direction direction;

    Plane *plane;
    Snake *snake;
    Mouse *mouse;

    GameStatus status;

public:
    SnakeGame();

    void init_game();

    void update();

    void update_direction(int);

    bool check_collision();

    void catch_mouse();

    void pause();

    void resume();

    void draw_score();

    void draw();

    void start_game();

    ~SnakeGame();
};

#endif