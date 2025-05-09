// File: src/Utils/Effect.cpp
#include "Effect.h"

Effect::Effect(string name) {
    this->name = name;
    enabled = true;
    intensity = 1.0;
    width = 1280;
    height = 720;
}

Effect::~Effect() {
    // Clean up resources
}

void Effect::setup(int width, int height) {
    this->width = width;
    this->height = height;
}

void Effect::update(float phase, float* audioData, int numBands, map<string, float>& globalParams) {
    // Base implementation does nothing
    // This should be overridden by derived classes
}

bool Effect::setParameter(string name, float value) {
    // Check if parameter exists
    if (params.find(name) != params.end()) {
        params[name] = value;
        return true;
    }
    return false;
}

float Effect::getParameter(string name) {
    // Check if parameter exists
    if (params.find(name) != params.end()) {
        return params[name];
    }
    return 0.0f;
}

void Effect::ensureParameter(string name, float defaultValue) {
    // Create parameter if it doesn't exist
    if (params.find(name) == params.end()) {
        params[name] = defaultValue;
    }
}

float Effect::getAudioEnergy(float* audioData, int numBands, string range) {
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

// Fixed savePreset method for Effect.cpp
void Effect::savePreset(ofXml& xml) {
    // Save enabled status and intensity
    xml.appendChild("enabled").set(ofToString(enabled));
    xml.appendChild("intensity").set(ofToString(intensity));
    
    // Save parameters
    ofXml paramsXml;
    for (auto& param : params) {
        paramsXml.appendChild(param.first).set(ofToString(param.second));
    }
    xml.appendChild("parameters").appendChild(paramsXml);
}

// Fixed loadPreset method for Effect.cpp
void Effect::loadPreset(ofXml& xml) {
    // Load enabled status
    auto enabledNode = xml.find("enabled");
    if (enabledNode.size() > 0) {
        enabled = ofToBool(xml.getChild("enabled").getValue());
    }
    
    // Load intensity
    auto intensityNode = xml.find("intensity");
    if (intensityNode.size() > 0) {
        intensity = ofToFloat(xml.getChild("intensity").getValue());
    }
    
    // Load parameters
    auto paramsNode = xml.find("parameters");
    if (paramsNode.size() > 0) {
        ofXml paramsXml = xml.getChild("parameters");
        auto paramNodes = paramsXml.getChildren();
        
        for (auto& paramNode : paramNodes) {
            string paramName = paramNode.getName();
            float paramValue = ofToFloat(paramNode.getValue());
            
            // Only set if the parameter exists in our map
            if (params.find(paramName) != params.end()) {
                params[paramName] = paramValue;
            }
        }
    }
}