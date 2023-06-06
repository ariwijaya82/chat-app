WEBOTS_HOME = /usr/local/webots
RESOURCES_PATH = $(WEBOTS_HOME)/projects/robots/robotis/darwin-op

INCLUDE = -I"$(RESOURCES_PATH)/libraries/managers/include" \
		  -I"$(RESOURCES_PATH)/libraries/robotis-op2/robotis/Framework/include" \
		  -I"$(WEBOTS_HOME)/include/controller/cpp" \
		  -I"$(WEBOTS_HOME)/include/qt/QtWidgets" \
		  -I"$(WEBOTS_HOME)/include/qt/QtCore" \
		  -I"$(WEBOTS_HOME)/include/qt/QtGui"
LIBRARIES = -L"$(RESOURCES_PATH)/libraries/robotis-op2" -lrobotis-op2 \
			-L"$(RESOURCES_PATH)/libraries/managers" -lmanagers \
			-L"$(WEBOTS_HOME)/lib/controller" -lCppController -lController \
			-L"$(WEBOTS_HOME)/lib/webots" -lQt6Core -lQt6Widgets -lQt6Gui

CXX_SOURCES = main.cpp
TARGET = custom_walk

all: $(CXX_SOURCES)
	g++ -fPIC -o $(TARGET) $(CXX_SOURCES) $(INCLUDE) $(LIBRARIES)

test: test.cpp
	g++ -fPIC -o test test.cpp \
		-I"/usr/include/x86_64-linux-gnu/qt6" \
		-lQt6Core -lQt6Widgets -lQt6Gui
