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
        cout << "Camera initialized: " << camera.getDeviceName() << endl;
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

void CameraLayer::savePreset(ofXml& xml) {
    // Save camera settings
    xml.addChild("active").set(active);
    xml.addChild("feedbackEnabled").set(feedbackEnabled);
    
    // Save position and transform
    xml.addChild("x").set(x);
    xml.addChild("y").set(y);
    xml.addChild("scale").set(scale);
    xml.addChild("rotation").set(rotation);
    xml.addChild("opacity").set(opacity);
    xml.addChild("mirror").set(mirror);
    
    // Save chroma key settings
    xml.addChild("chromaKeyEnabled").set(chromaKeyEnabled);
    
    ofXml chromaColorXml;
    chromaColorXml.addChild("r").set(chromaColor.r);
    chromaColorXml.addChild("g").set(chromaColor.g);
    chromaColorXml.addChild("b").set(chromaColor.b);
    xml.addChild("chromaColor").appendChild(chromaColorXml);
    
    xml.addChild("chromaTolerance").set(chromaTolerance);
}

void CameraLayer::loadPreset(ofXml& xml) {
    // Load camera settings
    if (xml.find("active").size() > 0) {
        setActive(xml.getChild("active").getBoolValue());
    }
    
    if (xml.find("feedbackEnabled").size() > 0) {
        setFeedbackEnabled(xml.getChild("feedbackEnabled").getBoolValue());
    }
    
    // Load position and transform
    if (xml.find("x").size() > 0) {
        setX(xml.getChild("x").getFloatValue());
    }
    
    if (xml.find("y").size() > 0) {
        setY(xml.getChild("y").getFloatValue());
    }
    
    if (xml.find("scale").size() > 0) {
        setScale(xml.getChild("scale").getFloatValue());
    }
    
    if (xml.find("rotation").size() > 0) {
        setRotation(xml.getChild("rotation").getFloatValue());
    }
    
    if (xml.find("opacity").size() > 0) {
        setOpacity(xml.getChild("opacity").getFloatValue());
    }
    
    if (xml.find("mirror").size() > 0) {
        setMirror(xml.getChild("mirror").getBoolValue());
    }
    
    // Load chroma key settings
    if (xml.find("chromaKeyEnabled").size() > 0) {
        setChromaKey(xml.getChild("chromaKeyEnabled").getBoolValue());
    }
    
    if (xml.find("chromaColor").size() > 0) {
        ofXml colorXml = xml.getChild("chromaColor");
        int r = colorXml.getChild("r").getIntValue();
        int g = colorXml.getChild("g").getIntValue();
        int b = colorXml.getChild("b").getIntValue();
        setChromaColor(ofColor(r, g, b));
    }
    
    if (xml.find("chromaTolerance").size() > 0) {
        setChromaTolerance(xml.getChild("chromaTolerance").getFloatValue());
    }
}