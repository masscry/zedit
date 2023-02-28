#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <sstream>
#include <iostream>

#include <zedit.h>
#include <zdict.h>

int main(int, char*[]) {
    try {
        std::unordered_set<std::string> dict;
        dict.emplace("exit");

        zedit::Engine engine(
            "zcmd> ",
            zedit::ITerminal::default_terminal(),
            std::unique_ptr<zedit::BasicDictFormatter>(
                new zedit::BasicDictFormatter(std::move(dict))
            )
        );

        while(true) {
            auto status = engine.process_input();
            if (status == zedit::Engine::NEW_INPUT) {
                if (engine.ready() == "exit") {
                    break;
                }
                engine.print("OK");
            }
        }
    } catch (const zedit::Error& error) {
        std::cerr << "\nGot Exception: (" << error.what() << "). Abort." << std::endl;
    }
    return 0;
}
