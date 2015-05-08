#pragma once

#include "ofxStateMachine.h"
#include "SharedData.h"
#include "ofMain.h"
#include "ofxUI.h"

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

    ofxUICanvas *gui;
    void guiEvent(ofxUIEventArgs &e);
};

class KeygenScreen : public itg::ofxState<SharedData>
{
    float entropyAvailable;
    vector <float> entropyHistory;

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
