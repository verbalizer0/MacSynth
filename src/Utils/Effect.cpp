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

void Effect::savePreset(ofXml& xml) {
    ofXml enabledXml;
    enabledXml.set(enabled);
    xml.appendChild("enabled").appendChild(enabledXml);
    
    ofXml intensityXml;
    intensityXml.set(intensity);
    xml.appendChild("intensity").appendChild(intensityXml);
    
    // Save parameters
    ofXml paramsXml;
    for (auto& param : params) {
        ofXml paramXml;
        paramXml.set(param.second);
        paramsXml.appendChild(param.first).appendChild(paramXml);
    }
    xml.appendChild("parameters").appendChild(paramsXml);
}

void Effect::loadPreset(ofXml& xml) {
    // Load enabled status
    if (xml.exists("enabled")) {
        enabled = xml.getChild("enabled").getBoolValue();
    }
    
    // Load intensity
    if (xml.exists("intensity")) {
        intensity = xml.getChild("intensity").getFloatValue();
    }
    
    // Load parameters
    if (xml.exists("parameters")) {
        ofXml paramsXml = xml.getChild("parameters");
        for (auto& param : params) {
            if (paramsXml.exists(param.first)) {
                params[param.first] = paramsXml.getChild(param.first).getFloatValue();
            }
        }
    }
}