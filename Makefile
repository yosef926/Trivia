CXX      := g++
CC       := gcc
CXXFLAGS := -std=c++20 -Wall -Wextra -Ibackend/include -Ibackend/DB
CFLAGS   := -O2

LDFLAGS  := -lws2_32 -lmswsock
TARGET   := backend_server.exe
FRONTEND := frontend/TriviaClient.csproj

SRC_DIR  := backend/src
OBJ_DIR  := obj

CPPSOURCES := $(wildcard $(SRC_DIR)/*.cpp)
CSOURCES   := $(wildcard $(SRC_DIR)/*.c)

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CPPSOURCES))
OBJECTS += $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(CSOURCES))

.PHONY: all backend frontend clean

all: backend frontend

backend: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir $(OBJ_DIR) 2>nul || mkdir -p $(OBJ_DIR) 2>/dev/null || true

frontend:
	dotnet build $(FRONTEND) -c Release

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	dotnet clean $(FRONTEND) 2>/dev/null || true