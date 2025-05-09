// File: src/UI/GUI.h
#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "../Layers/BackgroundLayer.h"
#include "../Layers/SpriteLayer.h"
#include "../Layers/FXLayer.h"
#include "../Layers/CameraLayer.h"
#include "../Utils/AudioAnalyzer.h"

class ofApp;

class GUI {
public:
    GUI();
    ~GUI();
    
    void setup(ofApp* app);
    void update();
    void draw();
    
    // Get selected tab
    string getSelectedTab() { return currentTab; }
    
private:
    ofApp* app;
    ofxImGui::Gui gui;
    
    // ImGui state
    string currentTab;
    bool showAudioPanel;
    bool showTemplatePanel;
    
    // Tabs
    void drawBackgroundTab();
    void drawSpriteTab();
    void drawFXTab();
    void drawCameraTab();
    void drawTempoTab();
    
    // Audio panel
    void drawAudioPanel();
    
    // Scene selector
    void drawSceneSelector();
    
    // Helper methods
    void drawSlider(string label, float* value, float min, float max);
    void drawColorEdit(string label, ofColor* color);
    
    // Temporary storage for GUI values
    struct {
        float feedbackAmount;
        float feedbackZoom;
        float feedbackRotate;
        float colorShift;
        int patternType;
        float patternSpeed;
        float patternDensity;
        ofColor colorStart;
        ofColor colorEnd;
        string gradientType;
        int sourceType;
    } backgroundParams;
    
    struct {
        int density;
        int maxTrailLength;
        float spriteScale;
        float motionAmount;
        string blendMode;
        float audioReactivity;
    } spriteParams;
    
    struct {
        map<string, bool> effectsEnabled;
        map<string, float> effectsIntensity;
        map<string, map<string, float>> effectParams;
    } fxParams;
    
    struct {
        bool active;
        bool feedbackEnabled;
        float x;
        float y;
        float scale;
        float rotation;
        float opacity;
        bool mirror;
        bool chromaKeyEnabled;
        ofColor chromaColor;
        float chromaTolerance;
    } cameraParams;
    
    struct {
        float gain;
        int device;
        string clockSource;
        float bpm;
    } audioParams;
};