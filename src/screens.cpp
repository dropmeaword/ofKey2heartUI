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
    logo.loadImage("key2myheartlogo.png");
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

	int halfWidth = ofGetWidth() >> 1;
	int halfHeight = ofGetHeight() >> 1;

    float lw = ofMap((logo.width*.5), 0, logo.width, 0, ofGetScreenWidth());
    float factor = (logo.width / lw);
    float lh = logo.height / factor;
    //ofLogNotice() << "resize factor: " << factor << ", lw: " << lw << ", lh: " << lh;
	logo.draw(halfWidth - (lw/2), halfHeight + 150, lw, lh);

	string msg = "Insert USB bracelet to begin...";
	ofRectangle bbox = getSharedData().font.getStringBoundingBox(msg, 0,0);
	getSharedData().font.drawString(msg, (halfWidth - bbox.width/2), ( halfHeight - bbox.height/2) );
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

    ofLogNotice() << "installing bookshelf";
    installerBookshelf.start();
}

void PatientScreen::stateExit() {
    gui->setVisible(false);
    clearForm();
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
    button = gui->addLabelButton("Create", false);
    gui->autoSizeToFitWidgets();
    gui->setVisible(false);
    ofAddListener(gui->newGUIEvent, this, &PatientScreen::guiEvent);

    txtName->setAutoUnfocus(true);
    txtEmail->setAutoUnfocus(true);

    txtName->setFocus(true);
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

void PatientScreen::clearForm() {
    txtName->setTextString("");
    txtEmail->setTextString("");
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
                changeState("scnGetReady");
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
    bool changed = false;
    switch(key) {
        case OF_KEY_RIGHT:
            changeState("scnGetReady");
            break;
        case OF_KEY_LEFT:
            changeState("scnStart");
            break;
        case OF_KEY_TAB:
            if( txtName->isFocused() && !changed ) {
                txtName->setFocus(false);
                txtEmail->setFocus(true);
                changed = true;
            }

            if( txtEmail->isFocused() && !changed ) {
                txtName->setFocus(true);
                txtEmail->setFocus(false);
                changed = true;
            }
            break;
        case OF_KEY_RETURN:
            if( isFormFilled() ) {
                ofLogVerbose() << "(i) name: " << getSharedData().gpgName << ", email: " << getSharedData().gpgEmail;
                changeState("scnGetReady");
            } else {
                warning.message = "Form is missing something\nplease fill out in full!";
                warning.created = ofGetElapsedTimeMillis();
                warning.ttl = 10000;

                ofLogVerbose() << "Form is missing something, please fill out in full!";
            }
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
void GetReadyScreen::stateEnter() {
    getSharedData().currentState = this->getName();
    getSharedData().recording = true;
}

void GetReadyScreen::stateExit() {
}


void GetReadyScreen::setup() {
}

void GetReadyScreen::update()
{
}

void GetReadyScreen::draw()
{
    ofBackgroundGradient(ofColor::grey, ofColor::black);
	ofSetColor(255, 255, 255);
	string msg = "Thanks!\nPlease get ready in the identity booth...";
	ofRectangle bbox = getSharedData().font.getStringBoundingBox(msg, 0,0);
	getSharedData().font.drawString(msg, ((ofGetWidth() >> 1) - bbox.width/2), ((ofGetHeight() >> 1) - bbox.height/2) );
}

void GetReadyScreen::mousePressed(int x, int y, int button)
{
}

void GetReadyScreen::keyPressed(int key) {
    switch(key) {
        case OF_KEY_RIGHT:
            changeState("scnKeygen");
            break;
        case OF_KEY_LEFT:
            changeState("scnPatient");
            break;
        default:
            break;
    }
}

string GetReadyScreen::getName()
{
	return "scnGetReady";
}


// ///////////////////////////////////////////////////////////////////////////
void KeygenScreen::stateEnter() {
    wasKeyGenerated = false;
    getSharedData().recording = true;
    entered = ofGetElapsedTimeMillis();
    getSharedData().currentState = this->getName();
    //gui->setVisible(true);
	heart.play();
    heart.setLoopState(OF_LOOP_NORMAL);

    getSharedData().wasKeyGenerated = false;

    seconds = 0;
    lastUpdate = ofGetElapsedTimeMillis();
}

void KeygenScreen::stateExit() {
    getSharedData().recording = false;
    //gui->setVisible(false);
}


void KeygenScreen::setup() {
    //svg.load("drawing-heart.svg");
    //svg.load("heart2.svg");

	heart.loadMovie("heartbeat.mp4");

    showfft = false;

/*
    for(int i = 0; i < 256; i++)
    {
        entropyHistory.push_back(0.0);
    }

    // setup UI
    gui = new ofxUICanvas((ofGetScreenWidth()/2)-200, ofGetScreenHeight()/2, 400, 400);

    gui->addLabel("HEARTBEAT", OFX_UI_FONT_SMALL);
    heart = gui->addMovingGraph("grHeartbeat", entropyHistory, 256, -1.0, -1.0);
    gui->addLabel("ENTROPY", OFX_UI_FONT_SMALL);
    entropy = gui->addMovingGraph("grEntropy", entropyHistory, 256, 0.0, 4096);

    gui->autoSizeToFitWidgets();
    gui->setVisible(false);
*/
	ofSetCircleResolution(80);
}

void KeygenScreen::gpgKeyGenerate() {
    keygen.setIdentity(getSharedData().gpgName, getSharedData().gpgEmail);
    //ofLogNotice() << "Starting key generation";
    keygen.start();
}

void KeygenScreen::update()
{
    getSharedData().fft.update();
    heart.update();

    // count seconds
	if (ofGetElapsedTimeMillis() - lastUpdate > 1000)
	{
		seconds++;
		lastUpdate = ofGetElapsedTimeMillis();
	}

    // generate key 5 seconds into this screen
	if ( (ofGetElapsedTimeMillis() - entered > 5000) && (!keygen.isThreadRunning() && !wasKeyGenerated) )
	{
	    this->gpgKeyGenerate();
	}

    // take entropy data
    entropyAvailable = 1.0 * randpool::getEntropyPoolAvailable();
    entropyAvailable = ofMap(entropyAvailable, .0, getSharedData().randomPoolSize, .0, 1.0, true);

    entropyHistory.push_back( entropyAvailable );
    if( entropyHistory.size() >= 510 ){
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

    // if the key generation process is completed
	if(keygen.isDone() && !wasKeyGenerated) {
        getSharedData().wasKeyGenerated = true;
        ofLogNotice() << "key generated successfully...";
	}

    long timeLimit = 60*2+30;
	if( (seconds > timeLimit) && getSharedData().wasKeyGenerated) {
        changeState("scnThankyou");
	}
}

void KeygenScreen::drawAudio(string name, float x, float y, float w, float h, vector<float> &data) {
    ofPushMatrix();
        ofTranslate(x, y, 0);

        ofSetColor(225);
        ofDrawBitmapString(name, 4, 18);

        ofSetLineWidth(1);
        ofRect(0, 0, w, h);

		ofSetColor(0, 245, 0);
		ofSetLineWidth(3);

        ofBeginShape();
        for (unsigned int i = 0; i < data.size(); i++){
            float yp = (h/2) - data[i] * (h*0.5);
            ofVertex(i*2, yp); //100 - getSharedData().left[i]*gheight);
        }
        ofEndShape(false);
    ofPopMatrix();
}

void KeygenScreen::drawEntropy(string name, float x, float y, float w, float h, vector<float> &data) {
    ofPushMatrix();
        ofTranslate(x, y, 0);

        ofSetColor(225);
        ofDrawBitmapString(name, 4, 18);

        ofSetLineWidth(1);
        ofRect(0, 0, w, h);

		ofSetColor(0, 245, 0);
		ofSetLineWidth(3);

        //lets draw the volume history as a graph
        float step = (w-2) / data.size();
        float xp = 0;
        ofBeginShape();
        for (unsigned int i = 0; i < data.size(); i++){
            float offset = ofMap(data[i], 0, 1.0, 2, h-2);
            if (data[i] < 0.40) ofSetColor(245, 245, 0);
            if (data[i] < 0.20) ofSetColor(245, 100, 0);
            if (data[i] < 0.10) ofSetColor(245, 0, 0);
            ofVertex(i, h - offset); //100 - entropyHistory[i] * gheight);
            xp += step;
        }
        ofEndShape(false);
    ofPopMatrix();
}

void KeygenScreen::draw()
{
    ofEnableAlphaBlending();
//    ofBackgroundGradient(ofColor::grey, ofColor::black);
	ofBackground(0, 0, 0);

    if(showfft) {
        getSharedData().fft.draw(0, 0, 400, 400);
    }

	ofSetColor(225);
	ofNoFill();

    float vw = heart.getWidth() * .6;
    float vh = heart.getHeight() * .6;
    heart.draw(540, (ofGetScreenHeight()/2)-(vh/2), vw, vh);


    int xpos = 32;
    int ypos = 170;
    int gheight = 180;

    drawAudio("Heartbeat", xpos, ypos, 512, 200, getSharedData().left);

    ypos += 250;
    int gwidth = 200;

    ofSetColor(255, 255, 255, 30);
	stringstream ss;
	ss << (int)(entropyAvailable*100) << " of " << getSharedData().randomPoolSize;
	ofRectangle bbox = getSharedData().font.getStringBoundingBox(ss.str(), 0,0);
	getSharedData().font.drawString(ss.str(), (bbox.width/2)+xpos-80, (bbox.height/2)+ypos+70 );

    drawEntropy("Random pool", xpos, ypos, 512, 200, entropyHistory);

    xpos = 565;
    ypos = 170;


	getSharedData().drawCounter++;
}

void KeygenScreen::mousePressed(int x, int y, int button)
{
}

void KeygenScreen::keyPressed(int key) {
    switch(key) {
        case OF_KEY_RIGHT:
            changeState("scnThankyou");
            break;
        case OF_KEY_LEFT:
            changeState("scnPatient");
            break;
        case 'f':
            showfft = !showfft;
            break;
        default:
            break;
    }
}

string KeygenScreen::getName()
{
	return "scnKeygen";
}



// ////////////////////////////////////////////////////////////////////////////////
void ThankyouScreen::stateEnter() {
    getSharedData().currentState = this->getName();
}

void ThankyouScreen::stateExit() {
}


void ThankyouScreen::setup() {
}

void ThankyouScreen::update()
{
}

void ThankyouScreen::draw()
{
    ofBackgroundGradient(ofColor::grey, ofColor::black);
	ofSetColor(255, 255, 255);
	string msg = "Thanks!\nEnjoy your new identity.";
	ofRectangle bbox = getSharedData().font.getStringBoundingBox(msg, 0,0);
	getSharedData().font.drawString(msg, ((ofGetWidth() >> 1) - bbox.width/2), ((ofGetHeight() >> 1) - bbox.height/2) );
}

void ThankyouScreen::mousePressed(int x, int y, int button)
{
}

void ThankyouScreen::keyPressed(int key) {
    // on any key
    changeState("scnStart");
/*
    switch(key) {
        case OF_KEY_RIGHT:
            changeState("scnStart");
            break;
        case OF_KEY_LEFT:
            changeState("scnKeygen");
            break;
        default:
            break;
    }
*/
}

string ThankyouScreen::getName()
{
	return "scnThankyou";
}
