// File: src/Utils/PixelateEffect.h
#pragma once

#include "Effect.h"

class PixelateEffect : public Effect {
public:
    PixelateEffect();
    virtual ~PixelateEffect();
    
    // Setup the effect
    void setup(int width, int height) override;
    
    // Update the effect
    void update(float phase, float* audioData, int numBands, map<string, float>& globalParams) override;
    
    // Apply the effect to an input FBO
    void apply(ofFbo& inputFbo) override;
    
private:
    // Shader for pixelation
    ofShader pixelateShader;
    
    // Buffer for processing
    ofFbo bufferFbo;
};