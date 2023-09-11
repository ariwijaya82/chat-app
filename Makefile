WEBOTS_HOME = /usr/local/webots
RESOURCES_PATH = $(WEBOTS_HOME)/projects/robots/robotis/darwin-op

INCLUDE = -I"$(RESOURCES_PATH)/libraries/managers/include" \
		  -I"$(RESOURCES_PATH)/libraries/robotis-op2/robotis/Framework/include" \
		  -I"$(WEBOTS_HOME)/include/controller/cpp" \
		  -I"/usr/include/x86_64-linux-gnu/qt6" \
		  -I"./include"
LIBRARIES = -L"$(RESOURCES_PATH)/libraries/robotis-op2" -lrobotis-op2 \
			-L"$(RESOURCES_PATH)/libraries/managers" -lmanagers \
			-L"$(WEBOTS_HOME)/lib/controller" -lCppController -lController \
			-lQt6Widgets -lQt6Core -lQt6Gui

SRC_DIR = library
OBJ_DIR = build

CXX = g++
FLAGS = -fPIC -Wall
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

all: navigation standing testing

navigation: $(OBJ) src/navigation.cpp
	mkdir -p build
	$(CXX) $(FLAGS) $^ -o ./build/navigation $(INCLUDE) $(LIBRARIES)

standing: $(OBJ) src/standing.cpp
	$(CXX) $(FLAGS) $^ -o ./webots_ws/controllers/standing/standing $(INCLUDE) $(LIBRARIES)

testing: $(OBJ) src/testing.cpp
	$(CXX) $(FLAGS) $^ -o ./build/testing $(INCLUDE) $(LIBRARIES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p build webots_ws/controllers/standing
	$(CXX) $(FLAGS) -c $< -o $@ $(INCLUDE) $(LIBRARIES)

run:
	./build/navigation

test:
	./build/testing

clean:
	rm -r build webots_ws/controllers