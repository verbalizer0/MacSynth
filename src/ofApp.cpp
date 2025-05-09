// File: src/ofApp.cpp
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // Set app properties
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofBackground(20);
    
    // Setup canvas dimensions
    canvasWidth = 1280;
    canvasHeight = 720;
    
    // Initialize FBOs
    mainFbo.allocate(canvasWidth, canvasHeight, GL_RGBA);
    finalFbo.allocate(canvasWidth, canvasHeight, GL_RGBA);
    
    // Clear FBOs
    mainFbo.begin();
    ofClear(0, 0, 0, 0);
    mainFbo.end();
    
    finalFbo.begin();
    ofClear(0, 0, 0, 0);
    finalFbo.end();
    
    // Setup audio analyzer
    audioAnalyzer.setup();
    
    // Setup layers
    backgroundLayer.setup(canvasWidth, canvasHeight);
    spriteLayer.setup(canvasWidth, canvasHeight);
    fxLayer.setup(canvasWidth, canvasHeight);
    cameraLayer.setup(canvasWidth, canvasHeight);
    
    // Setup GUI (when implemented)
    gui = new GUI();
    gui->setup(this);
    
    // Default settings
    currentScene = 0;
    debugMode = false;
    playing = true;
    
    // Load default scene
    loadScene(currentScene);
    
    cout << "MacSynth setup complete!" << endl;
}

//--------------------------------------------------------------
void ofApp::update(){
    float deltaTime = ofGetLastFrameTime();
    
    // Update audio analyzer
    audioAnalyzer.update();
    
    // Get audio data
    float* spectrum = audioAnalyzer.getSpectrum();
    int numBands = audioAnalyzer.getNumBands();
    float phase = audioAnalyzer.getBeatPhase();
    
    // Only update if playing
    if (playing) {
        // Update layers
        backgroundLayer.update(deltaTime, spectrum, numBands, phase);
        spriteLayer.update(deltaTime, spectrum, numBands);
        fxLayer.update(phase, spectrum, numBands);
        cameraLayer.update(deltaTime, spectrum, numBands, phase);
        
        // Set camera feedback if enabled
        if (cameraLayer.isActive() && cameraLayer.isFeedbackEnabled()) {
            // Get camera pixels and pass to background layer
            // In a full implementation, this would pass actual camera pixels
            // For now, we'll skip this part
        }
    }
    
    // Update parameters from GUI
    // updateParameters();
}

//--------------------------------------------------------------
void ofApp::draw(){
    // Draw background layer to main FBO
    mainFbo.begin();
    ofClear(0, 0, 0, 255);
    backgroundLayer.draw();
    mainFbo.end();
    
    // Draw sprite layer on top
    mainFbo.begin();
    spriteLayer.draw();
    mainFbo.end();
    
    // Process with FX layer
    fxLayer.process(mainFbo);
    
    // Apply camera layer
    cameraLayer.draw();
    
    // Composite final output
    finalFbo.begin();
    ofClear(0, 0, 0, 255);
    
    // Draw FX layer output
    fxLayer.getOutputFbo().draw(0, 0);
    
    // Overlay camera layer if active
    if (cameraLayer.isActive()) {
        cameraLayer.getOutputFbo().draw(0, 0);
    }
    
    finalFbo.end();
    
    // Draw final output to screen
    ofBackground(20);
    
    // Center the output in the window
    float screenWidth = ofGetWidth();
    float screenHeight = ofGetHeight();
    float xPos = (screenWidth - canvasWidth) / 2;
    float yPos = (screenHeight - canvasHeight) / 2;
    
    finalFbo.draw(xPos, yPos);
    
    // Draw debug info if enabled
    if (debugMode) {
        ofDrawBitmapStringHighlight("FPS: " + ofToString(ofGetFrameRate(), 1), 10, 20);
        ofDrawBitmapStringHighlight("Scene: " + ofToString(currentScene + 1), 10, 40);
        ofDrawBitmapStringHighlight("BPM: " + ofToString(audioAnalyzer.getBPM(), 1), 10, 60);
        ofDrawBitmapStringHighlight("Phase: " + ofToString(audioAnalyzer.getBeatPhase(), 2), 10, 80);
        
        // Draw waveform
        ofPushStyle();
        ofSetColor(0, 255, 0);
        float* waveform = audioAnalyzer.getWaveform();
        for (int i = 0; i < audioAnalyzer.getBufferSize() - 1; i++) {
            ofDrawLine(10 + i * 0.5, 120 + waveform[i] * 50, 10 + (i + 1) * 0.5, 120 + waveform[i + 1] * 50);
        }
        ofPopStyle();
    }
    
    // Draw GUI if implemented
    gui->draw();
}

//--------------------------------------------------------------
void ofApp::exit(){
    // Clean up resources
    if (gui) {
        delete gui;
        gui = nullptr;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'd' || key == 'D') {
        debugMode = !debugMode;
    } else if (key == ' ') {
        playing = !playing;
    } else if (key >= '1' && key <= '8') {
        // Switch scenes with number keys 1-8
        int sceneIndex = key - '1';
        loadScene(sceneIndex);
    } else if (key == 's' || key == 'S') {
        // Save current scene
        saveScene(currentScene);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::updateParameters() {
    // This would update parameters from GUI
    // Will be implemented when GUI class is available
}

//--------------------------------------------------------------
void ofApp::saveScene(int sceneIndex) {
    // Create XML to save scene
    ofXml xml;
    
    // Save background layer
    ofXml backgroundXml;
    backgroundLayer.savePreset(backgroundXml);
    xml.appendChild("backgroundLayer").appendChild(backgroundXml);
    
    // Save sprite layer
    ofXml spriteXml;
    spriteLayer.savePreset(spriteXml);
    xml.appendChild("spriteLayer").appendChild(spriteXml);
    
    // Save FX layer
    ofXml fxXml;
    
    // Save each effect
    for (auto& effect : fxLayer.getEffects()) {
        ofXml effectXml;
        effect.second->savePreset(effectXml);
        fxXml.appendChild(effect.first).appendChild(effectXml);
    }
    
    xml.appendChild("fxLayer").appendChild(fxXml);
    
    // Save camera layer
    ofXml cameraXml;
    cameraLayer.savePreset(cameraXml);
    xml.appendChild("cameraLayer").appendChild(cameraXml);
    
    // Save XML to file
    string scenePath = "Scenes/scene_" + ofToString(sceneIndex) + ".xml";
    xml.save(scenePath);
    
    cout << "Scene saved to " << scenePath << endl;
}

//--------------------------------------------------------------
void ofApp::loadScene(int sceneIndex) {
    string scenePath = "Scenes/scene_" + ofToString(sceneIndex) + ".xml";
    
    // Check if scene file exists
    if (!ofFile::doesFileExist(scenePath)) {
        cout << "Scene file not found: " << scenePath << endl;
        return;
    }
    
    ofXml xml;
    bool loaded = xml.load(scenePath);
    
    if (loaded) {
        // Load background layer
        auto backgroundNode = xml.find("backgroundLayer");
        if (backgroundNode.size() > 0) {
            ofXml backgroundXml = xml.getChild("backgroundLayer");
            backgroundLayer.loadPreset(backgroundXml);
        }
        
        // Load sprite layer
        auto spriteNode = xml.find("spriteLayer");
        if (spriteNode.size() > 0) {
            ofXml spriteXml = xml.getChild("spriteLayer");
            spriteLayer.loadPreset(spriteXml);
        }
        
        // Load FX layer effects
        auto fxNode = xml.find("fxLayer");
        if (fxNode.size() > 0) {
            ofXml fxXml = xml.getChild("fxLayer");
            auto effectNodes = fxXml.getChildren();
            
            for (auto& effectNode : effectNodes) {
                string effectName = effectNode.getName();
                Effect* effect = fxLayer.getEffect(effectName);
                
                if (effect != nullptr) {
                    effect->loadPreset(effectNode);
                }
            }
        }
        
        // Load camera layer
        auto cameraNode = xml.find("cameraLayer");
        if (cameraNode.size() > 0) {
            ofXml cameraXml = xml.getChild("cameraLayer");
            cameraLayer.loadPreset(cameraXml);
        }
        
        currentScene = sceneIndex;
        cout << "Scene loaded from " << scenePath << endl;
    } else {
        cout << "Failed to load scene from " << scenePath << endl;
    }
}