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

FLAGS = -fPIC

LIB_SRC = $(wildcard lib/*.cpp)

all: navigation standing testing

navigation: $(LIB_SRC) src/navigation.cpp
	mkdir -p build
	g++ $(FLAGS) $^ -o ./build/navigation $(INCLUDE) $(LIBRARIES)

standing: $(LIB_SRC) src/standing.cpp
	mkdir -p webots_ws/controllers/standing
	g++ $(FLAGS) $^ -o ./webots_ws/controllers/standing/standing $(INCLUDE) $(LIBRARIES)

testing: $(LIB_SRC) src/testing.cpp
	mkdir -p build
	g++ $(FLAGS) $^ -o ./build/testing $(INCLUDE) $(LIBRARIES)

run:
	./build/navigation

test:
	./build/testing

clean:
	rm -r build webots_ws/controllers