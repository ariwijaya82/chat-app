SRC = main.cpp plot.cpp
TARGET = main
INCLUDE = -I /usr/include/x86_64-linux-gnu/qt6
LIBS = -lQt6Widgets -lQt6Core -lQt6Gui

all:
	g++ -fPIC -o $(TARGET) $(SRC) $(INCLUDE) $(LIBS)

run:
	./$(TARGET)

clean:
	rm $(TARGET)