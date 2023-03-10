#include "zedit.h"

#include <curses.h>

#include <memory>

namespace zedit {

/// \brief Basic curses-based terminal.
class CursesTerminal: public ITerminal {
public:
    CursesTerminal();
    ~CursesTerminal();

    Event get() override;
    void clear_line() override;
    void print(const std::string& str) override;
    void print(
        const std::string&,
        std::size_t start,
        std::size_t stop,
        Color
    ) override;
    void move_cursor(int pos) override;
    void scroll_line() override;
    void finish() override;

private:
    CursesTerminal(const CursesTerminal&) /* = delete */;
    CursesTerminal& operator=(const CursesTerminal&) /* = delete */;
    CursesTerminal(CursesTerminal&&) /* = delete */;
    CursesTerminal& operator=(CursesTerminal&&) /* = delete */;

    SCREEN* _scr;
    int _line;
};

CursesTerminal::CursesTerminal()
    : _scr(::newterm(::getenv("TERM"), stdout, stdin))
    , _line(0)
{
    ::cbreak();
    ::noecho();
    ::keypad(stdscr, true);
    ::scrollok(stdscr, true);
    ::start_color();

    ::init_pair(Color::WHITE, COLOR_WHITE, COLOR_BLACK);
    ::init_pair(Color::RED, COLOR_RED, COLOR_BLACK);
    ::init_pair(Color::GREEN, COLOR_GREEN, COLOR_BLACK);
    ::init_pair(Color::BLUE, COLOR_BLUE, COLOR_BLACK);
}

CursesTerminal::~CursesTerminal() {
    ::endwin();
    ::delscreen(_scr);
}

Event CursesTerminal::get() {
    switch(auto smb = ::getch()) {
    case KEY_LEFT:
        return Event { Event::SPECIAL, CURSOR_LEFT };
    case KEY_RIGHT:
        return Event { Event::SPECIAL, CURSOR_RIGHT };
    case KEY_UP:
        return Event { Event::SPECIAL, CURSOR_UP };
    case KEY_DOWN:
        return Event { Event::SPECIAL, CURSOR_DOWN };
    case '\n':
        return Event { Event::SPECIAL, ENTER };
    case KEY_BACKSPACE:
        return Event { Event::SPECIAL, BACKSPACE };
    case KEY_DC:
        return Event { Event::SPECIAL, DELETE };
    default:
        return Event { Event::KEY, smb };
    }
}

void CursesTerminal::clear_line() {
    ::clrtoeol();
}

void CursesTerminal::print(const std::string& str) {
    ::wattron(stdscr, COLOR_PAIR(Color::WHITE));
    ::waddnstr(stdscr, str.c_str(), -1);
}

void CursesTerminal::print(
    const std::string& text,
    std::size_t start,
    std::size_t stop,
    Color color
) {
    ::wattron(stdscr, COLOR_PAIR(color));
    ::waddnstr(stdscr, text.c_str() + start, stop-start);
}


void CursesTerminal::move_cursor(int pos) {
    ::wmove(stdscr, _line, pos);
}

void CursesTerminal::scroll_line() {
    if (_line < getmaxy(stdscr)-1) {
        _line += 1;
    } else {
        scroll(stdscr);
    }
}

void CursesTerminal::finish() {
    ::refresh();
}

UPTerminal ITerminal::default_terminal() {
    return std::unique_ptr<ITerminal>(new CursesTerminal);
}

} // namespace zedit