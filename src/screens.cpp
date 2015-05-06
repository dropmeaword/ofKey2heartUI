
#include "screens.h"

void StartScreen::stateEnter() {
}

void StartScreen::stateExit() {
}


void StartScreen::setup() {
}

void StartScreen::update()
{
	if (ofGetElapsedTimeMillis() - getSharedData().lastUpdate > 1000)
	{
		getSharedData().counter--;
		getSharedData().lastUpdate = ofGetElapsedTimeMillis();
	}
}

void StartScreen::draw()
{
    ofBackgroundGradient(ofColor::grey, ofColor::black);
	ofSetColor(255, 255, 255);
	string msg = "Insert USB bracelet to begin...";
	ofRectangle bbox = getSharedData().font.getStringBoundingBox(msg, 0,0);
	getSharedData().font.drawString(msg, ((ofGetWidth() >> 1) - bbox.width/2), ((ofGetHeight() >> 1) - bbox.height/2) );
}

void StartScreen::mousePressed(int x, int y, int button)
{
	changeState("scnPatient");
}

string StartScreen::getName()
{
	return "scnStart";
}


// ///////////////////////////////////////////////////////////////////////////
void PatientScreen::stateEnter() {
    gui->setVisible(true);
}

void PatientScreen::stateExit() {
    gui->setVisible(false);
}

void PatientScreen::setup() {
    // setup UI
    gui = new ofxUICanvas();

    gui->addLabel("OFXUI TUTORIAL", OFX_UI_FONT_LARGE);
    gui->addSlider("BACKGROUND",0.0,255.0,100.0);
    gui->addToggle("FULLSCREEN", false);
    gui->autoSizeToFitWidgets();
    gui->setVisible(false);
    ofAddListener(gui->newGUIEvent, this, &PatientScreen::guiEvent);
}

void PatientScreen::update()
{
	if (ofGetElapsedTimeMillis() - getSharedData().lastUpdate > 1000)
	{
		getSharedData().counter--;
		getSharedData().lastUpdate = ofGetElapsedTimeMillis();
	}
}

void PatientScreen::draw()
{
    ofBackgroundGradient(ofColor::grey, ofColor::black);
	ofSetColor(255, 255, 255);
	getSharedData().font.drawString(ofToString(getSharedData().counter), ofGetWidth() >> 1, ofGetHeight() >> 1);
}

void PatientScreen::guiEvent(ofxUIEventArgs &e)
{
    if(e.getName() == "BACKGROUND")
    {
        ofxUISlider *slider = e.getSlider();
        ofBackground(slider->getScaledValue());
    }
    else if(e.getName() == "FULLSCREEN")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        ofSetFullscreen(toggle->getValue());
    }
}


void PatientScreen::mousePressed(int x, int y, int button)
{
	changeState("scnKeygen");
}

string PatientScreen::getName()
{
	return "scnPatient";
}


// ///////////////////////////////////////////////////////////////////////////
void KeygenScreen::stateEnter() {
}

void KeygenScreen::stateExit() {
}


void KeygenScreen::setup() {
	ofSetCircleResolution(80);
}

void KeygenScreen::update()
{
	//lets scale the vol up to a 0-1 range
	getSharedData().scaledVol = ofMap(getSharedData().smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

	//lets record the volume into an array
	getSharedData().volHistory.push_back( getSharedData().scaledVol );

	//if we are bigger the the size we want to record - lets drop the oldest value
	if( getSharedData().volHistory.size() >= 400 ){
		getSharedData().volHistory.erase(getSharedData().volHistory.begin(), getSharedData().volHistory.begin()+1);
	}
}

void KeygenScreen::draw()
{
    ofBackgroundGradient(ofColor::grey, ofColor::black);

	ofSetColor(225);
	ofDrawBitmapString("AUDIO INPUT EXAMPLE", 32, 32);
	ofDrawBitmapString("press 's' to unpause the audio\n'e' to pause the audio", 31, 92);

	ofNoFill();

	// draw the left channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 170, 0);

		ofSetColor(225);
		ofDrawBitmapString("Left Channel", 4, 18);

		ofSetLineWidth(1);
		ofRect(0, 0, 512, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);

			ofBeginShape();
			for (unsigned int i = 0; i < getSharedData().left.size(); i++){
				ofVertex(i*2, 100 -getSharedData().left[i]*180.0f);
			}
			ofEndShape(false);

		ofPopMatrix();
	ofPopStyle();

	// draw the right channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 370, 0);

		ofSetColor(225);
		ofDrawBitmapString("Right Channel", 4, 18);

		ofSetLineWidth(1);
		ofRect(0, 0, 512, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);

			ofBeginShape();
			for (unsigned int i = 0; i < getSharedData().right.size(); i++){
				ofVertex(i*2, 100 -getSharedData().right[i]*180.0f);
			}
			ofEndShape(false);

		ofPopMatrix();
	ofPopStyle();

	// draw the average volume:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(565, 170, 0);

		ofSetColor(225);
		ofDrawBitmapString("Scaled average vol (0-100): " + ofToString(getSharedData().scaledVol * 100.0, 0), 4, 18);
		ofRect(0, 0, 400, 400);

		ofSetColor(245, 58, 135);
		ofFill();
		ofCircle(200, 200, getSharedData().scaledVol * 190.0f);

		//lets draw the volume history as a graph
		ofBeginShape();
		for (unsigned int i = 0; i < getSharedData().volHistory.size(); i++){
			if( i == 0 ) ofVertex(i, 400);

			ofVertex(i, 400 - getSharedData().volHistory[i] * 70);

			if( i == getSharedData().volHistory.size() -1 ) ofVertex(i, 400);
		}
		ofEndShape(false);

		ofPopMatrix();
	ofPopStyle();

	getSharedData().drawCounter++;

//	ofSetColor(225);
//	string reportString = "buffers received: "+ofToString(getSharedData().bufferCounter)+"\ndraw routines called: "+ofToString(getSharedData().drawCounter)+"\nticks: " + ofToString(soundStream.getTickCount());
//	ofDrawBitmapString(reportString, 32, 589);
}

void KeygenScreen::mousePressed(int x, int y, int button)
{
	changeState("scnStart");
}

string KeygenScreen::getName()
{
	return "scnKeygen";
}



