INCLUDE_PATHS := -Iexternal/include -I. -Iwrappers/
LIB_PATHS := -Lexternal/libs
BUILD_PATH := build
FLAGS := -lglfw3 -lX11 -lpthread -lXrandr -lXi -ldl

# Files
HELLO_WORLD := hello_world/hello_world
HELLO_TRIANGLE := \
	hello_triangle/hello_triangle \
	hello_triangle/exercise1 \
	hello_triangle/exercise2 \
SHADERS := \
	shaders/shader_exercise1

%: %.cpp
	$(CXX) -o $@  $< wrappers/*.cpp glad.c stb_image.cpp $(FLAGS) $(INCLUDE_PATHS) $(LIB_PATHS)
	(cd $(dir $@); ./$(notdir $@))

clean:
	find . -maxdepth 2 -type f -executable -exec rm {} +
