#pragma once

#include "ofxStateMachine.h"
#include "SharedData.h"
#include "ofMain.h"
#include "ofxUI.h"
#include "ofxSvg.h"

#include "shell.h"

typedef struct sMessage {
    string message;
    long created;
    long ttl;
} tMessage;

class StartScreen : public itg::ofxState<SharedData>
{
public:
    void stateEnter();
	void stateExit();
    void setup();
	void update();
	void draw();
	void mousePressed(int x, int y, int button);
    void keyPressed(int key);
	string getName();

	ofImage logo;
};

class PatientScreen : public itg::ofxState<SharedData>
{
public:
    void stateEnter();
	void stateExit();
    void setup();
	void update();
	void draw();


	void mousePressed(int x, int y, int button);
    void keyPressed(int key);
	string getName();

    ThreadBookshelf installerBookshelf;

    int btnLastValue;

    ofxUITextInput *txtName;
    ofxUITextInput *txtEmail;
    ofxUILabelButton *button;

    ofxUICanvas *gui;

    tMessage warning;

    void readForm();
	void clearForm();
    bool isFormFilled();

    void guiEvent(ofxUIEventArgs &e);
};

class GetReadyScreen : public itg::ofxState<SharedData>
{
public:
    void stateEnter();
	void stateExit();
    void setup();
	void update();
	void draw();
	void mousePressed(int x, int y, int button);
    void keyPressed(int key);
	string getName();
};

class KeygenScreen : public itg::ofxState<SharedData>
{
    float entropyAvailable;
    float entropyMax;
    vector <float> entropyHistory;

    ofVideoPlayer 		heart;

    long lastUpdate;
    long seconds;

    long entered;
    bool wasKeyGenerated;

    bool showfft;

    ThreadGenKey keygen;

    //ofxSVG svg;

//    ofxUIMovingGraph *heart;
//    ofxUIMovingGraph *entropy;
//    ofxUICanvas *gui;

public:

    void drawEntropy(string name, float x, float y, float w, float h, vector<float> &data);
    void drawAudio(string name, float x, float y, float w, float h, vector<float> &data);

    void gpgKeyGenerate();

    void stateEnter();
	void stateExit();
    void setup();
	void update();
	void draw();
	void mousePressed(int x, int y, int button);
    void keyPressed(int key);
	string getName();
};

class ThankyouScreen : public itg::ofxState<SharedData>
{
public:
    void stateEnter();
	void stateExit();
    void setup();
	void update();
	void draw();
	void mousePressed(int x, int y, int button);
    void keyPressed(int key);
	string getName();
};
