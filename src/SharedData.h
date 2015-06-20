
#pragma once

class SharedData {
public:
    int counter;
    long lastUpdate;
    ofTrueTypeFont font;

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

    string currentState;
};
