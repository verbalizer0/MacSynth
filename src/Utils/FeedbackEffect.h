// File: src/Utils/FeedbackEffect.h
#pragma once

#include "Effect.h"

class FeedbackEffect : public Effect {
public:
    FeedbackEffect();
    virtual ~FeedbackEffect();
    
    // Setup the effect
    void setup(int width, int height) override;
    
    // Update the effect
    void update(float phase, float* audioData, int numBands, map<string, float>& globalParams) override;
    
    // Apply the effect to an input FBO
    void apply(ofFbo& inputFbo) override;
    
private:
    // Feedback buffers
    ofFbo bufferFbo;
    ofFbo tempFbo;
    
    // Shader for feedback effects
    ofShader feedbackShader;
};