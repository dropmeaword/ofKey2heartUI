
#pragma once

#include "ofxFFTBase.h"

class SharedData {
public:
    int counter;
    long lastUpdate;
    ofTrueTypeFont font;

    ofxFFTBase fft;

    vector <float> left;
    vector <float> right;
    vector <float> volHistory;

    string gpgName;
    string gpgEmail;

    int 	bufferCounter;
    int 	drawCounter;

    float smoothedVol;
    float scaledVol;

    int randomPoolSize;

    bool recording;

    bool wasKeyGenerated;

    string currentState;
};
