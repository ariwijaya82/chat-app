INCLUDE = -I"/usr/include/x86_64-linux-gnu/qt6" \
					-I"../include" -I"./include"
LIBRARIES = -lQt6Core -lQt6Widgets -lQt6Gui -lQt6WebSockets

CXX = g++
FLAGS = -fPIC -Wall

OBJ_DIR = build

LIB_DIR1 = ../library
LIBS1 = $(wildcard $(LIB_DIR1)/*.cpp)
OBJS1 = $(patsubst $(LIB_DIR1)/%.cpp, $(OBJ_DIR)/%.o, $(LIBS1))

LIB_DIR2 = ./library
LIBS2 = $(wildcard $(LIB_DIR2)/*.cpp)
OBJS2 = $(patsubst $(LIB_DIR2)/%.cpp, $(OBJ_DIR)/%.o, $(LIBS2))

SOURCES =  main.cpp

all: $(OBJS1) $(OBJS2) $(SOURCES)
	mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) $^ -o ./${OBJ_DIR}/main $(INCLUDE) $(LIBRARIES)

$(OBJ_DIR)/%.o: $(LIB_DIR1)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) -c $< -o $@ $(INCLUDE) $(LIBRARIES)

$(OBJ_DIR)/%.o: $(LIB_DIR2)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) -c $< -o $@ $(INCLUDE) $(LIBRARIES)

run:
	./$(OBJ_DIR)/main

clean:
	rm -r $(OBJ_DIR)