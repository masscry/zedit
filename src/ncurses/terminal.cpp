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
    void clear() override;
    void print(const std::string& str) override;
    void move_cursor(int pos) override;
    void finish() override;
private:
    CursesTerminal(const CursesTerminal&) /* = delete */;
    CursesTerminal& operator=(const CursesTerminal&) /* = delete */;
    CursesTerminal(CursesTerminal&&) /* = delete */;
    CursesTerminal& operator=(CursesTerminal&&) /* = delete */;

    SCREEN* _scr;
};

CursesTerminal::CursesTerminal() {
    _scr = ::newterm(::getenv("TERM"), stdout, stdin);
    ::cbreak();
    ::noecho();
    ::keypad(stdscr, true);
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

void CursesTerminal::clear() {
    ::werase(stdscr);
}

void CursesTerminal::print(const std::string& str) {
    ::printw("%s", str.c_str());
}

void CursesTerminal::move_cursor(int pos) {
    ::wmove(stdscr, 0, pos);
}

void CursesTerminal::finish() {
    ::refresh();
}

UPTerminal ITerminal::default_terminal() {
    return std::unique_ptr<ITerminal>(new CursesTerminal);
}

} // namespace zedit