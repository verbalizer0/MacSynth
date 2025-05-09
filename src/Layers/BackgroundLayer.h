// File: src/Layers/BackgroundLayer.h
#pragma once

#include "ofMain.h"

class BackgroundLayer {
public:
    BackgroundLayer();
    ~BackgroundLayer();
    
    void setup(int width, int height);
    void update(float deltaTime, float* audioData, int numBands, float phase);
    void draw();
    
    // Set feedback texture from camera
    void setFeedbackTexture(ofPixels& pixels);
    
    // Get output FBO
    ofFbo& getOutputFbo() { return outputFbo; }
    
    // Save and load presets
    void savePreset(ofXml& xml);
    void loadPreset(ofXml& xml);
    
    // Background source types
    enum SourceType {
        COLOR,
        VIDEO,
        CAMERA,
        PATTERN
    };
    
    // Pattern types
    enum PatternType {
        GRADIENT,
        BARS,
        CIRCLES,
        NOISE
    };
    
    // Set background source type
    void setSourceType(SourceType type);
    SourceType getSourceType() const { return sourceType; }
    
    // Set color parameters
    void setColorStart(ofColor color) { colorStart = color; }
    void setColorEnd(ofColor color) { colorEnd = color; }
    void setGradientType(string type) { gradientType = type; }
    
    // Set feedback parameters
    void setFeedbackAmount(float amount) { feedbackAmount = amount; }
    void setFeedbackZoom(float zoom) { feedbackZoom = zoom; }
    void setFeedbackRotate(float rotate) { feedbackRotate = rotate; }
    void setColorShift(float shift) { colorShift = shift; }
    
    // Set video source
    void setVideoSource(string path);
    
    // Set camera source
    void setCameraSource(ofVideoGrabber& camera);
    
    // Set pattern parameters
    void setPatternType(PatternType type) { patternType = type; }
    void setPatternSpeed(float speed) { patternSpeed = speed; }
    void setPatternDensity(float density) { patternDensity = density; }
    
    // Getter methods for private members
    float getFeedbackAmount() const { return feedbackAmount; }
    float getFeedbackZoom() const { return feedbackZoom; }
    float getFeedbackRotate() const { return feedbackRotate; }
    float getColorShift() const { return colorShift; }
    PatternType getPatternType() const { return patternType; }
    float getPatternSpeed() const { return patternSpeed; }
    float getPatternDensity() const { return patternDensity; }
    ofColor getColorStart() const { return colorStart; }
    ofColor getColorEnd() const { return colorEnd; }
    string getGradientType() const { return gradientType; }
    
private:
    int width, height;
    
    // Source type and parameters
    SourceType sourceType;
    
    // Color parameters
    ofColor colorStart;
    ofColor colorEnd;
    string gradientType; // "none", "linear", "radial"
    
    // Video parameters
    ofVideoPlayer videoPlayer;
    
    // Camera parameters
    ofVideoGrabber* cameraSource;
    
    // Pattern parameters
    PatternType patternType;
    float patternSpeed;
    float patternDensity;
    float patternTime;
    
    // Feedback parameters
    float feedbackAmount;
    float feedbackZoom;
    float feedbackRotate;
    float colorShift;
    
    // FBOs for rendering
    ofFbo outputFbo;
    ofFbo feedbackFbo;
    
    // Feedback texture
    ofTexture feedbackTexture;
    bool hasFeedbackTexture;
    
    // Render methods for different sources
    void renderColorBackground();
    void renderVideoBackground();
    void renderCameraBackground();
    void renderPatternBackground(float phase);
    
    // Pattern-specific render methods
    void renderGradientPattern(float phase);
    void renderBarsPattern(float phase);
    void renderCirclesPattern(float phase);
    void renderNoisePattern();
    
    // Apply feedback effect
    void applyFeedback();
    
    // Apply audio reactivity
    void applyAudioReactivity(float* audioData, int numBands);
};