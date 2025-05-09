// File: src/Utils/Sprite.cpp
#include "Sprite.h"

//--------------------------------------------------------------
// Base Sprite Implementation
//--------------------------------------------------------------

Sprite::Sprite() {
    // Default properties
    id = "";
    x = 0.5;
    y = 0.5;
    scale = 1.0;
    rotation = 0.0;
    
    motionSpeed = ofVec2f(0, 0);
    rotationSpeed = 0.0;
    motionType = MOTION_NONE;
    opacity = 1.0;
    motionAmount = 1.0;
    
    maxTrailLength = 0;
    
    // Motion parameters
    circleRadius = 0.1;
    circlePhase = 0.0;
    waveAmplitudeX = 0.1;
    waveAmplitudeY = 0.1;
    waveFrequencyX = 1.0;
    waveFrequencyY = 1.0;
    wavePhaseX = 0.0;
    wavePhaseY = PI / 2.0; // 90 degrees offset
    basePosition = ofVec2f(0.5, 0.5);
    
    // Audio reactivity
    audioReactivity = 0.5;
    reactsTo = "all";
}

Sprite::~Sprite() {
    // Clean up resources
}

void Sprite::setup(float x, float y, float scale, float rotation) {
    this->x = x;
    this->y = y;
    this->scale = scale;
    this->rotation = rotation;
    
    // Save base position for motion patterns
    basePosition = ofVec2f(x, y);
}

void Sprite::update(float deltaTime, float* audioData, int numBands) {
    // Store previous position for trail
    if (maxTrailLength > 0) {
        TrailPoint point;
        point.x = x;
        point.y = y;
        point.scale = scale;
        point.rotation = rotation;
        point.opacity = opacity;
        
        trail.insert(trail.begin(), point);
        
        // Trim trail to max length
        if (trail.size() > maxTrailLength) {
            trail.resize(maxTrailLength);
        }
    }
    
    // Apply audio reactivity if available
    if (audioData != nullptr && numBands > 0) {
        applyAudioReactivity(audioData, numBands);
    }
    
    // Apply motion based on type
    applyMotion(deltaTime);
    
    // Update rotation
    rotation += rotationSpeed * deltaTime;
    
    // Keep rotation in range [0, TWO_PI]
    rotation = fmodf(rotation, TWO_PI);
}

void Sprite::draw(int canvasWidth, int canvasHeight) {
    // Draw trail if enabled
    if (maxTrailLength > 0 && trail.size() > 0) {
        drawTrail(canvasWidth, canvasHeight);
    }
    
    // Base class doesn't draw anything else
}

float Sprite::getAudioEnergy(float* audioData, int numBands, string range) {
    if (audioData == nullptr || numBands <= 0) {
        return 0.0;
    }
    
    int startBin = 0;
    int endBin = numBands - 1;
    
    // Map frequency ranges to different parts of the spectrum
    if (range == "bass") {
        endBin = numBands / 4;
    } else if (range == "mid") {
        startBin = numBands / 4;
        endBin = numBands * 3 / 4;
    } else if (range == "high") {
        startBin = numBands * 3 / 4;
    }
    
    // Calculate energy in the selected range
    float sum = 0.0;
    for (int i = startBin; i <= endBin; i++) {
        sum += audioData[i];
    }
    
    // Normalize by number of bands
    return sum / (endBin - startBin + 1);
}

void Sprite::applyAudioReactivity(float* audioData, int numBands) {
    if (audioReactivity <= 0.0) return;
    
    // Get energy in the frequency range this sprite reacts to
    float energy = getAudioEnergy(audioData, numBands, reactsTo);
    
    // Apply energy to sprite properties based on reactivity
    float impact = energy * audioReactivity;
    
    // Scale up temporarily with audio energy
    scale = scale * 0.9 + (scale * (1.0 + impact)) * 0.1;
    
    // Increase rotation speed with audio
    rotationSpeed += impact * 0.1;
    
    // Optionally modify other properties based on audio
    // opacity = 0.5 + impact * 0.5;
}

void Sprite::applyMotion(float deltaTime) {
    // Apply different motion types
    switch (motionType) {
        case MOTION_LINEAR:
            // Linear motion - update position based on speed
            x += motionSpeed.x * deltaTime;
            y += motionSpeed.y * deltaTime;
            
            // Wrap around edges
            if (x < 0) x = 1;
            if (x > 1) x = 0;
            if (y < 0) y = 1;
            if (y > 1) y = 0;
            break;
            
        case MOTION_CIRCULAR:
            applyCircularMotion(deltaTime);
            break;
            
        case MOTION_BOUNCE:
            applyBounceMotion(deltaTime);
            break;
            
        case MOTION_WAVE:
            applyWaveMotion(deltaTime);
            break;
            
        case MOTION_FOLLOW:
            // Follow motion would require a target
            // Not implemented in base class
            break;
            
        case MOTION_NONE:
        default:
            // No motion
            break;
    }
}

void Sprite::applyCircularMotion(float deltaTime) {
    // Update circle phase
    circlePhase += deltaTime * motionAmount;
    
    // Calculate new position on circle
    x = basePosition.x + cos(circlePhase) * circleRadius;
    y = basePosition.y + sin(circlePhase) * circleRadius;
    
    // Optional: rotate to face movement direction
    rotation = circlePhase + PI/2;
}

void Sprite::applyBounceMotion(float deltaTime) {
    // Move according to current speed
    x += motionSpeed.x * deltaTime * motionAmount;
    y += motionSpeed.y * deltaTime * motionAmount;
    
    // Check for bounce on edges
    if (x <= 0 || x >= 1) {
        motionSpeed.x = -motionSpeed.x;
        
        // Ensure we're not stuck at the edge
        if (x <= 0) x = 0.01;
        if (x >= 1) x = 0.99;
    }
    
    if (y <= 0 || y >= 1) {
        motionSpeed.y = -motionSpeed.y;
        
        // Ensure we're not stuck at the edge
        if (y <= 0) y = 0.01;
        if (y >= 1) y = 0.99;
    }
}

void Sprite::applyWaveMotion(float deltaTime) {
    // Update wave phases
    wavePhaseX += deltaTime * waveFrequencyX * motionAmount;
    wavePhaseY += deltaTime * waveFrequencyY * motionAmount;
    
    // Calculate wave movement
    float waveX = cos(wavePhaseX) * waveAmplitudeX;
    float waveY = sin(wavePhaseY) * waveAmplitudeY;
    
    // Apply to base position
    x = basePosition.x + waveX;
    y = basePosition.y + waveY;
    
    // Optional: rotate to follow wave direction
    float dx = -sin(wavePhaseX) * waveAmplitudeX * waveFrequencyX;
    float dy = cos(wavePhaseY) * waveAmplitudeY * waveFrequencyY;
    rotation = atan2(dy, dx);
}

void Sprite::drawTrail(int canvasWidth, int canvasHeight) {
    // Base class doesn't draw anything
}

//--------------------------------------------------------------
// Basic Sprite Implementation
//--------------------------------------------------------------

BasicSprite::BasicSprite() : Sprite() {
    // Default color
    color = ofColor(255, 255, 255);
}

BasicSprite::~BasicSprite() {
    // Clean up resources
}

void BasicSprite::setup(float x, float y, float scale, float rotation, ofColor color) {
    // Call base setup
    Sprite::setup(x, y, scale, rotation);
    
    // Set color
    this->color = color;
    
    // Set default motion parameters
    motionSpeed = ofVec2f((ofRandom(0, 1) - 0.5) * 0.1, (ofRandom(0, 1) - 0.5) * 0.1);
    rotationSpeed = ofRandom(-0.5, 0.5);
    motionType = MOTION_LINEAR;
    
    // Random audio reactivity
    float reactType = ofRandom(0, 3);
    if (reactType < 1) {
        reactsTo = "bass";
    } else if (reactType < 2) {
        reactsTo = "mid";
    } else {
        reactsTo = "high";
    }
}

void BasicSprite::draw(int canvasWidth, int canvasHeight) {
    // Draw trail first
    Sprite::draw(canvasWidth, canvasHeight);
    
    ofPushMatrix();
    ofPushStyle();
    
    // Convert normalized coordinates to pixels
    float pixelX = x * canvasWidth;
    float pixelY = y * canvasHeight;
    
    // Apply transformations
    ofTranslate(pixelX, pixelY);
    ofRotateZDeg(ofRadToDeg(rotation));
    ofScale(scale, scale);
    
    // Set color and opacity
    ofSetColor(color, opacity * 255);
    
    // Draw a simple shape
    ofDrawCircle(0, 0, 20);
    
    ofPopStyle();
    ofPopMatrix();
}

void BasicSprite::drawTrail(int canvasWidth, int canvasHeight) {
    ofPushStyle();
    
    // Draw trail with decreasing opacity
    for (int i = 0; i < trail.size(); i++) {
        float trailOpacity = trail[i].opacity * (1.0 - (float)i / trail.size());
        
        // Convert normalized coordinates to pixels
        float pixelX = trail[i].x * canvasWidth;
        float pixelY = trail[i].y * canvasHeight;
        
        ofPushMatrix();
        
        // Apply transformations
        ofTranslate(pixelX, pixelY);
        ofRotateZDeg(ofRadToDeg(trail[i].rotation));
        ofScale(trail[i].scale, trail[i].scale);
        
        // Set color with trail opacity
        ofSetColor(color, trailOpacity * 128);
        
        // Draw a smaller circle for trail
        ofDrawCircle(0, 0, 15);
        
        ofPopMatrix();
    }
    
    ofPopStyle();
}

//--------------------------------------------------------------
// GIF Sprite Implementation
//--------------------------------------------------------------

GifSprite::GifSprite() : Sprite() {
    path = "";
    isAnimated = false;
    currentFrame = 0;
    frameTime = 0;
    isPlaying = true;
}

GifSprite::~GifSprite() {
    // Clean up resources
    frames.clear();
}

void GifSprite::setup(string path, float x, float y, float scale, float rotation) {
    // Call base setup
    Sprite::setup(x, y, scale, rotation);
    
    // Set path and load GIF
    this->path = path;
    
    // Try to load as animated GIF
    if (!loadGif(path)) {
        // Fall back to single image
        image.load(path);
        isAnimated = false;
    }
    
    // Set default motion parameters
    motionSpeed = ofVec2f((ofRandom(0, 1) - 0.5) * 0.1, (ofRandom(0, 1) - 0.5) * 0.1);
    rotationSpeed = ofRandom(-0.2, 0.2);
    motionType = MOTION_LINEAR;
    
    // Random audio reactivity
    float reactType = ofRandom(0, 3);
    if (reactType < 1) {
        reactsTo = "bass";
    } else if (reactType < 2) {
        reactsTo = "mid";
    } else {
        reactsTo = "high";
    }
}

void GifSprite::update(float deltaTime, float* audioData, int numBands) {
    // Call base update
    Sprite::update(deltaTime, audioData, numBands);
    
    // Update animation if animated
    if (isAnimated && isPlaying && frames.size() > 0) {
        frameTime += deltaTime;
        
        // Check if it's time to advance to next frame
        float frameDuration = (currentFrame < frameDurations.size()) ? 
                             frameDurations[currentFrame] : 0.1; // Default 100ms
        
        if (frameTime >= frameDuration) {
            // Advance to next frame
            currentFrame = (currentFrame + 1) % frames.size();
            frameTime = 0;
        }
    }
}

void GifSprite::draw(int canvasWidth, int canvasHeight) {
    // Draw trail first
    Sprite::draw(canvasWidth, canvasHeight);
    
    ofPushMatrix();
    ofPushStyle();
    
    // Convert normalized coordinates to pixels
    float pixelX = x * canvasWidth;
    float pixelY = y * canvasHeight;
    
    // Apply transformations
    ofTranslate(pixelX, pixelY);
    ofRotateZDeg(ofRadToDeg(rotation));
    ofScale(scale, scale);
    
    // Set opacity
    ofSetColor(255, 255, 255, opacity * 255);
    
    // Draw current frame
    if (isAnimated && frames.size() > 0) {
        // Draw current animation frame
        ofImage& frame = frames[currentFrame];
        frame.draw(-frame.getWidth() / 2, -frame.getHeight() / 2);
    } else if (image.isAllocated()) {
        // Draw static image
        image.draw(-image.getWidth() / 2, -image.getHeight() / 2);
    }
    
    ofPopStyle();
    ofPopMatrix();
}

void GifSprite::drawTrail(int canvasWidth, int canvasHeight) {
    ofPushStyle();
    
    // Draw trail with decreasing opacity
    for (int i = 0; i < trail.size(); i++) {
        float trailOpacity = trail[i].opacity * (1.0 - (float)i / trail.size());
        
        // Convert normalized coordinates to pixels
        float pixelX = trail[i].x * canvasWidth;
        float pixelY = trail[i].y * canvasHeight;
        
        ofPushMatrix();
        
        // Apply transformations
        ofTranslate(pixelX, pixelY);
        ofRotateZDeg(ofRadToDeg(trail[i].rotation));
        ofScale(trail[i].scale * 0.8, trail[i].scale * 0.8); // Slightly smaller for trail
        
        // Set opacity for trail
        ofSetColor(255, 255, 255, trailOpacity * 128);
        
        // Draw the image for the trail
        if (isAnimated && frames.size() > 0) {
            // Use a consistent frame for the trail (e.g., frame 0)
            ofImage& frame = frames[0];
            frame.draw(-frame.getWidth() / 2, -frame.getHeight() / 2);
        } else if (image.isAllocated()) {
            image.draw(-image.getWidth() / 2, -image.getHeight() / 2);
        }
        
        ofPopMatrix();
    }
    
    ofPopStyle();
}

bool GifSprite::loadGif(string path) {
    // In a real implementation, this would use ofxGif or similar addon
    // Since we don't have that in this example, we'll simulate it
    
    // Check if file exists
    if (!ofFile::doesFileExist(path)) {
        ofLogError("GifSprite") << "File not found: " << path;
        return false;
    }
    
    // For this example, we'll just load a single image
    // and pretend we have multiple frames for demo purposes
    
    ofImage tempImage;
    bool loaded = tempImage.load(path);
    
    if (!loaded) {
        ofLogError("GifSprite") << "Failed to load image: " << path;
        return false;
    }
    
    // Simulate multiple frames
    isAnimated = true;
    frames.clear();
    frameDurations.clear();
    
    // Create 4 frames (in a real implementation, these would be actual GIF frames)
    for (int i = 0; i < 4; i++) {
        frames.push_back(tempImage);
        frameDurations.push_back(0.1); // 100ms per frame
    }
    
    currentFrame = 0;
    frameTime = 0;
    
    return true;
}

void GifSprite::setFrame(int frame) {
    if (isAnimated && frame >= 0 && frame < frames.size()) {
        currentFrame = frame;
        frameTime = 0;
    }
}