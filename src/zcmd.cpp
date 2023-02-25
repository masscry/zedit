#include <cstdlib>
#include <zedit.h>

enum CustomKeys {
    ENTER = '\n'
};

int main(int, char*[]) {

    zedit::Engine engine("zcmd> ", zedit::ITerminal::default_terminal());

    while(true) {
        auto status = engine.process_input();
        if (status == zedit::Engine::Status::NEW_INPUT) {
            if (engine.ready() == "exit") {
                break;
            }
        }
    }
    return 0;
}
