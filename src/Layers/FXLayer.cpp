// File: src/Layers/FXLayer.cpp
#include "FXLayer.h"
#include "PixelateEffect.h"

FXLayer::FXLayer() {
    width = 1280;
    height = 720;
    
    // Initialize global parameters
    globalParams["pixelate"] = 1.0;
}

FXLayer::~FXLayer() {
    // Clean up effects
    for (auto& effect : effects) {
        delete effect.second;
    }
    effects.clear();
}

void FXLayer::setup(int width, int height) {
    this->width = width;
    this->height = height;
    
    // Allocate FBOs
    outputFbo.allocate(width, height, GL_RGBA);
    tempFbo.allocate(width, height, GL_RGBA);
    
    // Clear FBOs
    outputFbo.begin();
    ofClear(0, 0, 0, 0);
    outputFbo.end();
    
    tempFbo.begin();
    ofClear(0, 0, 0, 0);
    tempFbo.end();
    
    // Initialize default effects
    initializeDefaultEffects();
}

void FXLayer::initializeDefaultEffects() {
    // Add pixelate effect
    PixelateEffect* pixelate = new PixelateEffect();
    pixelate->setup(width, height);
    effects["pixelate"] = pixelate;
}

void FXLayer::update(float phase, float* audioData, int numBands) {
    // Update all effects
    for (auto& effect : effects) {
        if (effect.second->isEnabled()) {
            effect.second->update(phase, audioData, numBands, globalParams);
        }
    }
}

void FXLayer::process(ofFbo& inputFbo) {
    // Copy input to output FBO
    outputFbo.begin();
    ofClear(0, 0, 0, 0);
    inputFbo.draw(0, 0);
    outputFbo.end();
    
    // Process each effect in sequence
    for (auto& effect : effects) {
        if (effect.second->isEnabled() && effect.second->getIntensity() > 0.0) {
            // Copy current output to temp buffer
            tempFbo.begin();
            ofClear(0, 0, 0, 0);
            outputFbo.draw(0, 0);
            tempFbo.end();
            
            // Apply effect to temp and store in output
            outputFbo.begin();
            ofClear(0, 0, 0, 0);
            effect.second->apply(tempFbo);
            outputFbo.end();
        }
    }
}

void FXLayer::addEffect(Effect* effect) {
    if (effect == nullptr) return;
    
    // Check if effect already exists
    if (effects.find(effect->getName()) != effects.end()) {
        // Delete old effect
        delete effects[effect->getName()];
    }
    
    // Add new effect
    effects[effect->getName()] = effect;
}

void FXLayer::removeEffect(string name) {
    auto it = effects.find(name);
    if (it != effects.end()) {
        delete it->second;
        effects.erase(it);
    }
}

Effect* FXLayer::getEffect(string name) {
    auto it = effects.find(name);
    if (it != effects.end()) {
        return it->second;
    }
    return nullptr;
}

bool FXLayer::hasEffect(string name) {
    return effects.find(name) != effects.end();
}

void FXLayer::enableEffect(string name, bool enabled) {
    auto it = effects.find(name);
    if (it != effects.end()) {
        it->second->setEnabled(enabled);
    }
}

void FXLayer::setEffectParameter(string effectName, string paramName, float value) {
    auto it = effects.find(effectName);
    if (it != effects.end()) {
        it->second->setParameter(paramName, value);
    }
}

void FXLayer::setGlobalParam(string name, float value) {
    globalParams[name] = value;
}