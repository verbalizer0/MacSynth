// File: src/Utils/Sprite.h
#pragma once

#include "ofMain.h"

enum MotionType {
    MOTION_NONE,
    MOTION_LINEAR,
    MOTION_CIRCULAR,
    MOTION_BOUNCE,
    MOTION_WAVE,
    MOTION_FOLLOW
};

class Sprite {
public:
    Sprite();
    virtual ~Sprite();
    
    // Setup the sprite
    virtual void setup(float x, float y, float scale, float rotation);
    
    // Update the sprite
    virtual void update(float deltaTime, float* audioData, int numBands);
    
    // Draw the sprite
    virtual void draw(int canvasWidth, int canvasHeight);
    
    // Get sprite type
    virtual string getType() = 0;
    
    // Getters and setters
    string getId() { return id; }
    void setId(string id) { this->id = id; }
    
    float getX() { return x; }
    void setX(float x) { this->x = x; }
    
    float getY() { return y; }
    void setY(float y) { this->y = y; }
    
    float getScale() { return scale; }
    void setScale(float scale) { this->scale = scale; }
    
    float getRotation() { return rotation; }
    void setRotation(float rotation) { this->rotation = rotation; }
    
    ofVec2f getMotionSpeed() { return motionSpeed; }
    void setMotionSpeed(ofVec2f speed) { this->motionSpeed = speed; }
    void setMotionSpeed(float amount) { 
        this->motionSpeed.x *= amount / motionAmount; 
        this->motionSpeed.y *= amount / motionAmount;
        this->motionAmount = amount;
    }
    
    float getRotationSpeed() { return rotationSpeed; }
    void setRotationSpeed(float speed) { this->rotationSpeed = speed; }
    
    MotionType getMotionType() { return motionType; }
    void setMotionType(MotionType type) { this->motionType = type; }
    
    float getOpacity() { return opacity; }
    void setOpacity(float opacity) { this->opacity = opacity; }
    
    int getMaxTrailLength() { return maxTrailLength; }
    void setMaxTrailLength(int length) { this->maxTrailLength = length; }
    
    float getAudioReactivity() { return audioReactivity; }
    void setAudioReactivity(float reactivity) { this->audioReactivity = reactivity; }
    
protected:
    // Sprite properties
    string id;
    float x;
    float y;
    float scale;
    float rotation;
    ofVec2f motionSpeed;
    float rotationSpeed;
    MotionType motionType;
    float opacity;
    float motionAmount;
    
    // Trail properties
    struct TrailPoint {
        float x;
        float y;
        float scale;
        float rotation;
        float opacity;
    };
    vector<TrailPoint> trail;
    int maxTrailLength;
    
    // Motion parameters for different types
    float circleRadius;
    float circlePhase;
    float waveAmplitudeX;
    float waveAmplitudeY;
    float waveFrequencyX;
    float waveFrequencyY;
    float wavePhaseX;
    float wavePhaseY;
    ofVec2f basePosition;
    
    // Audio reactivity
    float audioReactivity;
    string reactsTo; // "bass", "mid", "high", "all"
    
    // Utility to get audio energy in a frequency range
    float getAudioEnergy(float* audioData, int numBands, string range);
    
    // Apply audio reactivity
    virtual void applyAudioReactivity(float* audioData, int numBands);
    
    // Apply different motion types
    void applyMotion(float deltaTime);
    void applyCircularMotion(float deltaTime);
    void applyBounceMotion(float deltaTime);
    void applyWaveMotion(float deltaTime);
    
    // Draw trail
    virtual void drawTrail(int canvasWidth, int canvasHeight);
};

class BasicSprite : public Sprite {
public:
    BasicSprite();
    virtual ~BasicSprite();
    
    // Setup with color
    void setup(float x, float y, float scale, float rotation, ofColor color);
    
    // Draw implementation
    void draw(int canvasWidth, int canvasHeight) override;
    
    // Get sprite type
    string getType() override { return "basic"; }
    
    // Color getter and setter
    ofColor getColor() { return color; }
    void setColor(ofColor color) { this->color = color; }
    
protected:
    ofColor color;
    
    // Draw trail implementation
    void drawTrail(int canvasWidth, int canvasHeight) override;
};

class GifSprite : public Sprite {
public:
    GifSprite();
    virtual ~GifSprite();
    
    // Setup with GIF path
    void setup(string path, float x, float y, float scale, float rotation);
    
    // Update with animation
    void update(float deltaTime, float* audioData, int numBands) override;
    
    // Draw implementation
    void draw(int canvasWidth, int canvasHeight) override;
    
    // Get sprite type
    string getType() override { return "gif"; }
    
    // Path getter
    string getPath() { return path; }
    
protected:
    string path;
    ofImage image;
    
    // Animation properties
    bool isAnimated;
    vector<ofImage> frames;
    vector<float> frameDurations;
    int currentFrame;
    float frameTime;
    bool isPlaying;
    
    // Draw trail implementation
    void drawTrail(int canvasWidth, int canvasHeight) override;
    
    // Load GIF frames
    bool loadGif(string path);
    
    // Play/pause animation
    void play() { isPlaying = true; }
    void pause() { isPlaying = false; }
    
    // Set specific frame
    void setFrame(int frame);
};