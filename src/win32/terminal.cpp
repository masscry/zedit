#include "zedit.h"

#include <memory>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace {

template<typename Func, typename Arg1>
void WinCall(Func&& func, Arg1&& a1) {
    if (func(std::forward<Arg1>(a1)) == FALSE) {
        std::stringstream text;
        text << "WinCall[1] Failed (" << GetLastError() << ")";
        throw Error(text.str());
    }
}

template<typename Func, typename Arg1, typename Arg2>
void WinCall(Func&& func, Arg1&& a1, Arg2&& a2) {
    if (func(std::forward<Arg1>(a1), std::forward<Arg2>(a2)) == FALSE) {
        std::stringstream text;
        text << "WinCall[2] Failed (" << GetLastError() << ")";
        throw zedit::Error(text.str());
    }
}

template<typename Func, typename Arg1, typename Arg2, typename Arg3>
void WinCall(Func&& func, Arg1&& a1, Arg2&& a2, Arg3&& a3) {
    if (func(std::forward<Arg1>(a1), std::forward<Arg2>(a2), std::forward<Arg3>(a3)) == FALSE) {
        std::stringstream text;
        text << "WinCall[3] Failed (" << GetLastError() << ")";
        throw zedit::Error(text.str());
    }
}

template<typename Func, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
void WinCall(Func&& func, Arg1&& a1, Arg2&& a2, Arg3&& a3, Arg4&& a4) {
    if (func(std::forward<Arg1>(a1), std::forward<Arg2>(a2), std::forward<Arg3>(a3), std::forward<Arg4>(a4)) == FALSE) {
        std::stringstream text;
        text << "WinCall[4] Failed (" << GetLastError() << ")";
        throw zedit::Error(text.str());
    }
}


template<typename Func, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
void WinCall(Func&& func, Arg1&& a1, Arg2&& a2, Arg3&& a3, Arg4&& a4, Arg5&& a5) {
    if (func(std::forward<Arg1>(a1), std::forward<Arg2>(a2), std::forward<Arg3>(a3), std::forward<Arg4>(a4), std::forward<Arg5>(a5)) == FALSE) {
        std::stringstream text;
        text << "WinCall[5] Failed (" << GetLastError() << ")";
        throw zedit::Error(text.str());
    }
}

template<typename Func, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
void WinCall(Func&& func, Arg1&& a1, Arg2&& a2, Arg3&& a3, Arg4&& a4, Arg5&& a5, Arg6&& a6) {
    if (func(std::forward<Arg1>(a1), std::forward<Arg2>(a2), std::forward<Arg3>(a3), std::forward<Arg4>(a4), std::forward<Arg5>(a5), std::forward<Arg6>(a6)) == FALSE) {
        std::stringstream text;
        text << "WinCall[6] Failed (" << GetLastError() << ")";
        throw zedit::Error(text.str());
    }
}

} // namespace

namespace zedit {

/// \brief Basic curses-based terminal.
class Win32Terminal: public ITerminal {
public:
    Win32Terminal();
    ~Win32Terminal();

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
    Win32Terminal(const Win32Terminal&) /* = delete */;
    Win32Terminal& operator=(const Win32Terminal&) /* = delete */;
    Win32Terminal(Win32Terminal&&) /* = delete */;
    Win32Terminal& operator=(Win32Terminal&&) /* = delete */;

    HANDLE _in_handle;
    HANDLE _out_handle;
    DWORD _old_in_mode;
    DWORD _old_out_mode;
    int _line;
    int _max_col;
    int _max_line;
};

Win32Terminal::Win32Terminal()
    : _in_handle(GetStdHandle(STD_INPUT_HANDLE))
    , _out_handle(GetStdHandle(STD_OUTPUT_HANDLE))
    , _line(0)
    , _max_col(0)
    , _max_line(0)
{
    if (GetFileType(_in_handle) != FILE_TYPE_CHAR) {
        throw Error("Invalid Terminal");  
    }

    CONSOLE_SCREEN_BUFFER_INFO con_info;
    DWORD console_mode;

    WinCall(GetConsoleScreenBufferInfo, _out_handle, &con_info);

    WinCall(GetConsoleMode, _in_handle, &_old_in_mode);
    WinCall(GetConsoleMode, _out_handle, &_old_out_mode);

    WinCall(SetConsoleMode, _in_handle, 0);
    WinCall(SetConsoleMode, _out_handle, 0);

    _max_col = con_info.dwSize.X;
    _max_line = con_info.dwSize.Y;
}

Win32Terminal::~Win32Terminal() {
    SetConsoleMode(_in_handle, _old_in_mode);
    SetConsoleMode(_out_handle, _old_out_mode);
}

Event Win32Terminal::get() {
    for (;;) {
        INPUT_RECORD recs[1];
    
        DWORD read_recs = 0;
        WinCall(ReadConsoleInput,
          _in_handle,
          recs,
          _countof(recs),
          &read_recs
        );

        for (DWORD i = 0; i < read_recs; ++i) {
            const auto& rec = recs[i];
            if ((rec.EventType == KEY_EVENT) && (rec.Event.KeyEvent.bKeyDown != FALSE)) {
                const auto& key_evt = rec.Event.KeyEvent;
                Event evt;
                switch(key_evt.wVirtualKeyCode) {
                case VK_LEFT:
                    evt.type = Event::SPECIAL;
                    evt.data = CURSOR_LEFT; 
                    return evt;
                case VK_RIGHT:
                    evt.type = Event::SPECIAL;
                    evt.data = CURSOR_RIGHT; 
                    return evt;
                case VK_UP:
                    evt.type = Event::SPECIAL;
                    evt.data = CURSOR_UP; 
                    return evt;
                case VK_DOWN:
                    evt.type = Event::SPECIAL;
                    evt.data = CURSOR_DOWN; 
                    return evt;
                case VK_RETURN:
                    evt.type = Event::SPECIAL;
                    evt.data = ENTER; 
                    return evt;
                case VK_BACK:
                    evt.type = Event::SPECIAL;
                    evt.data = BACKSPACE; 
                    return evt;
                case VK_DELETE:
                    evt.type = Event::SPECIAL;
                    evt.data = DELETE; 
                    return evt;
                default:
                    evt.type = Event::KEY;
                    evt.data = recs[i].Event.KeyEvent.uChar.AsciiChar;
                    return evt;
                }
            }
        }
    }
}

void Win32Terminal::clear_line() {
    char line[256];
    std::memset(line, ' ', sizeof(line));
    CONSOLE_SCREEN_BUFFER_INFO con_info;
    WinCall(GetConsoleScreenBufferInfo, _out_handle, &con_info);
    WinCall(WriteConsole, _out_handle, line, con_info.dwSize.X - con_info.dwCursorPosition.X, nullptr, nullptr);
}

void Win32Terminal::print(const std::string& str) {
    WinCall(SetConsoleTextAttribute, _out_handle, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
    WinCall(WriteConsole, _out_handle, str.c_str(), str.size(), nullptr, nullptr);
}

void Win32Terminal::print(
    const std::string& text,
    std::size_t start,
    std::size_t stop,
    Color color
) {
    WORD color_mask = 0;
    switch(color){
    default:
    case WHITE:
        color_mask = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY;
        break;
    case RED:
        color_mask = FOREGROUND_RED|FOREGROUND_INTENSITY;
        break;
    case GREEN:
        color_mask = FOREGROUND_GREEN|FOREGROUND_INTENSITY;
        break;
    case BLUE:
        color_mask = FOREGROUND_BLUE|FOREGROUND_INTENSITY;
        break;
    }
    WinCall(SetConsoleTextAttribute, _out_handle, color_mask);
    WinCall(WriteConsole, _out_handle, text.c_str()+start, stop-start, nullptr, nullptr);
}


void Win32Terminal::move_cursor(int pos) {
    COORD new_pos;
    new_pos.X = pos;
    new_pos.Y = _line;
    WinCall(SetConsoleCursorPosition, _out_handle, new_pos);
}

void Win32Terminal::scroll_line() {
    _line += 1;
}

void Win32Terminal::finish() {
}

UPTerminal ITerminal::default_terminal() {
    return std::unique_ptr<ITerminal>(new Win32Terminal);
}

} // namespace zedit