CXX      := g++
CC       := gcc
CXXFLAGS := -std=c++20 -Wall -Wextra -Ibackend/include -Ibackend/DB
CFLAGS   := -O2

LDFLAGS  := -lws2_32 -lmswsock
TARGET   := backend_server.exe

SRC_DIR  := backend/src
OBJ_DIR  := obj

CPPSOURCES := $(wildcard $(SRC_DIR)/*.cpp)
CSOURCES   := $(wildcard $(SRC_DIR)/*.c)

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CPPSOURCES))
OBJECTS += $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(CSOURCES))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir $(OBJ_DIR) 2>nul || mkdir -p $(OBJ_DIR) 2>/dev/null || true

clean:
	rm -rf $(OBJ_DIR) $(TARGET)