/// \file zedit.h
/// \author masscry
#pragma once

#include <cstddef>

#include <memory>
#include <stdexcept>
#include <sstream>
#include <vector>

namespace zedit {

typedef std::unique_ptr<class ITerminal> UPTerminal;

typedef std::unique_ptr<class IFormatter> UPFormatter;

typedef std::unique_ptr<class Engine> UPEngine;

/// \brief Special keys to be process different from simple characters.
enum SpecialKey {
    CURSOR_UP = 1, ///< Move cursor UP
    CURSOR_DOWN, ///< Move cursor DOWN
    CURSOR_LEFT, ///< Move cursor LEFT
    CURSOR_RIGHT, ///< Move cursor RIGHT
    BACKSPACE, ///< Remove character before cursor
    DELETE, ///< Remove character under cursor
    ENTER ///< Accept input
};

enum Color {
    WHITE = 1,
    RED = 2,
    GREEN = 3,
    BLUE = 4,
};

/// \brief User input events
struct Event {
    enum {
        KEY, ///< Common ASCII character
        SPECIAL ///< Special key
    } type; ///< Event type
    int data; ///< character code, or SpecialKey
};

/// \brief Generic errors thrown by Engine
class Error : public std::runtime_error {
public:
    Error(const char*);
    Error(const std::string&);
    Error(std::string&& text);
};

/// \brief Abstract terminal interface, hides platform dependent things.
class ITerminal {
public:

    ITerminal();
    
    virtual
    ~ITerminal() = 0;

    /// \brief Get last user input event.
    virtual 
    Event get() = 0;

    /// \brief Clear terminal.
    virtual 
    void clear_line() = 0;

    /// \brief Print given text at cursor position
    virtual 
    void print(const std::string&) = 0;

    /// \brief Print given text at cursor position from start to stop with color
    virtual 
    void print(
        const std::string&,
        std::size_t start,
        std::size_t stop, 
        Color
    ) = 0;

    /// \brief Move cursor to given position
    virtual
    void move_cursor(int) = 0;

    /// \brief Scroll line up
    virtual
    void scroll_line() = 0;

    /// \brief Do some action at end of read-write loop.
    virtual
    void finish() = 0;

    /// \brief Returns default terminal for given platform.
    static 
    UPTerminal default_terminal();

private:
    ITerminal(const ITerminal&) /* = delete */;
    ITerminal& operator=(const ITerminal&) /* = delete */;
    ITerminal(ITerminal&&) /* = delete */;
    ITerminal& operator=(ITerminal&&) /* = delete */;
};

struct Token {
    enum Type {
        END = 0,
        WHITE,
        RED,
        GREEN,
        BLUE,
    } type;
    std::size_t first;
    std::size_t last;

    Token(Type type, std::size_t first, std::size_t last)
        : type(type)
        , first(first)
        , last(last)
    { ; }

    Token(const Token& src)
        : type(src.type)
        , first(src.first)
        , last(src.last)
    { ; }

    Token& operator=(const Token& src){
        type = src.type;
        first = src.first;
        last = src.last;
        return *this;
    }

    ~Token() { ; }

};

class IFormatter {
public:
    IFormatter();

    virtual 
    ~IFormatter() = 0;

    virtual 
    void add_string(const std::string& text) = 0;
    
    virtual 
    Token get_token() = 0;

private:
    IFormatter(const IFormatter&) /* = delete */;
    IFormatter& operator=(const IFormatter&) /* = delete */;
    IFormatter(IFormatter&&) /* = delete */;
    IFormatter& operator=(IFormatter&&) /* = delete */;
};


/// \brief Console line edit engine.
class Engine {
public:

    enum Status {
        WAIT = 0,
        NEW_INPUT = 1
    };

    Engine(const char* welcome, UPTerminal&& terminal, UPFormatter&& formatter);

    ~Engine();

    /// \brief Last ready command
    const std::string& ready() const throw();

    /// \brief Single user input processing loop.
    /// 
    /// Reads terminal, update current command line, print ready data to 
    /// terminal.
    ///
    /// \return status of processed input round
    /// \retval Status::WAIT when engine needs additional input
    /// \retval STATUS::NEW_INPUT when engine has new input ready
    /// 
    Status process_input();

    void print(const std::string& text);

private:

    std::string current() const;
    int cursor_pos() const throw();

    Status handle_char(int);
    Status handle_special(SpecialKey);

    Engine(const Engine&) /* = delete */;
    Engine& operator=(const Engine&) /* = delete */;
    Engine(Engine&&) /* = delete */;
    Engine& operator=(Engine&&) /* = delete */;

    UPTerminal _terminal;
    UPFormatter _formatter;
    std::string _welcome;
    std::string _ready;
    std::string _edit;
    std::stringstream _user_text;
    std::vector<std::string> _history;
    std::size_t _cursor_pos;
    std::size_t _history_pos;
};

inline
Error::Error(const char* text): std::runtime_error(text) { ; }

inline
Error::Error(const std::string& text): std::runtime_error(text) { ; }

inline
Error::Error(std::string&& text): std::runtime_error(std::move(text)) { ; }

} // namespace zedit
