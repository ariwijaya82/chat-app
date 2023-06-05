WEBOTS_HOME = /usr/local/webots
RESOURCES_PATH = $(WEBOTS_HOME)/projects/robots/robotis/darwin-op

INCLUDE = -I"$(RESOURCES_PATH)/libraries/managers/include" \
		  -I"$(RESOURCES_PATH)/libraries/robotis-op2/robotis/Framework/include" \
		  -I"$(WEBOTS_HOME)/include/controller/cpp"
LIBRARIES = -L"$(RESOURCES_PATH)/libraries/robotis-op2" -lrobotis-op2 \
			-L"$(RESOURCES_PATH)/libraries/managers" -lmanagers \
			-L"$(WEBOTS_HOME)/lib/controller" -lCppController -lController

CXX_SOURCES = main.cpp
TARGET = custom_walk

all: $(CXX_SOURCES)
	g++ -o $(TARGET) $(CXX_SOURCES) $(INCLUDE) $(LIBRARIES)

plot: plot.cpp
	g++ -o plot plot.cpp -I"/usr/include/python3.10" \
		-I"../../../.env/lib/python3.9/site-packages/numpy/core/include" \
		-I"/usr/include/eigen3"
		-lpython3.9
