#pragma once

#include "zedit.h"

#include <unordered_set>

namespace zedit {

class BasicDictFormatter: public IFormatter {
public:
    BasicDictFormatter(std::unordered_set<std::string>&&);
    ~BasicDictFormatter();

    void add_string(const std::string&) override;

    zedit::Token get_token() override;

private:
    std::string _text;
    const char *_cursor;
    const char *_marker;
    const char *_token;
    std::unordered_set<std::string> _dict;
};

} // namespace zedit