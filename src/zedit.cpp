#include "zedit.h"

#include <cassert>
#include <cctype>
#include <string>
#include <utility>

namespace zedit {

template<typename Func>
class Deferred {
public:
    Deferred(Func&& func) : _func(std::forward<Func>(func)) { ; }
    ~Deferred() { _func(); }
private:
    Func _func;
};

template<typename Func>
Deferred<Func> make_deferred(Func&& func) {
    return Deferred<Func>(std::forward<Func>(func));
}

namespace {

int clamp(int value, int minVal, int maxVal) {
    return (value < minVal)
        ? minVal
        : (value > maxVal)
            ? maxVal
            : value;
}

} // namespace

ITerminal::ITerminal() { ; }

ITerminal::~ITerminal() { ; }

Engine::Engine(const char* welcome, UPTerminal&& terminal) 
    : _terminal(std::move(terminal))
    , _welcome(welcome)
    , _cursor_pos(0)
    , _history_pos(0) { }

Engine::~Engine() { ; }

const std::string& Engine::ready() const throw() {
    return _ready;
}

Engine::Status Engine::process_input() {
    auto user_text = _user_text.str();
    if (!user_text.empty()) {
        _terminal->move_cursor(0);
        _terminal->clear_line();
        _terminal->print(user_text);
        _user_text.str(std::string());
        _terminal->scroll_line();
    }

    auto cur_line = current();

    _terminal->move_cursor(0);
    _terminal->clear_line();
    _terminal->print(cur_line);

    auto smb = _terminal->get();
    auto finish_terminal = make_deferred([this](){
        _terminal->finish();
    });

    Engine::Status status = Engine::Status::WAIT;
    switch(smb.type) {
    case Event::KEY:
        status = handle_char(smb.data);
        break;
    case Event::SPECIAL:
        status = handle_special(static_cast<SpecialKey>(smb.data));
        break;
    default:
        assert(0);
        throw Error("Unknown event type");
    }
    if (status == Engine::Status::NEW_INPUT) {
        _terminal->scroll_line();
    }
    return status;
}

void Engine::print(const std::string& text) {
    _user_text << text;
}

std::string Engine::current() const {
    return _welcome + _edit;
}

int Engine::cursor_pos() const throw() {
    return _welcome.size() + _cursor_pos;
}

Engine::Status Engine::handle_char(int smb) {
    if ((!std::isprint(smb))) {
        return Engine::Status::WAIT;
    }
    if (_edit.size() == _cursor_pos) {
        _edit.push_back(static_cast<char>(smb));
        _cursor_pos = _edit.size();
    } else {
        _edit.insert(_cursor_pos, 1, static_cast<char>(smb));
        _cursor_pos += 1;
    }
    return Engine::Status::WAIT;
}

Engine::Status Engine::handle_special(SpecialKey key) {
    switch (key) {
        case CURSOR_UP:
            if (_history.empty()) {
                return Engine::Status::WAIT;
            }
            _history_pos = clamp(_history_pos - 1, 0, _history.size());
            _edit = _history[_history_pos];
            _cursor_pos = _edit.size();
            return Engine::Status::WAIT;
        case CURSOR_DOWN:
            if (_history.empty()) {
                return Engine::Status::WAIT;
            }
            _history_pos = clamp(_history_pos + 1, 0, _history.size());
            if (_history_pos != _history.size()) {
                _edit = _history[_history_pos];
                _cursor_pos = _edit.size();
            } else {
                _edit.clear();
                _cursor_pos = 0;
            }
            return Engine::Status::WAIT;
        case CURSOR_LEFT:
            _cursor_pos = clamp(_cursor_pos - 1, 0, _edit.size());
            return Engine::Status::WAIT;
        case CURSOR_RIGHT:
            _cursor_pos = clamp(_cursor_pos + 1, 0, _edit.size());
            return Engine::Status::WAIT;
        case BACKSPACE:
            if (_edit.empty()) {
                return Engine::Status::WAIT;
            }
            if (_edit.size() == _cursor_pos) {
                _edit.pop_back();
                _cursor_pos = _edit.size();
            } else {
                _edit.erase(_cursor_pos, 1);
                _cursor_pos -= 1;
            }
            return Engine::Status::WAIT;
        case DELETE:
            if (_edit.empty()) {
                return Engine::Status::WAIT;
            }
            if (_cursor_pos < _edit.size()) {
                _edit.erase(_cursor_pos, 1);
            }
            return Engine::Status::WAIT;
        case ENTER:
            _history.push_back(_edit);
            _history_pos = _history.size();
            _ready = std::move(_edit);
            _cursor_pos = 0;
            return Engine::Status::NEW_INPUT;
        default:
            assert(0);
            throw Error("Invalid special key");
    }
}

}