
#pragma once

class SharedData {
public:
    int counter;
    long lastUpdate;
    ofTrueTypeFont font;

    vector <float> left;
    vector <float> right;
    vector <float> volHistory;

    int 	bufferCounter;
    int 	drawCounter;

    float smoothedVol;
    float scaledVol;

};
