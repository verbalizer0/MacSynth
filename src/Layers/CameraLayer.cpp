// File: src/Layers/CameraLayer.cpp
#include "CameraLayer.h"

CameraLayer::CameraLayer() {
    width = 1280;
    height = 720;
    
    // Default parameters
    active = false;
    feedbackEnabled = false;
    
    x = 0.5;
    y = 0.5;
    scale = 1.0;
    rotation = 0.0;
    opacity = 1.0;
    mirror = true;
    
    chromaKeyEnabled = false;
    chromaColor = ofColor(0, 255, 0);  // Green
    chromaTolerance = 0.4;
}

CameraLayer::~CameraLayer() {
    // Close camera if open
    if (camera.isInitialized()) {
        camera.close();
    }
}

void CameraLayer::setup(int width, int height) {
    this->width = width;
    this->height = height;
    
    // Set up FBO
    outputFbo.allocate(width, height, GL_RGBA);
    
    // Clear FBO
    outputFbo.begin();
    ofClear(0, 0, 0, 0);
    outputFbo.end();
    
    // Setup chroma key shader
    if (!chromaKeyShader.isLoaded()) {
        // In a real implementation, this would load shader files
        // For this example, we will simulate a chroma key shader
        /*
        chromaKeyShader.load("shaders/chromakey");
        */
    }
    
    // Try to initialize camera
    setupCamera(0);
}

// Fixed setupCamera method for CameraLayer.cpp
bool CameraLayer::setupCamera(int deviceId) {
    // First close existing camera if open
    if (camera.isInitialized()) {
        camera.close();
    }
    
    // Set up camera with device ID
    camera.setDeviceID(deviceId);
    bool success = camera.setup(640, 480);
    
    if (success) {
        active = true;
        cout << "Camera initialized: Device ID " << deviceId << endl;
    } else {
        active = false;
        cout << "Failed to initialize camera with device ID " << deviceId << endl;
    }
    
    return success;
}

void CameraLayer::update(float deltaTime, float* audioData, int numBands, float phase) {
    // Update camera
    if (active && camera.isInitialized()) {
        camera.update();
        
        // Apply audio reactivity
        if (audioData != nullptr && numBands > 0) {
            applyAudioReactivity(audioData, numBands);
        }
    }
}

void CameraLayer::draw() {
    if (!active || !camera.isInitialized() || !camera.isFrameNew()) {
        return;
    }
    
    outputFbo.begin();
    ofClear(0, 0, 0, 0);
    
    ofPushMatrix();
    ofPushStyle();
    
    // Convert normalized coordinates to pixels
    float pixelX = x * width;
    float pixelY = y * height;
    
    // Calculate dimensions to maintain aspect ratio
    float cameraWidth = camera.getWidth();
    float cameraHeight = camera.getHeight();
    float cameraRatio = cameraWidth / cameraHeight;
    float screenRatio = (float)width / height;
    
    float drawWidth, drawHeight;
    
    if (cameraRatio > screenRatio) {
        // Camera is wider than canvas - fit height
        drawHeight = height * scale;
        drawWidth = drawHeight * cameraRatio;
    } else {
        // Camera is taller than canvas - fit width
        drawWidth = width * scale;
        drawHeight = drawWidth / cameraRatio;
    }
    
    // Apply transformations
    ofTranslate(pixelX, pixelY);
    ofRotateZDeg(ofRadToDeg(rotation));
    
    // Apply mirror if enabled
    if (mirror) {
        ofScale(-1, 1);
    } else {
        ofScale(1, 1);
    }
    
    // Set opacity
    ofSetColor(255, 255, 255, opacity * 255);
    
    // Draw camera
    if (chromaKeyEnabled) {
        // Apply chroma key
        applyChromaKey(camera.getTexture());
    } else {
        // Draw normally
        camera.draw(-drawWidth / 2, -drawHeight / 2, drawWidth, drawHeight);
    }
    
    ofPopStyle();
    ofPopMatrix();
    
    outputFbo.end();
}

void CameraLayer::applyChromaKey(ofTexture& texture) {
    // In a real implementation, this would use the shader
    // For this example, we'll just simulate a chroma key effect
    
    // Get pixels from camera
    ofPixels pixels;
    texture.readToPixels(pixels);
    
    // Create output pixels
    ofPixels outputPixels;
    outputPixels.allocate(pixels.getWidth(), pixels.getHeight(), OF_PIXELS_RGBA);
    
    // Process each pixel
    for (int y = 0; y < pixels.getHeight(); y++) {
        for (int x = 0; x < pixels.getWidth(); x++) {
            ofColor pixelColor = pixels.getColor(x, y);
            
            // Calculate color distance from chroma key color
            float distance = ofDist(pixelColor.r, pixelColor.g, pixelColor.b,
                                   chromaColor.r, chromaColor.g, chromaColor.b);
            
            // Normalize distance
            distance = distance / 441.67; // sqrt(255^2 + 255^2 + 255^2)
            
            // Apply tolerance
            if (distance < chromaTolerance) {
                // Transparent pixel
                outputPixels.setColor(x, y, ofColor(0, 0, 0, 0));
            } else {
                // Keep original pixel
                outputPixels.setColor(x, y, pixelColor);
            }
        }
    }
    
    // Create texture from processed pixels
    ofTexture outputTexture;
    outputTexture.allocate(outputPixels);
    outputTexture.loadData(outputPixels);
    
    // Draw texture
    float drawWidth = pixels.getWidth();
    float drawHeight = pixels.getHeight();
    outputTexture.draw(-drawWidth / 2, -drawHeight / 2, drawWidth, drawHeight);
}

void CameraLayer::applyAudioReactivity(float* audioData, int numBands) {
    // Example: Modulate scale with bass frequencies
    float bassSum = 0;
    int bassBands = numBands / 4;
    
    for (int i = 0; i < bassBands; i++) {
        bassSum += audioData[i];
    }
    
    float bassEnergy = bassSum / bassBands;
    
    // Scale up temporarily with bass
    scale = scale * 0.9 + (1.0 + bassEnergy * 0.2) * 0.1;
    
    // Rotate slightly with mid frequencies
    float midSum = 0;
    for (int i = numBands / 4; i < numBands * 3 / 4; i++) {
        midSum += audioData[i];
    }
    
    float midEnergy = midSum / (numBands / 2);
    rotation += (midEnergy - 0.5) * 0.01;
    
    // Ensure rotation stays in reasonable range
    rotation = fmodf(rotation, TWO_PI);
}

bool CameraLayer::setParameter(string name, float value) {
    if (name == "x") {
        x = ofClamp(value, 0, 1);
        return true;
    } else if (name == "y") {
        y = ofClamp(value, 0, 1);
        return true;
    } else if (name == "scale") {
        scale = ofClamp(value, 0.1, 3.0);
        return true;
    } else if (name == "rotation") {
        rotation = value;
        return true;
    } else if (name == "opacity") {
        opacity = ofClamp(value, 0, 1);
        return true;
    } else if (name == "mirror") {
        mirror = value > 0.5;
        return true;
    } else if (name == "chromaKey") {
        chromaKeyEnabled = value > 0.5;
        return true;
    } else if (name == "chromaTolerance") {
        chromaTolerance = ofClamp(value, 0, 1);
        return true;
    }
    
    return false;
}

// Fixed savePreset method for CameraLayer.cpp
void CameraLayer::savePreset(ofXml& xml) {
    // Save camera settings
    xml.appendChild("active").set(ofToString(active));
    xml.appendChild("feedbackEnabled").set(ofToString(feedbackEnabled));
    
    // Save position and transform
    xml.appendChild("x").set(ofToString(x));
    xml.appendChild("y").set(ofToString(y));
    xml.appendChild("scale").set(ofToString(scale));
    xml.appendChild("rotation").set(ofToString(rotation));
    xml.appendChild("opacity").set(ofToString(opacity));
    xml.appendChild("mirror").set(ofToString(mirror));
    
    // Save chroma key settings
    xml.appendChild("chromaKeyEnabled").set(ofToString(chromaKeyEnabled));
    
    ofXml chromaColorXml;
    chromaColorXml.appendChild("r").set(ofToString(chromaColor.r));
    chromaColorXml.appendChild("g").set(ofToString(chromaColor.g));
    chromaColorXml.appendChild("b").set(ofToString(chromaColor.b));
    xml.appendChild("chromaColor").appendChild(chromaColorXml);
    
    xml.appendChild("chromaTolerance").set(ofToString(chromaTolerance));
}

// Fixed loadPreset method for CameraLayer.cpp
void CameraLayer::loadPreset(ofXml& xml) {
    // Load camera settings
    auto activeNode = xml.find("active");
    if (activeNode.size() > 0) {
        setActive(ofToBool(xml.getChild("active").getValue()));
    }
    
    auto feedbackNode = xml.find("feedbackEnabled");
    if (feedbackNode.size() > 0) {
        setFeedbackEnabled(ofToBool(xml.getChild("feedbackEnabled").getValue()));
    }
    
    // Load position and transform
    auto xNode = xml.find("x");
    if (xNode.size() > 0) {
        setX(ofToFloat(xml.getChild("x").getValue()));
    }
    
    auto yNode = xml.find("y");
    if (yNode.size() > 0) {
        setY(ofToFloat(xml.getChild("y").getValue()));
    }
    
    auto scaleNode = xml.find("scale");
    if (scaleNode.size() > 0) {
        setScale(ofToFloat(xml.getChild("scale").getValue()));
    }
    
    auto rotationNode = xml.find("rotation");
    if (rotationNode.size() > 0) {
        setRotation(ofToFloat(xml.getChild("rotation").getValue()));
    }
    
    auto opacityNode = xml.find("opacity");
    if (opacityNode.size() > 0) {
        setOpacity(ofToFloat(xml.getChild("opacity").getValue()));
    }
    
    auto mirrorNode = xml.find("mirror");
    if (mirrorNode.size() > 0) {
        setMirror(ofToBool(xml.getChild("mirror").getValue()));
    }
    
    // Load chroma key settings
    auto chromaKeyNode = xml.find("chromaKeyEnabled");
    if (chromaKeyNode.size() > 0) {
        setChromaKey(ofToBool(xml.getChild("chromaKeyEnabled").getValue()));
    }
    
    auto chromaColorNode = xml.find("chromaColor");
    if (chromaColorNode.size() > 0) {
        ofXml colorXml = xml.getChild("chromaColor");
        int r = ofToInt(colorXml.getChild("r").getValue());
        int g = ofToInt(colorXml.getChild("g").getValue());
        int b = ofToInt(colorXml.getChild("b").getValue());
        setChromaColor(ofColor(r, g, b));
    }
    
    auto chromaToleranceNode = xml.find("chromaTolerance");
    if (chromaToleranceNode.size() > 0) {
        setChromaTolerance(ofToFloat(xml.getChild("chromaTolerance").getValue()));
    }
}