# Makefile for MacSynth Video Synthesizer

# Include the openFrameworks common makefile
include $(OF_ROOT)/Users/michall/openFrameworks/libs/openFrameworksCompiled/project/makefileCommon/config.make

# App name and source files
APPNAME = MacSynth

# The source code
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
          src/Utils/GlitchEffect.cpp \
          src/Utils/StrobeEffect.cpp \
          src/Utils/WaveEffect.cpp \
          src/Utils/Sprite.cpp \
          src/Utils/SpriteLibrary.cpp \
          src/UI/GUI.cpp

# Include search paths
CFLAGS += -I src/Layers \
          -I src/Utils \
          -I src/UI

# Add these to your CFLAGS
CFLAGS += -I$(OF_ROOT)/addons/ofxImGui/src \
          -I$(OF_ROOT)/addons/ofxImGui/libs/imgui/src

# Link in required addons
OF_ADDONS_ACTIVE = ofxGif ofxFft ofxImGui

# Include the openFrameworks makefile
include $(OF_ROOT)/Users/michall/openFrameworks/libs/openFrameworksCompiled/project/makefileCommon/compile.project.mk

# Custom rules for this project
.PHONY: run clean

# Run the application
run: $(TARGET_NAME)
	@echo Running $(APPNAME)
	@./$(TARGET_NAME)

# Clean build files
clean:
	@echo Cleaning $(APPNAME)
	rm -rf obj bin
	
# Create directories if not exist
create_dirs:
	@mkdir -p src/Layers
	@mkdir -p src/Utils
	@mkdir -p src/UI
	@mkdir -p bin/data/Sprites