// File: src/Utils/FeedbackEffect.cpp
#include "FeedbackEffect.h"

FeedbackEffect::FeedbackEffect() : Effect("feedback") {
    // Initialize parameters with defaults
    ensureParameter("amount", 0.5);
    ensureParameter("zoom", 1.01);
    ensureParameter("rotate", 0.002);
    ensureParameter("offsetX", 0);
    ensureParameter("offsetY", 0);
    ensureParameter("hueShift", 0);
    ensureParameter("fade", 0.1);
}

FeedbackEffect::~FeedbackEffect() {
    // Clean up resources
}

void FeedbackEffect::setup(int width, int height) {
    Effect::setup(width, height);
    
    // Initialize FBOs
    bufferFbo.allocate(width, height, GL_RGBA);
    tempFbo.allocate(width, height, GL_RGBA);
    
    // Clear FBOs
    bufferFbo.begin();
    ofClear(0, 0, 0, 0);
    bufferFbo.end();
    
    tempFbo.begin();
    ofClear(0, 0, 0, 0);
    tempFbo.end();
    
    // Load feedback shader if needed
    // In a real implementation, this would load actual shader files
    // For this example, we'll use built-in OpenGL functionality
    /*
    if (!feedbackShader.isLoaded()) {
        feedbackShader.load("shaders/feedback");
    }
    */
}

void FeedbackEffect::update(float phase, float* audioData, int numBands, map<string, float>& globalParams) {
    // Apply global parameter scaling
    if (globalParams.find("feedback") != globalParams.end()) {
        // Scale feedback amount by global parameter
        float feedbackMultiplier = globalParams["feedback"];
        params["amount"] = params["amount"] * feedbackMultiplier;
    }
    
    // Apply audio reactivity
    if (audioData != nullptr && numBands > 0) {
        // Get bass energy for feedback amount
        float bassEnergy = getAudioEnergy(audioData, numBands, "bass");
        params["amount"] = ofLerp(params["amount"], bassEnergy * 0.8, 0.1);
        
        // Get mid energy for rotation
        float midEnergy = getAudioEnergy(audioData, numBands, "mid");
        params["rotate"] += (midEnergy - 0.5) * 0.001;
        
        // Ensure rotation stays in reasonable range
        params["rotate"] = ofClamp(params["rotate"], -0.1, 0.1);
    }
}

void FeedbackEffect::apply(ofFbo& inputFbo) {
    // Skip if amount is zero
    if (params["amount"] <= 0 || intensity <= 0) {
        inputFbo.draw(0, 0);
        return;
    }
    
    // Save current frame to temp buffer
    tempFbo.begin();
    ofClear(0, 0, 0, 0);
    inputFbo.draw(0, 0);
    tempFbo.end();
    
    // Apply feedback
    float effectiveAmount = params["amount"] * intensity;
    
    // Draw feedback buffer with transform
    ofPushMatrix();
    ofPushStyle();
    
    // Apply blend mode
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
    // Apply feedback opacity
    ofSetColor(255, 255, 255, 255 * effectiveAmount);
    
    // Apply feedback transform
    ofTranslate(width / 2, height / 2);
    ofRotateZDeg(params["rotate"] * 360.0);
    ofScale(params["zoom"], params["zoom"]);
    ofTranslate(-width / 2 + params["offsetX"], -height / 2 + params["offsetY"]);
    
    // Apply color shift if enabled
    if (params["hueShift"] != 0) {
        // In a real implementation, this would use the shader
        // For this example, we'll just simulate color shifting
        ofSetColor(255, 255, 255, 255 * effectiveAmount);
    }
    
    // Draw buffer content
    bufferFbo.draw(0, 0);
    
    ofPopStyle();
    ofPopMatrix();
    
    // Draw original content on top
    ofSetColor(255, 255, 255, 255);
    inputFbo.draw(0, 0);
    
    // Update buffer for next frame
    bufferFbo.begin();
    ofClear(0, 0, 0, 0);
    
    // Fade out buffer for next frame
    ofSetColor(255, 255, 255, 255 * (1.0 - params["fade"]));
    tempFbo.draw(0, 0);
    
    bufferFbo.end();
}