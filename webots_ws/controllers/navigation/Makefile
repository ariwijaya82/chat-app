WEBOTS_HOME = /usr/local/webots
RESOURCES_PATH = $(WEBOTS_HOME)/projects/robots/robotis/darwin-op

INCLUDE = -I"$(RESOURCES_PATH)/libraries/managers/include" \
		  -I"$(RESOURCES_PATH)/libraries/robotis-op2/robotis/Framework/include" \
		  -I"$(WEBOTS_HOME)/include/controller/cpp" \
		  -I"/usr/include/x86_64-linux-gnu/qt6" \
		  -I"./libraries"
LIBRARIES = -L"$(RESOURCES_PATH)/libraries/robotis-op2" -lrobotis-op2 \
			-L"$(RESOURCES_PATH)/libraries/managers" -lmanagers \
			-L"$(WEBOTS_HOME)/lib/controller" -lCppController -lController \
			-lQt6Widgets -lQt6Core -lQt6Gui

CXX_SOURCES = $(wildcard libraries/*.cpp) main.cpp
TARGET = navigation

all: $(CXX_SOURCES)
	g++ -fPIC -o $(TARGET) $(CXX_SOURCES) $(INCLUDE) $(LIBRARIES)

run:
	./$(TARGET)

clean:
	rm $(TARGET)