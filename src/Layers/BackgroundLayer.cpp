// File: src/Layers/BackgroundLayer.cpp
#include "BackgroundLayer.h"

BackgroundLayer::BackgroundLayer() {
    width = 1280;
    height = 720;
    
    // Default parameters
    sourceType = COLOR;
    
    colorStart = ofColor(0, 0, 0);
    colorEnd = ofColor(50, 0, 50);
    gradientType = "radial";
    
    feedbackAmount = 0.0;
    feedbackZoom = 1.02;
    feedbackRotate = 0.001;
    colorShift = 0.0;
    
    patternType = GRADIENT;
    patternSpeed = 1.0;
    patternDensity = 5.0;
    patternTime = 0.0;
    
    cameraSource = nullptr;
    hasFeedbackTexture = false;
}

BackgroundLayer::~BackgroundLayer() {
    // Clean up resources
}

void BackgroundLayer::setup(int width, int height) {
    this->width = width;
    this->height = height;
    
    // Set up FBOs
    outputFbo.allocate(width, height, GL_RGBA);
    feedbackFbo.allocate(width, height, GL_RGBA);
    
    // Clear FBOs
    outputFbo.begin();
    ofClear(0, 0, 0, 0);
    outputFbo.end();
    
    feedbackFbo.begin();
    ofClear(0, 0, 0, 0);
    feedbackFbo.end();
}

void BackgroundLayer::update(float deltaTime, float* audioData, int numBands, float phase) {
    // Update pattern time
    patternTime += deltaTime * patternSpeed;
    
    // Update video if playing
    if (sourceType == VIDEO && videoPlayer.isLoaded()) {
        videoPlayer.update();
    }
    
    // Apply audio reactivity
    if (audioData != nullptr) {
        applyAudioReactivity(audioData, numBands);
    }
}

void BackgroundLayer::draw() {
    outputFbo.begin();
    ofClear(0, 0, 0, 255);
    
    // Apply feedback if enabled
    if (feedbackAmount > 0.0) {
        applyFeedback();
    }
    
    // Render based on source type
    switch (sourceType) {
        case COLOR:
            renderColorBackground();
            break;
        case VIDEO:
            renderVideoBackground();
            break;
        case CAMERA:
            renderCameraBackground();
            break;
        case PATTERN:
            renderPatternBackground(patternTime);
            break;
    }
    
    outputFbo.end();
}

void BackgroundLayer::setSourceType(SourceType type) {
    sourceType = type;
}

void BackgroundLayer::setVideoSource(string path) {
    videoPlayer.load(path);
    videoPlayer.setLoopState(OF_LOOP_NORMAL);
    videoPlayer.play();
    sourceType = VIDEO;
}

void BackgroundLayer::setCameraSource(ofVideoGrabber& camera) {
    cameraSource = &camera;
    sourceType = CAMERA;
}

void BackgroundLayer::setFeedbackTexture(ofPixels& pixels) {
    if (!feedbackTexture.isAllocated()) {
        feedbackTexture.allocate(pixels);
    } else {
        feedbackTexture.loadData(pixels);
    }
    hasFeedbackTexture = true;
}

void BackgroundLayer::renderColorBackground() {
    ofPushStyle();
    
    // Determine if gradient or solid color
    if (gradientType == "none" || colorStart == colorEnd) {
        // Solid color
        ofSetColor(colorStart);
        ofDrawRectangle(0, 0, width, height);
    } else {
        // Create gradient mesh
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        
        if (gradientType == "linear") {
            // Linear gradient from top to bottom
            mesh.addVertex(ofVec3f(0, 0, 0));
            mesh.addVertex(ofVec3f(width, 0, 0));
            mesh.addVertex(ofVec3f(0, height, 0));
            mesh.addVertex(ofVec3f(width, height, 0));
            
            mesh.addColor(colorStart);
            mesh.addColor(colorStart);
            mesh.addColor(colorEnd);
            mesh.addColor(colorEnd);
        } else if (gradientType == "radial") {
            // Radial gradient from center
            int numSegments = 32;
            float centerX = width / 2;
            float centerY = height / 2;
            float radius = sqrt(width * width + height * height) / 2;
            
            // Add center vertex
            mesh.addVertex(ofVec3f(centerX, centerY, 0));
            mesh.addColor(colorStart);
            
            // Add outer vertices
            for (int i = 0; i <= numSegments; i++) {
                float angle = TWO_PI * i / numSegments;
                float x = centerX + radius * cos(angle);
                float y = centerY + radius * sin(angle);
                
                mesh.addVertex(ofVec3f(x, y, 0));
                mesh.addColor(colorEnd);
            }
            
            // Set to triangle fan mode
            mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        }
        
        // Draw gradient mesh
        mesh.draw();
    }
    
    ofPopStyle();
}

void BackgroundLayer::renderVideoBackground() {
    ofPushStyle();
    
    if (videoPlayer.isLoaded() && videoPlayer.isPlaying()) {
        // Calculate dimensions to maintain aspect ratio
        float videoRatio = (float)videoPlayer.getWidth() / videoPlayer.getHeight();
        float screenRatio = (float)width / height;
        
        float drawWidth, drawHeight, x, y;
        
        if (videoRatio > screenRatio) {
            // Video is wider than canvas - fit height
            drawHeight = height;
            drawWidth = drawHeight * videoRatio;
            x = (width - drawWidth) / 2;
            y = 0;
        } else {
            // Video is taller than canvas - fit width
            drawWidth = width;
            drawHeight = drawWidth / videoRatio;
            x = 0;
            y = (height - drawHeight) / 2;
        }
        
        // Draw video frame
        ofSetColor(255);
        videoPlayer.draw(x, y, drawWidth, drawHeight);
    } else {
        // Video not ready, draw black background
        ofSetColor(0);
        ofDrawRectangle(0, 0, width, height);
    }
    
    ofPopStyle();
}

void BackgroundLayer::renderCameraBackground() {
    ofPushStyle();
    
    if (cameraSource != nullptr && cameraSource->isInitialized()) {
        // Calculate dimensions to maintain aspect ratio
        float cameraRatio = (float)cameraSource->getWidth() / cameraSource->getHeight();
        float screenRatio = (float)width / height;
        
        float drawWidth, drawHeight, x, y;
        
        if (cameraRatio > screenRatio) {
            // Camera is wider than canvas - fit height
            drawHeight = height;
            drawWidth = drawHeight * cameraRatio;
            x = (width - drawWidth) / 2;
            y = 0;
        } else {
            // Camera is taller than canvas - fit width
            drawWidth = width;
            drawHeight = drawWidth / cameraRatio;
            x = 0;
            y = (height - drawHeight) / 2;
        }
        
        // Draw camera frame
        ofSetColor(255);
        cameraSource->draw(x, y, drawWidth, drawHeight);
    } else {
        // Camera not ready, draw black background
        ofSetColor(0);
        ofDrawRectangle(0, 0, width, height);
    }
    
    ofPopStyle();
}

void BackgroundLayer::renderPatternBackground(float phase) {
    // Render different patterns based on type
    switch (patternType) {
        case GRADIENT:
            renderGradientPattern(phase);
            break;
        case BARS:
            renderBarsPattern(phase);
            break;
        case CIRCLES:
            renderCirclesPattern(phase);
            break;
        case NOISE:
            renderNoisePattern();
            break;
    }
}

void BackgroundLayer::renderGradientPattern(float phase) {
    ofPushStyle();
    
    // Calculate shifting hues
    float baseHue = fmodf(phase * 50.0, 360.0);
    float hue1 = baseHue;
    float hue2 = fmodf(baseHue + 120.0, 360.0);
    float hue3 = fmodf(baseHue + 240.0, 360.0);
    
    // Create shifting colors
    ofColor color1 = ofColor::fromHsb(hue1, 255, 200);
    ofColor color2 = ofColor::fromHsb(hue2, 255, 200);
    ofColor color3 = ofColor::fromHsb(hue3, 255, 200);
    
    // Create gradient mesh
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    // Linear gradient from top left to bottom right
    mesh.addVertex(ofVec3f(0, 0, 0));
    mesh.addVertex(ofVec3f(width, 0, 0));
    mesh.addVertex(ofVec3f(0, height, 0));
    mesh.addVertex(ofVec3f(width, height, 0));
    
    mesh.addColor(color1);
    mesh.addColor(color2);
    mesh.addColor(color3);
    mesh.addColor(color1);
    
    // Draw gradient mesh
    mesh.draw();
    
    ofPopStyle();
}

void BackgroundLayer::renderBarsPattern(float phase) {
    ofPushStyle();
    
    int barCount = max(2, (int)(patternDensity * 10.0));
    float barHeight = height / barCount;
    
    for (int i = 0; i < barCount; i++) {
        // Calculate bar position with phase offset
        float y = (i * barHeight + phase * barHeight * 2) - fmodf(phase * barHeight * 2, height);
        
        // Calculate color based on position
        float hue = fmodf(colorShift + i * (360.0f / barCount), 360.0f);
        ofColor color = ofColor::fromHsb(hue, 255, 200);
        ofSetColor(color);
        
        // Draw bar
        ofDrawRectangle(0, y, width, barHeight);
    }
    
    ofPopStyle();
}

void BackgroundLayer::renderCirclesPattern(float phase) {
    ofPushStyle();
    
    ofBackground(0);
    
    int circleCount = max(1, (int)(patternDensity * 5.0));
    float baseRadius = 50.0 + 30.0 * sin(phase * TWO_PI);
    
    for (int i = 0; i < circleCount; i++) {
        // Calculate circle position based on index
        float x = width * (0.2 + 0.6 * (i % 3) / 2.0);
        float y = height * (0.2 + 0.6 * floor(i / 3.0) / 2.0);
        
        // Calculate radius with variation
        float radius = baseRadius * (0.5 + sin(i + phase * TWO_PI) * 0.5);
        
        // Calculate color based on position and time
        float hue = fmodf(colorShift + i * (360.0f / circleCount), 360.0f);
        
        // Create gradient for each circle
        ofColor centerColor = ofColor::fromHsb(hue, 255, 200, 200);
        ofColor edgeColor = ofColor::fromHsb(hue, 255, 200, 0);
        
        // Draw circle with gradient
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        
        // Center vertex
        mesh.addVertex(ofVec3f(x, y, 0));
        mesh.addColor(centerColor);
        
        // Edge vertices
        int segments = 32;
        for (int j = 0; j <= segments; j++) {
            float angle = TWO_PI * j / segments;
            float px = x + radius * cos(angle);
            float py = y + radius * sin(angle);
            mesh.addVertex(ofVec3f(px, py, 0));
            mesh.addColor(edgeColor);
        }
        
        mesh.draw();
    }
    
    ofPopStyle();
}

void BackgroundLayer::renderNoisePattern() {
    ofPushStyle();
    
    // Create noise with perlin
    ofPixels pixels;
    pixels.allocate(width, height, OF_PIXELS_RGB);
    
    // Base hue for color shifting
    float baseHue = colorShift;
    
    // Generate noise pixels
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Simple noise function
            float noise = ofNoise(x * 0.005 * patternDensity, y * 0.005 * patternDensity, patternTime * 0.1);
            
            // Apply noise pattern - convert to HSL for better control
            float hue = fmodf(baseHue + noise * 60.0, 360.0);
            float saturation = 0.8;
            float lightness = 0.1 + noise * 0.3;
            
            // Convert HSL to RGB
            ofColor color = ofColor::fromHsb(hue, saturation * 255, lightness * 255);
            
            // Set pixel color
            pixels.setColor(x, y, color);
        }
    }
    
    // Draw noise pattern
    ofTexture tex;
    tex.loadData(pixels);
    tex.draw(0, 0);
    
    ofPopStyle();
}

void BackgroundLayer::applyFeedback() {
    if (feedbackAmount <= 0.0) return;
    
    // Save current output to feedback buffer
    feedbackFbo.begin();
    outputFbo.draw(0, 0);
    feedbackFbo.end();
    
    ofPushMatrix();
    ofPushStyle();
    
    // Set blend mode and alpha
    ofEnableAlphaBlending();
    ofSetColor(255, 255, 255, 255 * feedbackAmount);
    
    // Apply transformations for feedback
    ofTranslate(width / 2, height / 2);
    ofRotateZDeg(feedbackRotate * 360.0);
    ofScale(feedbackZoom, feedbackZoom);
    ofTranslate(-width / 2, -height / 2);
    
    // Apply color shift if needed
    static ofShader feedbackShader;
    if (colorShift != 0.0) {
        // Load shader if not already loaded
        if (!feedbackShader.isLoaded()) {
            bool loaded = feedbackShader.load("shaders/feedback");
            if (!loaded) {
                ofLogError("BackgroundLayer") << "Failed to load feedback shader";
            }
        }
        
        // Begin shader
        feedbackShader.begin();
        feedbackShader.setUniform1f("colorShift", colorShift / 360.0); // Normalize to 0-1 range
    }
    
    // Draw previous buffer content
    feedbackFbo.draw(0, 0);
    
    // End shader if used
    if (colorShift != 0.0) {
        feedbackShader.end();
    }
    
    // Draw camera feedback if available
    if (hasFeedbackTexture) {
        ofSetColor(255, 255, 255, 100);
        feedbackTexture.draw(0, 0, width, height);
    }
    
    ofPopStyle();
    ofPopMatrix();
}

void BackgroundLayer::applyAudioReactivity(float* audioData, int numBands) {
    if (audioData == nullptr || numBands <= 0) return;
    
    // Calculate bass energy (first 1/4 of bands)
    float bassEnergy = 0.0;
    int bassBands = numBands / 4;
    for (int i = 0; i < bassBands; i++) {
        bassEnergy += audioData[i];
    }
    bassEnergy /= bassBands;
    
    // Calculate mid energy (middle 1/2 of bands)
    float midEnergy = 0.0;
    int midBands = numBands / 2;
    for (int i = numBands / 4; i < numBands * 3 / 4; i++) {
        midEnergy += audioData[i];
    }
    midEnergy /= midBands;
    
    // Modulate feedback parameters based on audio
    feedbackAmount = ofLerp(feedbackAmount, bassEnergy * 0.8, 0.1);
    colorShift = fmodf(colorShift + midEnergy * 2.0, 360.0);
    
    // Modulate pattern parameters based on audio
    patternSpeed = ofLerp(patternSpeed, 1.0 + bassEnergy * 2.0, 0.1);
}

// Fixed savePreset method for BackgroundLayer.cpp
void BackgroundLayer::savePreset(ofXml& xml) {
    // Save source type
    ofXml sourceTypeNode;
    sourceTypeNode.set(ofToString((int)sourceType));
    xml.appendChild("sourceType").set(ofToString((int)sourceType));
    
    // Save color parameters
    ofXml colorXml;
    colorXml.appendChild("colorStart_r").set(ofToString(colorStart.r));
    colorXml.appendChild("colorStart_g").set(ofToString(colorStart.g));
    colorXml.appendChild("colorStart_b").set(ofToString(colorStart.b));
    colorXml.appendChild("colorEnd_r").set(ofToString(colorEnd.r));
    colorXml.appendChild("colorEnd_g").set(ofToString(colorEnd.g));
    colorXml.appendChild("colorEnd_b").set(ofToString(colorEnd.b));
    colorXml.appendChild("gradientType").set(gradientType);
    xml.appendChild("color").appendChild(colorXml);
    
    // Save feedback parameters
    ofXml feedbackXml;
    feedbackXml.appendChild("amount").set(ofToString(feedbackAmount));
    feedbackXml.appendChild("zoom").set(ofToString(feedbackZoom));
    feedbackXml.appendChild("rotate").set(ofToString(feedbackRotate));
    feedbackXml.appendChild("colorShift").set(ofToString(colorShift));
    xml.appendChild("feedback").appendChild(feedbackXml);
    
    // Save pattern parameters
    ofXml patternXml;
    patternXml.appendChild("type").set(ofToString((int)patternType));
    patternXml.appendChild("speed").set(ofToString(patternSpeed));
    patternXml.appendChild("density").set(ofToString(patternDensity));
    xml.appendChild("pattern").appendChild(patternXml);
    
    // Save video source if applicable
    if (sourceType == VIDEO && videoPlayer.isLoaded()) {
        xml.appendChild("videoPath").set(videoPlayer.getMoviePath());
    }
}

// Fixed loadPreset method for BackgroundLayer.cpp
void BackgroundLayer::loadPreset(ofXml& xml) {
    // Load source type
    auto sourceTypeNode = xml.find("sourceType");
    if (sourceTypeNode.size() > 0) {
        setSourceType((SourceType)ofToInt(xml.getChild("sourceType").getValue()));
    }
    
    // Load color parameters
    auto colorNode = xml.find("color");
    if (colorNode.size() > 0) {
        ofXml colorXml = xml.getChild("color");
        int r, g, b;
        
        // Start color
        r = ofToInt(colorXml.getChild("colorStart_r").getValue());
        g = ofToInt(colorXml.getChild("colorStart_g").getValue());
        b = ofToInt(colorXml.getChild("colorStart_b").getValue());
        setColorStart(ofColor(r, g, b));
        
        // End color
        r = ofToInt(colorXml.getChild("colorEnd_r").getValue());
        g = ofToInt(colorXml.getChild("colorEnd_g").getValue());
        b = ofToInt(colorXml.getChild("colorEnd_b").getValue());
        setColorEnd(ofColor(r, g, b));
        
        // Gradient type
        setGradientType(colorXml.getChild("gradientType").getValue());
    }
    
    // Load feedback parameters
    auto feedbackNode = xml.find("feedback");
    if (feedbackNode.size() > 0) {
        ofXml feedbackXml = xml.getChild("feedback");
        setFeedbackAmount(ofToFloat(feedbackXml.getChild("amount").getValue()));
        setFeedbackZoom(ofToFloat(feedbackXml.getChild("zoom").getValue()));
        setFeedbackRotate(ofToFloat(feedbackXml.getChild("rotate").getValue()));
        setColorShift(ofToFloat(feedbackXml.getChild("colorShift").getValue()));
    }
    
    // Load pattern parameters
    auto patternNode = xml.find("pattern");
    if (patternNode.size() > 0) {
        ofXml patternXml = xml.getChild("pattern");
        setPatternType((PatternType)ofToInt(patternXml.getChild("type").getValue()));
        setPatternSpeed(ofToFloat(patternXml.getChild("speed").getValue()));
        setPatternDensity(ofToFloat(patternXml.getChild("density").getValue()));
    }
    
    // Load video source if applicable
    auto videoPathNode = xml.find("videoPath");
    if (sourceType == VIDEO && videoPathNode.size() > 0) {
        string videoPath = xml.getChild("videoPath").getValue();
        if (ofFile::doesFileExist(videoPath)) {
            setVideoSource(videoPath);
        }
    }
}