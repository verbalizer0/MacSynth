// File: src/UI/GUI.cpp
#include "GUI.h"
#include "../ofApp.h"

GUI::GUI() {
    currentTab = "Background";
    showAudioPanel = true;
    showTemplatePanel = false;
}

GUI::~GUI() {
    // Clean up resources
}

void GUI::setup(ofApp* app) {
    this->app = app;
    
    // Setup ImGui
    gui.setup();
    
    // Initialize parameters
    // Background params
    BackgroundLayer& bg = app->backgroundLayer;
    backgroundParams.feedbackAmount = bg.getFeedbackAmount();
    backgroundParams.feedbackZoom = bg.getFeedbackZoom();
    backgroundParams.feedbackRotate = bg.getFeedbackRotate();
    backgroundParams.colorShift = bg.getColorShift();
    backgroundParams.patternType = (int)bg.getPatternType();
    backgroundParams.patternSpeed = bg.getPatternSpeed();
    backgroundParams.patternDensity = bg.getPatternDensity();
    backgroundParams.colorStart = bg.getColorStart();
    backgroundParams.colorEnd = bg.getColorEnd();
    backgroundParams.gradientType = bg.getGradientType();
    backgroundParams.sourceType = (int)bg.getSourceType();
    
    // Sprite params
    spriteParams.density = app->spriteLayer.getDensity();
    spriteParams.maxTrailLength = app->spriteLayer.getMaxTrailLength();
    spriteParams.spriteScale = app->spriteLayer.getSpriteScale();
    spriteParams.motionAmount = app->spriteLayer.getMotionAmount();
    spriteParams.blendMode = app->spriteLayer.getBlendMode();
    spriteParams.audioReactivity = app->spriteLayer.getAudioReactivity();
    
    // FX params (initialize with existing effects)
    for (auto& effect : app->fxLayer.getEffects()) {
        string name = effect.first;
        fxParams.effectsEnabled[name] = effect.second->isEnabled();
        fxParams.effectsIntensity[name] = effect.second->getIntensity();
        
        // Get effect parameters
        map<string, float> params;
        // This is a simplified approach - in a real app, you would iterate through 
        // all parameters or have a more structured way to access them
        if (name == "pixelate") {
            params["sizeX"] = effect.second->getParameter("sizeX");
            params["sizeY"] = effect.second->getParameter("sizeY");
            params["dynamicSize"] = effect.second->getParameter("dynamicSize");
            params["threshold"] = effect.second->getParameter("threshold");
        } else if (name == "feedback") {
            params["amount"] = effect.second->getParameter("amount");
            params["zoom"] = effect.second->getParameter("zoom");
            params["rotate"] = effect.second->getParameter("rotate");
            params["offsetX"] = effect.second->getParameter("offsetX");
            params["offsetY"] = effect.second->getParameter("offsetY");
            params["hueShift"] = effect.second->getParameter("hueShift");
            params["fade"] = effect.second->getParameter("fade");
        }
        
        fxParams.effectParams[name] = params;
    }
    
    // Camera params
    cameraParams.active = app->cameraLayer.isActive();
    cameraParams.feedbackEnabled = app->cameraLayer.isFeedbackEnabled();
    cameraParams.x = app->cameraLayer.getX();
    cameraParams.y = app->cameraLayer.getY();
    cameraParams.scale = app->cameraLayer.getScale();
    cameraParams.rotation = app->cameraLayer.getRotation();
    cameraParams.opacity = app->cameraLayer.getOpacity();
    cameraParams.mirror = app->cameraLayer.getMirror();
    cameraParams.chromaKeyEnabled = false; // Initialize
    cameraParams.chromaColor = ofColor(0, 255, 0); // Initialize
    cameraParams.chromaTolerance = 0.4f; // Initialize
    
    // Audio params
    audioParams.gain = 1.0f;
    audioParams.device = 0;
    audioParams.clockSource = "Audio";
    audioParams.bpm = app->audioAnalyzer.getBPM();
}

void GUI::update() {
    // This method could update GUI state or apply parameter changes
}

void GUI::draw() {
    gui.begin();
    
    // Main tabs
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::MenuItem("Background", nullptr, currentTab == "Background")) {
            currentTab = "Background";
        }
        if (ImGui::MenuItem("Sprites", nullptr, currentTab == "Sprites")) {
            currentTab = "Sprites";
        }
        if (ImGui::MenuItem("FX", nullptr, currentTab == "FX")) {
            currentTab = "FX";
        }
        if (ImGui::MenuItem("Camera", nullptr, currentTab == "Camera")) {
            currentTab = "Camera";
        }
        if (ImGui::MenuItem("Tempo", nullptr, currentTab == "Tempo")) {
            currentTab = "Tempo";
        }
        
        ImGui::EndMainMenuBar();
    }
    
    // Draw scene selector
    drawSceneSelector();
    
    // Draw audio panel
    if (showAudioPanel) {
        drawAudioPanel();
    }
    
    // Draw active tab
    if (currentTab == "Background") {
        drawBackgroundTab();
    } else if (currentTab == "Sprites") {
        drawSpriteTab();
    } else if (currentTab == "FX") {
        drawFXTab();
    } else if (currentTab == "Camera") {
        drawCameraTab();
    } else if (currentTab == "Tempo") {
        drawTempoTab();
    }
    
    gui.end();
}

void GUI::drawBackgroundTab() {
    if (ImGui::Begin("Background Layer", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Source type selection
        const char* sourceTypes[] = { "Color", "Video", "Camera", "Pattern" };
        if (ImGui::Combo("Source Type", &backgroundParams.sourceType, sourceTypes, 4)) {
            app->backgroundLayer.setSourceType((BackgroundLayer::SourceType)backgroundParams.sourceType);
        }
        
        ImGui::Separator();
        
        // Color controls
        if (backgroundParams.sourceType == 0) {
            ImGui::Text("Color Settings");
            
            float color1[3] = {
                backgroundParams.colorStart.r / 255.0f,
                backgroundParams.colorStart.g / 255.0f,
                backgroundParams.colorStart.b / 255.0f
            };
            
            float color2[3] = {
                backgroundParams.colorEnd.r / 255.0f,
                backgroundParams.colorEnd.g / 255.0f,
                backgroundParams.colorEnd.b / 255.0f
            };
            
            if (ImGui::ColorEdit3("Start Color", color1)) {
                backgroundParams.colorStart.r = color1[0] * 255.0f;
                backgroundParams.colorStart.g = color1[1] * 255.0f;
                backgroundParams.colorStart.b = color1[2] * 255.0f;
                app->backgroundLayer.setColorStart(backgroundParams.colorStart);
            }
            
            if (ImGui::ColorEdit3("End Color", color2)) {
                backgroundParams.colorEnd.r = color2[0] * 255.0f;
                backgroundParams.colorEnd.g = color2[1] * 255.0f;
                backgroundParams.colorEnd.b = color2[2] * 255.0f;
                app->backgroundLayer.setColorEnd(backgroundParams.colorEnd);
            }
            
            const char* gradientTypes[] = { "none", "linear", "radial" };
            int gradientIndex = 0;
            if (backgroundParams.gradientType == "linear") gradientIndex = 1;
            else if (backgroundParams.gradientType == "radial") gradientIndex = 2;
            
            if (ImGui::Combo("Gradient Type", &gradientIndex, gradientTypes, 3)) {
                backgroundParams.gradientType = gradientTypes[gradientIndex];
                app->backgroundLayer.setGradientType(backgroundParams.gradientType);
            }
        }
        
        // Pattern controls
        if (backgroundParams.sourceType == 3) {
            ImGui::Text("Pattern Settings");
            
            const char* patternTypes[] = { "Gradient", "Bars", "Circles", "Noise" };
            if (ImGui::Combo("Pattern Type", &backgroundParams.patternType, patternTypes, 4)) {
                app->backgroundLayer.setPatternType((BackgroundLayer::PatternType)backgroundParams.patternType);
            }
            
            if (ImGui::SliderFloat("Pattern Speed", &backgroundParams.patternSpeed, 0.1f, 5.0f)) {
                app->backgroundLayer.setPatternSpeed(backgroundParams.patternSpeed);
            }
            
            if (ImGui::SliderFloat("Pattern Density", &backgroundParams.patternDensity, 1.0f, 20.0f)) {
                app->backgroundLayer.setPatternDensity(backgroundParams.patternDensity);
            }
        }
        
        ImGui::Separator();
        
        // Feedback controls
        ImGui::Text("Feedback Settings");
        
        if (ImGui::SliderFloat("Feedback Amount", &backgroundParams.feedbackAmount, 0.0f, 1.0f)) {
            app->backgroundLayer.setFeedbackAmount(backgroundParams.feedbackAmount);
        }
        
        if (ImGui::SliderFloat("Feedback Zoom", &backgroundParams.feedbackZoom, 0.9f, 1.1f)) {
            app->backgroundLayer.setFeedbackZoom(backgroundParams.feedbackZoom);
        }
        
        if (ImGui::SliderFloat("Feedback Rotate", &backgroundParams.feedbackRotate, -0.1f, 0.1f)) {
            app->backgroundLayer.setFeedbackRotate(backgroundParams.feedbackRotate);
        }
        
        if (ImGui::SliderFloat("Color Shift", &backgroundParams.colorShift, 0.0f, 1.0f)) {
            app->backgroundLayer.setColorShift(backgroundParams.colorShift);
        }
    }
    ImGui::End();
}

void GUI::drawSpriteTab() {
    if (ImGui::Begin("Sprite Layer", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Sprite controls
        if (ImGui::SliderInt("Sprite Count", &spriteParams.density, 0, 50)) {
            app->spriteLayer.setDensity(spriteParams.density);
        }
        
        if (ImGui::SliderInt("Trail Length", &spriteParams.maxTrailLength, 0, 30)) {
            app->spriteLayer.setMaxTrailLength(spriteParams.maxTrailLength);
        }
        
        if (ImGui::SliderFloat("Sprite Scale", &spriteParams.spriteScale, 0.1f, 3.0f)) {
            app->spriteLayer.setSpriteScale(spriteParams.spriteScale);
        }
        
        if (ImGui::SliderFloat("Motion Amount", &spriteParams.motionAmount, 0.0f, 3.0f)) {
            app->spriteLayer.setMotionAmount(spriteParams.motionAmount);
        }
        
        const char* blendModes[] = { "alpha", "add", "screen", "multiply", "subtract" };
        int blendIndex = 0;
        if (spriteParams.blendMode == "add") blendIndex = 1;
        else if (spriteParams.blendMode == "screen") blendIndex = 2;
        else if (spriteParams.blendMode == "multiply") blendIndex = 3;
        else if (spriteParams.blendMode == "subtract") blendIndex = 4;
        
        if (ImGui::Combo("Blend Mode", &blendIndex, blendModes, 5)) {
            spriteParams.blendMode = blendModes[blendIndex];
            app->spriteLayer.setBlendMode(spriteParams.blendMode);
        }
        
        if (ImGui::SliderFloat("Audio Reactivity", &spriteParams.audioReactivity, 0.0f, 1.0f)) {
            app->spriteLayer.setAudioReactivity(spriteParams.audioReactivity);
        }
        
        // Sprite library (placeholder for now)
        ImGui::Separator();
        ImGui::Text("Sprite Library");
        ImGui::Text("(Not implemented in this sample)");
        
        // This would display a grid of sprite thumbnails
        // for selection with library management functions
    }
    ImGui::End();
}

void GUI::drawFXTab() {
    if (ImGui::Begin("FX Layer", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Loop through available effects
        for (auto& effectPair : app->fxLayer.getEffects()) {
            string name = effectPair.first;
            Effect* effect = effectPair.second;
            
            ImGui::Separator();
            
            // Enable/disable toggle
            bool enabled = fxParams.effectsEnabled[name];
            if (ImGui::Checkbox(("Enable " + name).c_str(), &enabled)) {
                fxParams.effectsEnabled[name] = enabled;
                app->fxLayer.enableEffect(name, enabled);
            }
            
            // Only show parameters if effect is enabled
            if (enabled) {
                // Effect intensity
                float intensity = fxParams.effectsIntensity[name];
                if (ImGui::SliderFloat(("Intensity##" + name).c_str(), &intensity, 0.0f, 1.0f)) {
                    fxParams.effectsIntensity[name] = intensity;
                    effect->setIntensity(intensity);
                }
                
                // Effect-specific parameters
                if (name == "pixelate") {
                    auto& params = fxParams.effectParams[name];
                    
                    if (ImGui::SliderFloat("Size X", &params["sizeX"], 1.0f, 100.0f)) {
                        app->fxLayer.setEffectParameter(name, "sizeX", params["sizeX"]);
                    }
                    
                    if (ImGui::SliderFloat("Size Y", &params["sizeY"], 1.0f, 100.0f)) {
                        app->fxLayer.setEffectParameter(name, "sizeY", params["sizeY"]);
                    }
                    
                    if (ImGui::SliderFloat("Threshold", &params["threshold"], 0.0f, 1.0f)) {
                        app->fxLayer.setEffectParameter(name, "threshold", params["threshold"]);
                    }
                    
                    bool dynamicSize = params["dynamicSize"] > 0.5f;
                    if (ImGui::Checkbox("Dynamic Size", &dynamicSize)) {
                        params["dynamicSize"] = dynamicSize ? 1.0f : 0.0f;
                        app->fxLayer.setEffectParameter(name, "dynamicSize", params["dynamicSize"]);
                    }
                } else if (name == "feedback") {
                    auto& params = fxParams.effectParams[name];
                    
                    if (ImGui::SliderFloat("Amount", &params["amount"], 0.0f, 1.0f)) {
                        app->fxLayer.setEffectParameter(name, "amount", params["amount"]);
                    }
                    
                    if (ImGui::SliderFloat("Zoom", &params["zoom"], 0.9f, 1.1f)) {
                        app->fxLayer.setEffectParameter(name, "zoom", params["zoom"]);
                    }
                    
                    if (ImGui::SliderFloat("Rotate", &params["rotate"], -0.1f, 0.1f)) {
                        app->fxLayer.setEffectParameter(name, "rotate", params["rotate"]);
                    }
                    
                    if (ImGui::SliderFloat("Hue Shift", &params["hueShift"], 0.0f, 1.0f)) {
                        app->fxLayer.setEffectParameter(name, "hueShift", params["hueShift"]);
                    }
                    
                    if (ImGui::SliderFloat("Fade", &params["fade"], 0.0f, 1.0f)) {
                        app->fxLayer.setEffectParameter(name, "fade", params["fade"]);
                    }
                    
                    if (ImGui::SliderFloat("Offset X", &params["offsetX"], -50.0f, 50.0f)) {
                        app->fxLayer.setEffectParameter(name, "offsetX", params["offsetX"]);
                    }
                    
                    if (ImGui::SliderFloat("Offset Y", &params["offsetY"], -50.0f, 50.0f)) {
                        app->fxLayer.setEffectParameter(name, "offsetY", params["offsetY"]);
                    }
                }
            }
        }
    }
    ImGui::End();
}

void GUI::drawCameraTab() {
    if (ImGui::Begin("Camera Layer", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Camera enable
        bool active = cameraParams.active;
        if (ImGui::Checkbox("Enable Camera", &active)) {
            cameraParams.active = active;
            app->cameraLayer.setActive(active);
        }
        
        if (active) {
            // Camera device selection
            if (ImGui::Button("Setup Camera")) {
                app->cameraLayer.setupCamera(0); // Use device 0 for now
            }
            
            ImGui::Separator();
            
            // Position and transform
            if (ImGui::SliderFloat("X Position", &cameraParams.x, 0.0f, 1.0f)) {
                app->cameraLayer.setX(cameraParams.x);
            }
            
            if (ImGui::SliderFloat("Y Position", &cameraParams.y, 0.0f, 1.0f)) {
                app->cameraLayer.setY(cameraParams.y);
            }
            
            if (ImGui::SliderFloat("Scale", &cameraParams.scale, 0.1f, 3.0f)) {
                app->cameraLayer.setScale(cameraParams.scale);
            }
            
            if (ImGui::SliderFloat("Rotation", &cameraParams.rotation, -PI, PI)) {
                app->cameraLayer.setRotation(cameraParams.rotation);
            }
            
            if (ImGui::SliderFloat("Opacity", &cameraParams.opacity, 0.0f, 1.0f)) {
                app->cameraLayer.setOpacity(cameraParams.opacity);
            }
            
            bool mirror = cameraParams.mirror;
            if (ImGui::Checkbox("Mirror", &mirror)) {
                cameraParams.mirror = mirror;
                app->cameraLayer.setMirror(mirror);
            }
            
            ImGui::Separator();
            
            // Feedback controls
            bool feedbackEnabled = cameraParams.feedbackEnabled;
            if (ImGui::Checkbox("Enable Feedback", &feedbackEnabled)) {
                cameraParams.feedbackEnabled = feedbackEnabled;
                app->cameraLayer.setFeedbackEnabled(feedbackEnabled);
            }
            
            ImGui::Separator();
            
            // Chroma key controls
            bool chromaKeyEnabled = cameraParams.chromaKeyEnabled;
            if (ImGui::Checkbox("Enable Chroma Key", &chromaKeyEnabled)) {
                cameraParams.chromaKeyEnabled = chromaKeyEnabled;
                app->cameraLayer.setChromaKey(chromaKeyEnabled);
            }
            
            if (chromaKeyEnabled) {
                float color[3] = {
                    cameraParams.chromaColor.r / 255.0f,
                    cameraParams.chromaColor.g / 255.0f,
                    cameraParams.chromaColor.b / 255.0f
                };
                
                if (ImGui::ColorEdit3("Chroma Color", color)) {
                    cameraParams.chromaColor.r = color[0] * 255.0f;
                    cameraParams.chromaColor.g = color[1] * 255.0f;
                    cameraParams.chromaColor.b = color[2] * 255.0f;
                    app->cameraLayer.setChromaColor(cameraParams.chromaColor);
                }
                
                if (ImGui::SliderFloat("Tolerance", &cameraParams.chromaTolerance, 0.0f, 1.0f)) {
                    app->cameraLayer.setChromaTolerance(cameraParams.chromaTolerance);
                }
            }
        }
    }
    ImGui::End();
}

void GUI::drawTempoTab() {
    if (ImGui::Begin("Tempo Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Audio input settings
        if (ImGui::SliderFloat("Input Gain", &audioParams.gain, 0.0f, 4.0f)) {
            app->audioAnalyzer.setInputGain(audioParams.gain);
        }
        
        // Device selection
        if (ImGui::Button("Select Microphone")) {
            app->audioAnalyzer.setupMicrophone(0);
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Select Line In")) {
            app->audioAnalyzer.setupLineInput(0);
        }
        
        ImGui::Separator();
        
        // Clock source
        const char* clockSources[] = { "Audio", "MIDI", "CV", "Manual" };
        int clockIndex = 0;
        if (audioParams.clockSource == "MIDI") clockIndex = 1;
        else if (audioParams.clockSource == "CV") clockIndex = 2;
        else if (audioParams.clockSource == "Manual") clockIndex = 3;
        
        if (ImGui::Combo("Clock Source", &clockIndex, clockSources, 4)) {
            audioParams.clockSource = clockSources[clockIndex];
            // This would set the clock source in a real implementation
        }
        
        // Manual BPM if clock source is Manual
        if (audioParams.clockSource == "Manual") {
            if (ImGui::SliderFloat("BPM", &audioParams.bpm, 40.0f, 200.0f)) {
                // This would set the manual BPM in a real implementation
            }
            
            if (ImGui::Button("Tap Tempo")) {
                // This would implement tap tempo functionality
            }
        } else {
            // Display detected BPM for other clock sources
            ImGui::Text("Detected BPM: %.1f", app->audioAnalyzer.getBPM());
        }
        
        ImGui::Separator();
        
        // Audio visualizer
        ImGui::Text("Audio Input");
        
        // Draw a simple visualizer
        float* spectrum = app->audioAnalyzer.getSpectrum();
        int numBands = app->audioAnalyzer.getNumBands();
        
        if (spectrum && numBands > 0) {
            ImGui::PlotHistogram("##spectrum", spectrum, numBands, 0, nullptr, 0.0f, 1.0f, ImVec2(300, 80));
        } else {
            ImGui::Text("No audio input detected");
        }
    }
    ImGui::End();
}

void GUI::drawAudioPanel() {
    ImGui::SetNextWindowPos(ImVec2(10, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200, 60), ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin("Audio Monitor", &showAudioPanel, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
        // Display BPM
        ImGui::Text("BPM: %.1f", app->audioAnalyzer.getBPM());
        
        // Confidence meter
        float confidence = 0.7f; // In a real implementation, get from audio analyzer
        ImGui::ProgressBar(confidence, ImVec2(100, 10));
        
        // Beat indicator
        bool onBeat = app->audioAnalyzer.isOnBeat();
        ImGui::SameLine();
        if (onBeat) {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "●");
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "○");
        }
    }
    ImGui::End();
}

void GUI::drawSceneSelector() {
    ImGui::SetNextWindowPos(ImVec2(ofGetWidth() - 260, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(250, 60), ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin("Scenes", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
        // Scene selector buttons
        ImGui::Text("Scenes:");
        
        for (int i = 0; i < 8; i++) {
            if (i > 0) ImGui::SameLine();
            
            bool isCurrentScene = (app->currentScene == i);
            if (isCurrentScene) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.5f, 0.8f, 1.0f));
            }
            
            if (ImGui::Button(to_string(i + 1).c_str(), ImVec2(25, 25))) {
                app->loadScene(i);
            }
            
            if (isCurrentScene) {
                ImGui::PopStyleColor();
            }
        }
        
        // Save/load buttons
        ImGui::Separator();
        if (ImGui::Button("Save Scene")) {
            app->saveScene(app->currentScene);
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Play/Pause")) {
            app->playing = !app->playing;
        }
    }
    ImGui::End();
}

void GUI::drawSlider(string label, float* value, float min, float max) {
    ImGui::SliderFloat(label.c_str(), value, min, max);
}

void GUI::drawColorEdit(string label, ofColor* color) {
    float col[3] = {
        color->r / 255.0f,
        color->g / 255.0f,
        color->b / 255.0f
    };
    
    if (ImGui::ColorEdit3(label.c_str(), col)) {
        color->r = col[0] * 255.0f;
        color->g = col[1] * 255.0f;
        color->b = col[2] * 255.0f;
    }
}