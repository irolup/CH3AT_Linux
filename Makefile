# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -Ivendor/imgui -Ivendor/imgui/backends -Ivendor/rapidxml

# Output binary name
TARGET = CH3AT

# Source files
SRCS = main.cpp patch_helpers.cpp process_helpers.cpp \
	   table.cpp \
       vendor/imgui/imgui.cpp \
       vendor/imgui/imgui_draw.cpp \
       vendor/imgui/imgui_tables.cpp \
       vendor/imgui/imgui_widgets.cpp \
       vendor/imgui/backends/imgui_impl_glfw.cpp \
       vendor/imgui/backends/imgui_impl_opengl3.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Libraries to link
LIBS = -lglfw -lGL

# Default target
all: $(TARGET)

# Link the object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run