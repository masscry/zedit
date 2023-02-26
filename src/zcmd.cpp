#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <sstream>

#include <zedit.h>
#include <zdict.h>

int main(int, char*[]) {
    std::unordered_set<std::string> dict {
        "exit"
    };

    zedit::Engine engine(
        "zcmd> ",
        zedit::ITerminal::default_terminal(),
        std::make_unique<zedit::BasicDictFormatter>(std::move(dict))
    );

    while(true) {
        auto status = engine.process_input();
        if (status == zedit::Engine::Status::NEW_INPUT) {
            if (engine.ready() == "exit") {
                break;
            }
            engine.print("OK");
        }
    }
    return 0;
}
