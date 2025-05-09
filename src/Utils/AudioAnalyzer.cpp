// File: src/Utils/AudioAnalyzer.cpp
#include "AudioAnalyzer.h"

//--------------------------------------------------------------
// BeatDetector Implementation
//--------------------------------------------------------------

BeatDetector::BeatDetector() {
    bpm = 120;
    confidence = 0;
    phase = 0;
    onBeat = false;
    
    // Beat detection parameters
    beatThreshold = 0.15;
    minBeatInterval = 300; // Minimum ms between beats (200 BPM)
    lastBeatTime = 0;
    
    maxBeatTimes = 10;
}

BeatDetector::~BeatDetector() {
    // Clean up resources
}

void BeatDetector::setup(int bufferSize) {
    // Initialize energy history buffer (about 1 second worth)
    int historySize = 43; // About 1 second of history at 44.1kHz
    energyHistory.resize(historySize, 0);
    
    beatHistory.clear();
    beatTimes.clear();
}

void BeatDetector::update(float* spectrum, int numBands, float* waveform, int bufferSize) {
    float now = ofGetElapsedTimeMillis();
    
    // Calculate bass energy
    float bassEnergy = 0;
    int bassBands = min(numBands / 4, 4); // Use first few bands for bass
    
    for (int i = 0; i < bassBands; i++) {
        bassEnergy += spectrum[i];
    }
    bassEnergy /= bassBands;
    
    // Add to energy history
    energyHistory.push_back(bassEnergy);
    if (energyHistory.size() > 43) {
        energyHistory.erase(energyHistory.begin());
    }
    
    // Calculate average energy
    float avgEnergy = 0;
    for (auto e : energyHistory) {
        avgEnergy += e;
    }
    avgEnergy /= energyHistory.size();
    
    // Check for beat
    onBeat = false;
    if (bassEnergy > avgEnergy + beatThreshold && 
        bassEnergy > 0.1 && 
        now - lastBeatTime > minBeatInterval) {
        
        // Beat detected
        onBeat = true;
        
        // Calculate time since last beat
        float beatInterval = now - lastBeatTime;
        lastBeatTime = now;
        
        // Only keep reasonable intervals (300-2000ms = 30-200 BPM)
        if (beatInterval >= 300 && beatInterval <= 2000) {
            beatHistory.push_back(beatInterval);
            
            // Keep only recent beats
            if (beatHistory.size() > 8) {
                beatHistory.erase(beatHistory.begin());
            }
            
            // Store beat time for BPM calculation
            beatTimes.push_back(now);
            if (beatTimes.size() > maxBeatTimes) {
                beatTimes.erase(beatTimes.begin());
            }
            
            // Calculate BPM if we have enough beats
            if (beatTimes.size() >= 4) {
                vector<float> intervals;
                for (int i = 1; i < beatTimes.size(); i++) {
                    intervals.push_back(beatTimes[i] - beatTimes[i-1]);
                }
                
                // Average interval
                float avgInterval = 0;
                for (auto i : intervals) {
                    avgInterval += i;
                }
                avgInterval /= intervals.length();
                
                // Convert to BPM
                float newBpm = 60000 / avgInterval;
                
                // Only update BPM if it's in a reasonable range (40-200 BPM)
                if (newBpm >= 40 && newBpm <= 200) {
                    // Smooth BPM changes
                    bpm = bpm * 0.8 + newBpm * 0.2;
                    confidence = min(1.0f, beatTimes.size() / (float)maxBeatTimes);
                }
            }
        }
    }
    
    // Update phase based on current BPM
    float beatInterval = 60000 / bpm;
    phase = fmodf((now - lastBeatTime), beatInterval) / beatInterval;
}

//--------------------------------------------------------------
// AudioAnalyzer Implementation
//--------------------------------------------------------------

AudioAnalyzer::AudioAnalyzer() {
    bufferSize = 1024;
    sampleRate = 44100;
    numBands = bufferSize / 2;
    inputGain = 1.0;
    inputReady = false;
    energy = 0;
    
    // Allocate arrays
    spectrum = new float[numBands];
    waveform = new float[bufferSize];
    
    // Initialize arrays
    for (int i = 0; i < numBands; i++) {
        spectrum[i] = 0;
    }
    
    for (int i = 0; i < bufferSize; i++) {
        waveform[i] = 0;
    }
    
    // Initialize audio buffer
    audioBuffer.resize(bufferSize, 0);
    
    // Initialize band levels and thresholds
    bandLevels["bass"] = 0;
    bandLevels["lowMid"] = 0;
    bandLevels["mid"] = 0;
    bandLevels["highMid"] = 0;
    bandLevels["high"] = 0;
    
    bandThresholds["bass"] = 0.6;
    bandThresholds["lowMid"] = 0.5;
    bandThresholds["mid"] = 0.4;
    bandThresholds["highMid"] = 0.3;
    bandThresholds["high"] = 0.2;
    
    bandTriggers["bass"] = false;
    bandTriggers["lowMid"] = false;
    bandTriggers["mid"] = false;
    bandTriggers["highMid"] = false;
    bandTriggers["high"] = false;
}

AudioAnalyzer::~AudioAnalyzer() {
    // Clean up resources
    delete[] spectrum;
    delete[] waveform;
    
    // Close sound stream
    soundStream.close();
}

void AudioAnalyzer::setup() {
    // Initialize beat detector
    beatDetector.setup(bufferSize);
    
    // Try to setup default input
    setupMicrophone();
}

bool AudioAnalyzer::setupMicrophone(int deviceId) {
    // Close any existing sound stream
    soundStream.close();
    
    // Set up sound stream with device ID
    ofSoundStreamSettings settings;
    
    // Get audio devices
    auto devices = soundStream.getMatchingDevices("", true, false);
    
    if (devices.empty()) {
        ofLogError("AudioAnalyzer") << "No input devices found";
        inputReady = false;
        return false;
    }
    
    // Use specified device ID or default to first device
    if (deviceId >= 0 && deviceId < devices.size()) {
        settings.setInDevice(devices[deviceId]);
    } else {
        settings.setInDevice(devices[0]);
    }
    
    // Configure sound stream
    settings.setInListener(this);
    settings.sampleRate = sampleRate;
    settings.numOutputChannels = 0;
    settings.numInputChannels = 1;
    settings.bufferSize = bufferSize;
    
    // Setup sound stream
    bool success = soundStream.setup(settings);
    
    if (success) {
        inputReady = true;
        ofLogNotice("AudioAnalyzer") << "Microphone setup: " << devices[deviceId].name;
    } else {
        inputReady = false;
        ofLogError("AudioAnalyzer") << "Failed to setup microphone";
    }
    
    return success;
}

bool AudioAnalyzer::setupLineInput(int deviceId) {
    // Same as setupMicrophone but with different settings if needed
    return setupMicrophone(deviceId);
}

void AudioAnalyzer::setInputGain(float gain) {
    inputGain = ofClamp(gain, 0, 4.0);
}

void AudioAnalyzer::audioIn(ofSoundBuffer& input) {
    // Copy audio buffer with gain applied
    for (int i = 0; i < bufferSize && i < input.getNumFrames(); i++) {
        audioBuffer[i] = input[i] * inputGain;
    }
}

void AudioAnalyzer::update() {
    if (!inputReady) return;
    
    // Perform FFT on audio buffer
    float* fftBuffer = new float[bufferSize];
    for (int i = 0; i < bufferSize; i++) {
        fftBuffer[i] = audioBuffer[i];
    }
    
    // Apply window function to reduce spectral leakage
    for (int i = 0; i < bufferSize; i++) {
        // Hann window
        float window = 0.5 * (1 - cos(TWO_PI * i / (bufferSize - 1)));
        fftBuffer[i] *= window;
    }
    
    // Compute FFT
    // Note: In a real implementation, this would use ofxFft or similar addon
    // For this example, we'll simulate FFT results
    for (int i = 0; i < numBands; i++) {
        // Generate fake spectrum data for demonstration
        // In a real implementation, this would use actual FFT results
        float freq = (float)i / numBands;
        float level = 0.0;
        
        // Simulate some energy in different frequency ranges
        if (i < numBands / 8) {
            // Bass (low frequencies)
            level = ofNoise(ofGetElapsedTimef() * 2, i * 0.1) * 0.8;
        } else if (i < numBands / 4) {
            // Low mids
            level = ofNoise(ofGetElapsedTimef() * 1.5, i * 0.05) * 0.6;
        } else if (i < numBands / 2) {
            // Mids
            level = ofNoise(ofGetElapsedTimef(), i * 0.02) * 0.5;
        } else {
            // Highs
            level = ofNoise(ofGetElapsedTimef() * 0.8, i * 0.01) * 0.3;
        }
        
        // Smooth spectrum values
        spectrum[i] = spectrum[i] * 0.8 + level * 0.2;
    }
    
    // Copy audio buffer to waveform
    for (int i = 0; i < bufferSize; i++) {
        waveform[i] = audioBuffer[i];
    }
    
    // Calculate band levels
    calculateBandLevels();
    
    // Calculate overall energy
    calculateEnergy();
    
    // Update triggers
    updateTriggers();
    
    // Update beat detector
    beatDetector.update(spectrum, numBands, waveform, bufferSize);
    
    // Clean up
    delete[] fftBuffer;
}

void AudioAnalyzer::calculateBandLevels() {
    // Define frequency bands
    int bassStart = 0;
    int bassEnd = numBands / 8;
    
    int lowMidStart = numBands / 8;
    int lowMidEnd = numBands / 4;
    
    int midStart = numBands / 4;
    int midEnd = numBands / 2;
    
    int highMidStart = numBands / 2;
    int highMidEnd = numBands * 3 / 4;
    
    int highStart = numBands * 3 / 4;
    int highEnd = numBands - 1;
    
    // Calculate average energy in each band
    float bassSum = 0;
    for (int i = bassStart; i <= bassEnd; i++) {
        bassSum += spectrum[i];
    }
    bandLevels["bass"] = bassSum / (bassEnd - bassStart + 1);
    
    float lowMidSum = 0;
    for (int i = lowMidStart; i <= lowMidEnd; i++) {
        lowMidSum += spectrum[i];
    }
    bandLevels["lowMid"] = lowMidSum / (lowMidEnd - lowMidStart + 1);
    
    float midSum = 0;
    for (int i = midStart; i <= midEnd; i++) {
        midSum += spectrum[i];
    }
    bandLevels["mid"] = midSum / (midEnd - midStart + 1);
    
    float highMidSum = 0;
    for (int i = highMidStart; i <= highMidEnd; i++) {
        highMidSum += spectrum[i];
    }
    bandLevels["highMid"] = highMidSum / (highMidEnd - highMidStart + 1);
    
    float highSum = 0;
    for (int i = highStart; i <= highEnd; i++) {
        highSum += spectrum[i];
    }
    bandLevels["high"] = highSum / (highEnd - highStart + 1);
}

void AudioAnalyzer::calculateEnergy() {
    float sum = 0;
    for (int i = 0; i < numBands; i++) {
        sum += spectrum[i];
    }
    energy = sum / numBands;
}

void AudioAnalyzer::updateTriggers() {
    for (auto& trigger : bandTriggers) {
        string band = trigger.first;
        trigger.second = bandLevels[band] > bandThresholds[band];
    }
}

float AudioAnalyzer::getBandEnergy(string band) {
    if (bandLevels.find(band) != bandLevels.end()) {
        return bandLevels[band];
    }
    return 0.0;
}

bool AudioAnalyzer::getTrigger(string band) {
    if (bandTriggers.find(band) != bandTriggers.end()) {
        return bandTriggers[band];
    }
    return false;
}