// File: src/Utils/AudioAnalyzer.h
#pragma once

#include "ofMain.h"

class BeatDetector {
public:
    BeatDetector();
    ~BeatDetector();
    
    void setup(int bufferSize);
    void update(float* spectrum, int numBands, float* waveform, int bufferSize);
    
    float getBPM() { return bpm; }
    float getConfidence() { return confidence; }
    float getPhase() { return phase; }
    bool isOnBeat() { return onBeat; }
    
private:
    float bpm;
    float confidence;
    float phase;
    bool onBeat;
    
    // Beat detection parameters
    vector<float> energyHistory;
    vector<float> beatHistory;
    float beatThreshold;
    float minBeatInterval;
    float lastBeatTime;
    
    // For calculating BPM
    vector<float> beatTimes;
    int maxBeatTimes;
};

class AudioAnalyzer : public ofBaseSoundInput {
public:
    AudioAnalyzer();
    ~AudioAnalyzer();
    
    void setup();
    void update();
    
    // Audio input setup
    bool setupMicrophone(int deviceId = 0);
    bool setupLineInput(int deviceId = 0);
    void setInputGain(float gain);
    
    // Get audio data
    float* getSpectrum() { return spectrum; }
    float* getWaveform() { return waveform; }
    int getNumBands() { return numBands; }
    
    // Get band energy
    float getBandEnergy(string band);
    
    // Get FFT settings
    int getBufferSize() { return bufferSize; }
    int getSampleRate() { return sampleRate; }
    
    // Beat detection
    float getBPM() { return beatDetector.getBPM(); }
    float getBeatPhase() { return beatDetector.getPhase(); }
    bool isOnBeat() { return beatDetector.isOnBeat(); }
    
    // Get overall energy
    float getEnergy() { return energy; }
    
    // Check if input is working
    bool hasInput() { return inputReady; }
    
    // Trigger states for different bands
    bool getTrigger(string band);
    
    // Make audioIn public as it needs to be accessible by ofSoundStreamSettings
    void audioIn(ofSoundBuffer& input);
    
private:
    // Audio input
    ofSoundStream soundStream;
    vector<float> audioBuffer;
    int bufferSize;
    int sampleRate;
    float inputGain;
    bool inputReady;
    
    // FFT analysis
    float* spectrum;
    float* waveform;
    int numBands;
    
    // Band levels and triggers
    map<string, float> bandLevels;
    map<string, float> bandThresholds;
    map<string, bool> bandTriggers;
    
    // Overall energy
    float energy;
    
    // Beat detection
    BeatDetector beatDetector;
    
    // Calculate band levels
    void calculateBandLevels();
    
    // Calculate triggers
    void updateTriggers();
    
    // Calculate overall energy
    void calculateEnergy();
};