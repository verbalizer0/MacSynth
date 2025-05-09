// File: src/Utils/PixelateEffect.cpp
#include "PixelateEffect.h"

PixelateEffect::PixelateEffect() : Effect("pixelate") {
    // Initialize parameters with defaults
    ensureParameter("sizeX", 16);
    ensureParameter("sizeY", 16);
    ensureParameter("dynamicSize", 1.0); // Boolean as float (1.0 = true)
    ensureParameter("threshold", 0.5);
}

PixelateEffect::~PixelateEffect() {
    // Clean up resources
}

void PixelateEffect::setup(int width, int height) {
    Effect::setup(width, height);
    
    // Initialize buffer FBO
    bufferFbo.allocate(width, height, GL_RGBA);
    
    // Clear buffer
    bufferFbo.begin();
    ofClear(0, 0, 0, 0);
    bufferFbo.end();
    
    // Load pixelate shader
    if (!pixelateShader.isLoaded()) {
        bool loaded = pixelateShader.load("shaders/pixelate");
        if (!loaded) {
            ofLogError("PixelateEffect") << "Failed to load pixelate shader";
        } else {
            ofLogNotice("PixelateEffect") << "Pixelate shader loaded successfully";
        }
    }
}

void PixelateEffect::update(float phase, float* audioData, int numBands, map<string, float>& globalParams) {
    // Apply global parameter scaling
    if (globalParams.find("pixelate") != globalParams.end()) {
        // Scale pixelate parameters by global parameter
        float pixelateMultiplier = globalParams["pixelate"];
        if (pixelateMultiplier < 1.0) {
            // Scale down for higher resolution
            params["sizeX"] *= pixelateMultiplier;
            params["sizeY"] *= pixelateMultiplier;
        } else {
            // Scale up for lower resolution
            params["sizeX"] *= pixelateMultiplier;
            params["sizeY"] *= pixelateMultiplier;
        }
    }
    
    // Apply audio reactivity if dynamic size is enabled
    if (params["dynamicSize"] > 0.5 && audioData != nullptr && numBands > 0) {
        // Calculate energy in mid-high frequency range
        float sum = 0.0f;
        int startBand = numBands / 5;
        int endBand = numBands / 2;
        
        for (int i = startBand; i < endBand && i < numBands; i++) {
            sum += audioData[i];
        }
        
        float energy = sum / (endBand - startBand);
        
        // Modulate pixel size with audio energy
        float minSize = 2.0;
        float maxSize = 32.0;
        float newSize = minSize + (maxSize - minSize) * energy;
        
        // Smooth parameter changes
        params["sizeX"] = params["sizeX"] * 0.8 + newSize * 0.2;
        params["sizeY"] = params["sizeY"] * 0.8 + newSize * 0.2;
    }
    
    // Ensure minimum pixel size
    params["sizeX"] = std::max(1.0f, params["sizeX"]);
    params["sizeY"] = std::max(1.0f, params["sizeY"]);
}

void PixelateEffect::apply(ofFbo& inputFbo) {
    // Skip if intensity is zero
    if (intensity <= 0.0) {
        return;
    }
    
    // Apply pixelation effect
    bufferFbo.begin();
    ofClear(0, 0, 0, 0);
    
    // Use shader if available
    if (pixelateShader.isLoaded()) {
        // Apply pixelate shader
        pixelateShader.begin();
        
        // Set shader parameters
        pixelateShader.setUniform1f("sizeX", params["sizeX"] * intensity);
        pixelateShader.setUniform1f("sizeY", params["sizeY"] * intensity);
        pixelateShader.setUniform1f("threshold", params["threshold"]);
        pixelateShader.setUniform2f("resolution", width, height);
        
        // Draw input using shader
        inputFbo.draw(0, 0);
        
        pixelateShader.end();
    } else {
        // Fallback to CPU-based pixelation if shader not available
        // This is less efficient but ensures the effect still works
        
        // Get pixels from input FBO
        ofPixels inputPixels;
        inputFbo.readToPixels(inputPixels);
        
        // Calculate pixel blocks
        int pixelSizeX = std::max(1, (int)(params["sizeX"] * intensity));
        int pixelSizeY = std::max(1, (int)(params["sizeY"] * intensity));
        
        // Draw pixelated version
        ofSetColor(255);
        
        for (int y = 0; y < height; y += pixelSizeY) {
            for (int x = 0; x < width; x += pixelSizeX) {
                // Get color from center of block
                int sampleX = x + pixelSizeX / 2;
                int sampleY = y + pixelSizeY / 2;
                
                // Ensure within bounds
                sampleX = std::min(sampleX, width - 1);
                sampleY = std::min(sampleY, height - 1);
                
                ofColor color = inputPixels.getColor(sampleX, sampleY);
                
                // Apply threshold if enabled
                if (params["threshold"] < 1.0) {
                    float brightness = color.getBrightness() / 255.0f;
                    if (brightness < params["threshold"]) {
                        color = ofColor(0, 0, 0, color.a);
                    }
                }
                
                ofSetColor(color);
                ofDrawRectangle(x, y, pixelSizeX, pixelSizeY);
            }
        }
    }
    
    bufferFbo.end();
    
    // Draw result to input FBO
    ofPushStyle();
    ofSetColor(255, 255, 255, 255);
    ofEnableAlphaBlending();
    
    inputFbo.begin();
    bufferFbo.draw(0, 0);
    inputFbo.end();
    
    ofPopStyle();
}