#include "screens.h"
#include "randpool.h"

#include <Poco/Process.h>
#include <Poco/Pipe.h>
#include <Poco/PipeStream.h>
#include <Poco/StreamCopier.h>

#include <sstream>

using namespace Poco;
using Poco::Process;
using Poco::Pipe;
using Poco::PipeInputStream;
using Poco::PipeOutputStream;
using Poco::ProcessHandle;
using Poco::StreamCopier;

void StartScreen::stateEnter() {
    getSharedData().currentState = this->getName();
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
}

void StartScreen::keyPressed(int key) {
    switch(key) {
        case OF_KEY_RIGHT:
            changeState("scnPatient");
            break;
        case OF_KEY_LEFT:
            changeState("scnStart");
            break;
        default:
            break;
    }
}

string StartScreen::getName()
{
	return "scnStart";
}


// ///////////////////////////////////////////////////////////////////////////
void PatientScreen::stateEnter() {
    getSharedData().currentState = this->getName();
    gui->setVisible(true);

    installerBookshelf.start();
}

void PatientScreen::stateExit() {
    gui->setVisible(false);
}

void PatientScreen::setup() {
    // setup UI
    gui = new ofxUICanvas((ofGetScreenWidth()/2)-200, ofGetScreenHeight()/2, 400, 400);

    gui->addLabel("CREATE IDENTITY", OFX_UI_FONT_LARGE);
    gui->addSpacer(8);
    gui->addLabel("ENTER NAME", OFX_UI_FONT_SMALL);
    txtName = gui->addTextInput("TXT_NAME", "");
    gui->addLabel("ENTER EMAIL", OFX_UI_FONT_SMALL);
    txtEmail = gui->addTextInput("TXT_EMAIL", "");
    gui->addSpacer(8);
    gui->addLabelButton("Create", false);
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
    ofBackground(30, 30, 60);
	ofSetColor(255, 255, 255);

    if(!warning.message.empty() && ((ofGetElapsedTimeMillis() - warning.created) < warning.ttl) ) {
        ofSetColor(255, 0, 0);
		ofDrawBitmapString(warning.message, (ofGetWidth() >> 1)-120, (ofGetScreenHeight()/2) + 200);
    }
	//getSharedData().font.drawString(ofToString(getSharedData().counter), ofGetWidth() >> 1, ofGetHeight() >> 1);
}

void PatientScreen::readForm() {
    getSharedData().gpgName  = txtName->getTextString();
    getSharedData().gpgEmail = txtEmail->getTextString();
}

bool PatientScreen::isFormFilled() {
    readForm();
    if(!getSharedData().gpgName.empty() && !getSharedData().gpgEmail.empty()) {
        return true;
    } else {
        return false;
    }
}

void PatientScreen::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();

    ofLogVerbose() <<  "GUI element: " << name;

	if(kind == OFX_UI_WIDGET_LABELBUTTON)
    {
        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
        if( (button->getValue() == 0) && (btnLastValue == 1) ) {
            if( isFormFilled() ) {
                ofLogVerbose() << "(i) name: " << getSharedData().gpgName << ", email: " << getSharedData().gpgEmail;
            } else {
                warning.message = "Form is missing something\nplease fill out in full!";
                warning.created = ofGetElapsedTimeMillis();
                warning.ttl = 10000;

                ofLogVerbose() << "Form is missing something, please fill out in full!";
            }
        }

        btnLastValue = button->getValue();
        ofLogVerbose() << name << "\t value: " << btnLastValue << endl;
    }
}


void PatientScreen::mousePressed(int x, int y, int button)
{
}

void PatientScreen::keyPressed(int key) {
    switch(key) {
        case OF_KEY_RIGHT:
            changeState("scnKeygen");
            break;
        case OF_KEY_LEFT:
            changeState("scnStart");
            break;
        default:
            break;
    }
}

string PatientScreen::getName()
{
	return "scnPatient";
}


// ///////////////////////////////////////////////////////////////////////////
void KeygenScreen::stateEnter() {
    wasKeyGenerated = false;
    entered = ofGetElapsedTimeMillis();
    getSharedData().currentState = this->getName();
}

void KeygenScreen::stateExit() {
}


void KeygenScreen::setup() {
	ofSetCircleResolution(80);
}

void KeygenScreen::gpgKeyGenerate() {
    std::string cmd("oftest");
    std::vector<std::string> args;
    args.push_back("-ax");
    Poco::Pipe outPipe;
    ProcessHandle ph = Process::launch(cmd, args, 0, &outPipe, 0);
    Poco::PipeInputStream istr(outPipe);
    //ph.wait();

    stringstream out;

    Poco::StreamCopier::copyStream(istr, out);

    ofLogVerbose() << "output of command: " << out.str();

    wasKeyGenerated = true;
}

void KeygenScreen::update()
{
	if ( (ofGetElapsedTimeMillis() - entered > 5000) && (!wasKeyGenerated) )
	{
	    this->gpgKeyGenerate();
	}

    entropyAvailable = 1.0 * randpool::getEntropyPoolAvailable();
    entropyAvailable = ofMap(entropyAvailable, .0, getSharedData().randomPoolSize, .0, 1.0, true);

	entropyHistory.push_back( entropyAvailable );
	if( entropyHistory.size() >= 400 ){
		entropyHistory.erase(entropyHistory.begin(), entropyHistory.begin()+1);
	}

	//lets scale the vol up to a 0-1 range
	getSharedData().scaledVol = ofMap(getSharedData().smoothedVol, 0.0, 0.3, 0.0, 1.0, true);

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

	ofNoFill();

	// draw the left channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 170, 0);

		ofSetColor(225);
		ofDrawBitmapString("Heartbeat", 4, 18);

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
		ofDrawBitmapString("Random pool", 4, 18);

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
		//ofDrawBitmapString("Scaled average vol (0-100): " + ofToString(getSharedData().scaledVol * 100.0, 0), 4, 18);
		//ofRect(0, 0, 400, 400);

		ofSetColor(245, 58, 135);
		ofFill();
		ofCircle(200, 200, getSharedData().scaledVol * 120.0f);

		//lets draw the volume history as a graph
		ofBeginShape();
		for (unsigned int i = 0; i < entropyHistory.size(); i++){
			if( i == 0 ) ofVertex(i, 400);

			ofVertex(i, 400 - entropyHistory[i] * 70);

			if( i == entropyHistory.size() -1 ) ofVertex(i, 400);
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
}

void KeygenScreen::keyPressed(int key) {
    switch(key) {
        case OF_KEY_RIGHT:
            changeState("scnStart");
            break;
        case OF_KEY_LEFT:
            changeState("scnPatient");
            break;
        default:
            break;
    }
}

string KeygenScreen::getName()
{
	return "scnKeygen";
}



