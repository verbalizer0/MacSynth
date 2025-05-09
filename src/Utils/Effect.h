// File: src/Utils/Effect.h
#pragma once

#include "ofMain.h"

class Effect {
public:
    Effect(string name);
    virtual ~Effect();
    
    // Initialize the effect
    virtual void setup(int width, int height);
    
    // Update the effect parameters
    virtual void update(float phase, float* audioData, int numBands, map<string, float>& globalParams);
    
    // Apply the effect to an input FBO
    virtual void apply(ofFbo& inputFbo) = 0;
    
    // Get effect name
    string getName() { return name; }
    
    // Enabled status
    bool isEnabled() { return enabled; }
    void setEnabled(bool enabled) { this->enabled = enabled; }
    
    // Effect intensity (0-1)
    float getIntensity() { return intensity; }
    void setIntensity(float intensity) { this->intensity = ofClamp(intensity, 0, 1); }
    
    // Set parameter value
    virtual bool setParameter(string name, float value);
    
    // Get parameter value
    virtual float getParameter(string name);
    
    // Save/load preset
    virtual void savePreset(ofXml& xml);
    virtual void loadPreset(ofXml& xml);
    
protected:
    string name;
    bool enabled;
    float intensity;
    
    int width, height;
    
    // Effect parameters
    map<string, float> params;
    
    // Create parameter if it doesn't exist
    void ensureParameter(string name, float defaultValue);
    
    // Utility to get audio energy in a frequency range
    float getAudioEnergy(float* audioData, int numBands, string range);
};