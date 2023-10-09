INCLUDE = -I"../../include" -I"/usr/include/x86_64-linux-gnu/qt6"
LIBRARIES = -lQt6Widgets -lQt6Core -lQt6Gui

CXX = g++
FLAGS = -fPIC -Wall

OBJ_DIR = build
LIB_DIR = ../../library

LIBS = $(wildcard $(LIB_DIR)/*.cpp)
OBJS = $(patsubst $(LIB_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(LIBS))

all: $(OBJS) main.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) $^ -o ./${OBJ_DIR}/main $(INCLUDE) $(LIBRARIES)

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) -c $< -o $@ $(INCLUDE) $(LIBRARIES)

run:
	./$(OBJ_DIR)/main

clean:
	rm -r $(OBJ_DIR)