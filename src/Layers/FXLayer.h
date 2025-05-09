// File: src/Layers/FXLayer.h
#pragma once

#include "ofMain.h"
#include "Effect.h"
#include "PixelateEffect.h"

class FXLayer {
public:
    FXLayer();
    ~FXLayer();
    
    void setup(int width, int height);
    void update(float phase, float* audioData, int numBands);
    
    // Process an input FBO with all active effects
    void process(ofFbo& inputFbo);
    
    // Get output FBO
    ofFbo& getOutputFbo() { return outputFbo; }
    
    // Effect management
    void addEffect(Effect* effect);
    void removeEffect(string name);
    Effect* getEffect(string name);
    bool hasEffect(string name);
    
    // Get effects map
    map<string, Effect*>& getEffects() { return effects; }
    
    // Enable/disable effect
    void enableEffect(string name, bool enabled);
    
    // Set effect parameter
    void setEffectParameter(string effectName, string paramName, float value);
    
    // Set global parameters that affect all effects
    void setGlobalParam(string name, float value);
    
private:
    int width, height;
    
    // FBOs for rendering
    ofFbo outputFbo;
    ofFbo tempFbo;
    
    // Effects
    map<string, Effect*> effects;
    
    // Global parameters
    map<string, float> globalParams;
    
    // Initialize default effects
    void initializeDefaultEffects();
};