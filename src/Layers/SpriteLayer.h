// File: src/Layers/SpriteLayer.h
#pragma once

#include "ofMain.h"
#include "../Utils/Sprite.h"

class SpriteLayer {
public:
    SpriteLayer();
    ~SpriteLayer();
    
    void setup(int width, int height);
    void update(float deltaTime, float* audioData, int numBands);
    void draw();
    
    // Get output FBO
    ofFbo& getOutputFbo() { return outputFbo; }
    
    // Save and load presets
    void savePreset(ofXml& xml);
    void loadPreset(ofXml& xml);
    
    // Add a sprite to the layer
    void addSprite(Sprite* sprite);
    
    // Remove a sprite by ID
    void removeSprite(string id);
    
    // Clear all sprites
    void clearSprites();
    
    // Get all sprites
    vector<Sprite*>& getSprites() { return sprites; }
    
    // Get sprite count
    int getSpriteCount() { return sprites.size(); }
    
    // Set parameters
    void setDensity(int density) { this->density = density; }
    void setMaxTrailLength(int length);
    void setSpriteScale(float scale);
    void setMotionAmount(float amount);
    void setBlendMode(string mode) { blendMode = mode; }
    void setAudioReactivity(float reactivity) { audioReactivity = reactivity; }
    
    // Get parameters
    int getDensity() { return density; }
    int getMaxTrailLength() { return maxTrailLength; }
    float getSpriteScale() { return spriteScale; }
    float getMotionAmount() { return motionAmount; }
    string getBlendMode() { return blendMode; }
    float getAudioReactivity() { return audioReactivity; }
    
private:
    int width, height;
    
    // Sprites
    vector<Sprite*> sprites;
    
    // Sprite properties
    int density;
    int maxTrailLength;
    float spriteScale;
    float motionAmount;
    string blendMode;
    float audioReactivity;
    
    // FBO for rendering
    ofFbo outputFbo;
    
    // Maintain proper sprite density
    void maintainDensity();
    
    // Currently available sprite IDs to avoid duplicates
    std::set<string> usedIds;
    
    // Generate unique sprite ID
    string generateSpriteId();
};