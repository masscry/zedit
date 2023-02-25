.PHONY: clean configure build

CXX_COMPILER ?= clang++-14
BUILD_TYPE ?= Debug
GENERATOR ?= Ninja

OUT_DIR := out/$(BUILD_TYPE)-$(CXX_COMPILER)

all: build

build: $(OUT_DIR)
	cmake --build $(OUT_DIR)

$(OUT_DIR):
	cmake -G $(GENERATOR) \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_CXX_COMPILER=$(CXX_COMPILER) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-B $(OUT_DIR) \
		-S .
	ln -sf $(OUT_DIR)/compile_commands.json ./

clean:
	rm -rf ./out/$(BUILD_TYPE)-$(CXX_COMPILER)
