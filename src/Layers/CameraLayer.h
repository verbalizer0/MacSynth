// File: src/Layers/CameraLayer.h
#pragma once

#include "ofMain.h"

class CameraLayer {
public:
    CameraLayer();
    ~CameraLayer();
    
    void setup(int width, int height);
    void update(float deltaTime, float* audioData, int numBands, float phase);
    void draw();
    
    // Get output FBO
    ofFbo& getOutputFbo() { return outputFbo; }
    
    // Save and load presets
    void savePreset(ofXml& xml);
    void loadPreset(ofXml& xml);
    
    // Camera control
    bool setupCamera(int deviceId = 0);
    void setActive(bool active) { this->active = active; }
    bool isActive() { return active; }
    
    // Feedback control
    void setFeedbackEnabled(bool enabled) { feedbackEnabled = enabled; }
    bool isFeedbackEnabled() { return feedbackEnabled; }
    
    // Set parameters
    void setX(float x) { this->x = x; }
    void setY(float y) { this->y = y; }
    void setScale(float scale) { this->scale = scale; }
    void setRotation(float rotation) { this->rotation = rotation; }
    void setOpacity(float opacity) { this->opacity = opacity; }
    void setMirror(bool mirror) { this->mirror = mirror; }
    
    // Chroma key settings
    void setChromaKey(bool enabled) { chromaKeyEnabled = enabled; }
    void setChromaColor(ofColor color) { chromaColor = color; }
    void setChromaTolerance(float tolerance) { chromaTolerance = tolerance; }
    
    // Get parameters
    float getX() { return x; }
    float getY() { return y; }
    float getScale() { return scale; }
    float getRotation() { return rotation; }
    float getOpacity() { return opacity; }
    bool getMirror() { return mirror; }
    
    // Set parameter by name
    bool setParameter(string name, float value);
    
private:
    int width, height;
    
    // Camera settings
    ofVideoGrabber camera;
    bool active;
    bool feedbackEnabled;
    
    // Position and transform
    float x;
    float y;
    float scale;
    float rotation;
    float opacity;
    bool mirror;
    
    // Chroma key settings
    bool chromaKeyEnabled;
    ofColor chromaColor;
    float chromaTolerance;
    
    // Shader for chroma keying
    ofShader chromaKeyShader;
    
    // FBO for rendering
    ofFbo outputFbo;
    
    // Apply chroma key effect
    void applyChromaKey(ofTexture& texture);
    
    // Apply audio reactivity
    void applyAudioReactivity(float* audioData, int numBands);
};