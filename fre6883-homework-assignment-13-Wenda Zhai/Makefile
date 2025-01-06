CXX = g++
CXXFLAGS = -std=c++17 -Wall
LDFLAGS = -lcurl

CXXFLAGS += -pthread
LDFLAGS += -pthread

EXE_NAME = HW13
TARGET = $(EXE_NAME)

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)

HEADERS = $(wildcard *.h)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "[INFO] Linking object files to create the executable..."
	@$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "[SUCCESS] Build complete! Executable created at: $(TARGET)"

%.o: %.cpp $(HEADERS)
	@echo "[INFO] Compiling $< -> $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "[INFO] Cleaning up build files..."
	@rm -f $(OBJECTS) $(TARGET)
	@echo "[SUCCESS] Clean complete!"

run: all
	@echo "[INFO] Running the program..."
	@./$(TARGET)

.PHONY: all clean run
