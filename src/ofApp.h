// File: src/ofApp.h
#pragma once

#include "ofMain.h"
#include "Layers/BackgroundLayer.h"
#include "Layers/SpriteLayer.h"
#include "Layers/FXLayer.h"
#include "Layers/CameraLayer.h"
#include "Utils/AudioAnalyzer.h"
#include "UI/GUI.h"

class ofApp : public ofBaseApp{

public:
    void setup() override;
    void update() override;
    void draw() override;
    void exit() override;

    void keyPressed(int key) override;
    void keyReleased(int key) override;
    void mouseMoved(int x, int y ) override;
    void mouseDragged(int x, int y, int button) override;
    void mousePressed(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
    void mouseEntered(int x, int y) override;
    void mouseExited(int x, int y) override;
    void windowResized(int w, int h) override;
    void dragEvent(ofDragInfo dragInfo) override;
    void gotMessage(ofMessage msg) override;
    
    // Core components
    BackgroundLayer backgroundLayer;
    SpriteLayer spriteLayer;
    FXLayer fxLayer;
    CameraLayer cameraLayer;
    AudioAnalyzer audioAnalyzer;
    
    // GUI
    GUI* gui; // Uncomment when GUI class is implemented
    
    // Main canvas dimensions
    int canvasWidth;
    int canvasHeight;
    
    // Current scene
    int currentScene;
    bool debugMode;
    bool playing;
    
    // FBOs for composite rendering
    ofFbo mainFbo;
    ofFbo finalFbo;
    
    // Update parameters from GUI
    void updateParameters();
    
    // Scene management
    void saveScene(int sceneIndex);
    void loadScene(int sceneIndex);
};