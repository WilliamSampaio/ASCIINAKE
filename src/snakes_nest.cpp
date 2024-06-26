#include <iostream>
#include <ncurses.h>
#include <cctype>

#include "snakes_nest.hpp"

Plane::Plane()
{
    resize();
}

PlaneSize Plane::get_size()
{
    return size;
}

bool Plane::resize()
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

void Plane::draw()
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

void Dialog::draw(PlaneSize _plane, const std::vector<std::string> texts)
{
    size_t width, height, x, y;
    width = texts[0].length() + 1;
    height = texts.size() + 1;
    x = ((_plane.width / 2) - 1) - (width / 2);
    y = ((_plane.height / 2) - 1) - (height / 2);

    mvaddch(y, x, ACS_BSSB);
    mvaddch(y, x + width, ACS_BBSS);
    mvaddch(y + height, x, ACS_LLCORNER);
    mvaddch(y + height, x + width, ACS_LRCORNER);

    for (size_t i = 1; i < width; i++)
    {
        mvaddch(y, x + i, ACS_HLINE);
        mvaddch(y + height, x + i, ACS_HLINE);
    }

    for (size_t i = 1; i < height; i++)
    {
        mvaddch(y + i, x, ACS_VLINE);
        mvaddch(y + i, x + width, ACS_VLINE);
    }

    for (size_t i = 0; i < texts.size(); i++)
    {
        mvaddstr(y + i + 1, x + 1, texts[i].c_str());
    }
}

Snake::Snake(PlaneSize _plane, int _initial_len = 3)
{
    initial_pos(_plane, _initial_len);
}

void Snake::initial_pos(PlaneSize _plane, size_t _initial_len)
{
    body.clear();
    for (size_t i = 0; i < _initial_len; i++)
    {
        body.push_back(UnitType(_plane.width / 2, _plane.height / 2));
    }
}

void Snake::update(Direction _dir)
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

void Snake::draw(bool redraw = false)
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

void Mouse::insert(PlaneSize _plane, Snake *_snake)
{
    do
    {
        bool collision = false;
        UnitType random = random_pos(_plane);
        for (size_t i = 0; i < _snake->body.size(); i++)
        {
            if (_snake->body[i].collision(random))
            {
                collision = true;
                break;
            }
        }
        if (!collision)
        {
            mouse = random;
            break;
        }
    } while (true);
}

UnitType Mouse::random_pos(PlaneSize _plane)
{
    size_t tmpx = rand() % (_plane.width - 1);
    if (tmpx == 0)
        tmpx++;

    size_t tmpy = rand() % (_plane.height - 1);
    if (tmpy == 0)
        tmpy++;
    return UnitType(tmpx, tmpy);
}

void Mouse::draw()
{
    attron(COLOR_PAIR(Margenta));
    mvaddstr(mouse.pos_y, mouse.pos_x, CHAR_MOUSE);
    attron(COLOR_PAIR(Green));
}

SnakeGame::SnakeGame()
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

    init_pair(Colors::Blue, COLOR_BLUE, COLOR_BLACK);
    init_pair(Colors::Cyan, COLOR_CYAN, COLOR_BLACK);
    init_pair(Colors::Green, COLOR_GREEN, COLOR_BLACK);
    init_pair(Colors::Margenta, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(Colors::Red, COLOR_RED, COLOR_BLACK);
    init_pair(Colors::White, COLOR_WHITE, COLOR_BLACK);
    init_pair(Colors::Yellow, COLOR_YELLOW, COLOR_BLACK);

    srand(time(NULL));
}

void SnakeGame::init_game()
{
    score = 0;
    delay = DELAY_DEFAULT;
    direction = Right;
    plane = new Plane();
    snake = new Snake(plane->get_size());
    mouse = new Mouse();
    mouse->insert(plane->get_size(), snake);
    status = GameStatus::Running;
    attron(COLOR_PAIR(Green));
    clear();
    draw();
}

void SnakeGame::update()
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
    case 'P':
        (status == GameStatus::Paused) ? resume() : pause();
        break;
    case 'R':
        if (status == GameStatus::Gameover || status == GameStatus::Completed)
        {
            init_game();
        }
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

        if (score == 1000)
            status = GameStatus::Completed;

        break;
    default:
        return;
    }
}

void SnakeGame::update_direction(int _key)
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

bool SnakeGame::check_collision()
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

void SnakeGame::catch_mouse()
{
    if (mouse->mouse.collision(snake->body[0]))
    {
        snake->body.push_back(snake->body[snake->body.size() - 1]);
        mouse->insert(plane->get_size(), snake);
        score += 10;
        delay -= 2000;
    }
}

void SnakeGame::pause()
{
    if (status == GameStatus::Running)
        status = GameStatus::Paused;
}

void SnakeGame::resume()
{
    if (status == GameStatus::Paused)
    {
        status = GameStatus::Running;
        clear();
        snake->draw(true);
    }
}

void SnakeGame::draw_score()
{
    attron(COLOR_PAIR(Yellow));
    std::string msg = "SCORE: " + std::to_string(score) + " / LEN: " + std::to_string((int)snake->body.size());
    mvaddstr(
        plane->get_size().height + 1,
        (plane->get_size().width - msg.length()) / 2,
        msg.c_str());
    attron(COLOR_PAIR(Green));
}

void SnakeGame::draw()
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
        Dialog::draw(plane->get_size(), pause_text);
        attroff(A_BLINK);
        attron(COLOR_PAIR(Green));
        break;
    case Gameover:
        attron(COLOR_PAIR(Red));
        Dialog::draw(plane->get_size(), game_over_text);
        attron(COLOR_PAIR(Green));
        break;
    case Completed:
        attron(COLOR_PAIR(1 + rand() % Yellow));
        Dialog::draw(plane->get_size(), win_text);
        attron(COLOR_PAIR(Green));
        break;
    default:
        break;
    }

    draw_score();

    refresh();
}

void SnakeGame::start_game()
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

SnakeGame::~SnakeGame()
{
    curs_set(true);
    nodelay(stdscr, false);
    endwin();
    echo();
}