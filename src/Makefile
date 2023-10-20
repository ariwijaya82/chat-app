WEBOTS_HOME = /usr/local/webots

INCLUDE = -I"/usr/include/x86_64-linux-gnu/qt6" \
					-I"../include"
LIBRARIES = -lQt6Core -lQt6Widgets -lQt6Gui -lQt6WebSockets

CXX = g++
FLAGS = -fPIC -Wall

OBJ_DIR = build
LIB_DIR = ../library

LIBS = $(wildcard $(LIB_DIR)/*.cpp)
OBJS = $(patsubst $(LIB_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(LIBS))

SOURCES = $(wildcard *.cpp)

all: $(OBJS) $(SOURCES)
	mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) $^ -o ./${OBJ_DIR}/main $(INCLUDE) $(LIBRARIES)

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) -c $< -o $@ $(INCLUDE) $(LIBRARIES)

run:
	./$(OBJ_DIR)/main

clean:
	rm -r $(OBJ_DIR)