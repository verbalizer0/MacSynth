# Simple Direct Makefile for MacSynth

CXX = g++
CXXFLAGS = -Wall -std=c++11 -DofIndexType="unsigned short"
CFLAGS += -I/path/to/directory/containing/tesselator.h
CFLAGS += -I$(OF_ROOT)/libs/libtess2/include

# OpenFrameworks Path
OF_PATH = /Users/michall/openFrameworks

# Include paths
INCLUDES = -I$(OF_PATH)/libs/openFrameworks \
           -I$(OF_PATH)/libs/openFrameworks/utils \
           -I$(OF_PATH)/libs/openFrameworks/communication \
           -I$(OF_PATH)/libs/openFrameworks/app \
           -I$(OF_PATH)/libs/openFrameworks/events \
           -I$(OF_PATH)/libs/openFrameworks/graphics \
           -I$(OF_PATH)/libs/openFrameworks/math \
           -I$(OF_PATH)/libs/openFrameworks/types \
           -I$(OF_PATH)/libs/openFrameworks/sound \
           -I$(OF_PATH)/libs/openFrameworks/gl \
           -I$(OF_PATH)/libs/openFrameworks/3d \
           -I$(OF_PATH)/libs/openFrameworks/video \
           -I$(OF_PATH)/addons/ofxGif/src \
           -I$(OF_PATH)/addons/ofxFft/src \
           -I$(OF_PATH)/addons/ofxImGui/src \
           -I$(OF_PATH)/addons/ofxImGui/libs/imgui/src \
           -I./src \
           -I./src/Layers \
           -I./src/Utils \
           -I./src/UI


# Source files
SOURCES = src/main.cpp \
          src/ofApp.cpp \
          src/Layers/BackgroundLayer.cpp \
          src/Layers/SpriteLayer.cpp \
          src/Layers/FXLayer.cpp \
          src/Layers/CameraLayer.cpp \
          src/Utils/AudioAnalyzer.cpp \
          src/Utils/Effect.cpp \
          src/Utils/FeedbackEffect.cpp \
          src/Utils/PixelateEffect.cpp \
          src/Utils/Sprite.cpp \
          src/Utils/SpriteLibrary.cpp \
          src/UI/GUI.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# OpenFrameworks libraries
LIBS = -L$(OF_PATH)/libs/openFrameworksCompiled/lib/osx \
       -lopenFrameworks \
       -framework Cocoa \
       -framework OpenGL \
       -framework CoreFoundation

# Output binary
BIN = bin/MacSynth

# Default target
all: $(BIN)

# Linking
$(BIN): $(OBJECTS)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Compilation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
	rm -f $(OBJECTS) $(BIN)

.PHONY: all clean