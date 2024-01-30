CXX = g++
FLAGS = -fPIC -Wall

INCLUDE = -I"/usr/include/x86_64-linux-gnu/qt6" -I"./include"
LIBRARY = -lQt6Core -lQt6Widgets -lQt6Gui -lQt6WebSockets

SRC_DIR = src
LIB_DIR = library
OBJ_DIR = build
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
LIBS = $(wildcard $(LIB_DIR)/*.cpp)
OBJS = $(patsubst $(LIB_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(LIBS))

all: $(OBJS) $(SRCS)
	mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) $^ -o ./${OBJ_DIR}/main $(INCLUDE) $(LIBRARY)

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) -c $< -o $@ $(INCLUDE) $(LIBRARY)

run:
	./$(OBJ_DIR)/main

clean:
	rm -r $(OBJ_DIR)