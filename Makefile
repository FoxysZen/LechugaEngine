BUILD_DIR = build
BUILD_TYPE = Debug

.PHONY: all configure build run clean rebuild

all: build

configure:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: $(BUILD_DIR)/Makefile
	cmake --build $(BUILD_DIR) --parallel

$(BUILD_DIR)/Makefile:
	$(MAKE) configure

run: build
	./$(BUILD_DIR)/LechugaEngine

release:
	$(MAKE) clean
	$(MAKE) build BUILD_TYPE=Release
	@echo "Release Version compiled in $(BUILD_DIR)/"

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all
