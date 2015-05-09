#include "ofApp.h"

#define LISTEN_OSC_PORT 12345

using namespace randpool;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);

	ofSetVerticalSync(true);
	ofBackground(54, 54, 54);

	state.getSharedData().randomPoolSize = getEntropyPoolSize();

    ofLogVerbose() << "Poolsize: " << state.getSharedData().randomPoolSize;
    ofLogVerbose() << "Entropy available: " << getEntropyPoolAvailable();

	// OSC listen on the given port
	ofLogNotice() << "OSC: listening for osc messages on port " << LISTEN_OSC_PORT << "\n";
	oscr.setup(LISTEN_OSC_PORT);

    // state machine
	state.getSharedData().counter = 0;
	state.getSharedData().lastUpdate = ofGetElapsedTimeMillis();
	state.getSharedData().font.loadFont("vag.ttf", 50);

	state.addState<StartScreen>();
	state.addState<PatientScreen>();
	state.addState<GetReadyScreen>();
	state.addState<KeygenScreen>();
	state.changeState("scnStart");

    // AUDIO INPUT

	// 0 output channels,
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)
	soundStream.listDevices();

	//if you want to set a different device id
	//soundStream.setDeviceID(0); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.

	int bufferSize = 256;

	state.getSharedData().left.assign(bufferSize, 0.0);
	state.getSharedData().right.assign(bufferSize, 0.0);
	state.getSharedData().volHistory.assign(400, 0.0);

	state.getSharedData().bufferCounter	= 0;
	state.getSharedData().drawCounter		= 0;
	state.getSharedData().smoothedVol     = 0.0;
	state.getSharedData().scaledVol		= 0.0;

	soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
}

//--------------------------------------------------------------
void ofApp::update(){
    oscLoop();
}

void ofApp::oscLoop() {
	while(oscr.hasWaitingMessages()){
		ofxOscMessage m;
		oscr.getNextMessage(&m);

		if(m.getAddress() == "/k2h/thumbdrive"){
            ofLogVerbose() << "Thumbdrive detected";
			string action = m.getArgAsString(0);
			if(action == "add" && state.getSharedData().currentState == "scnStart") {
                ofLogVerbose() << "Changing state";
                state.changeState("scnPatient");
			}
		}
	} // while
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(0, 0, 0);
    ofDrawBitmapString("Mouse click changes state", 20, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    if( key == OF_KEY_F1) {
        ofToggleFullscreen();
    }
/*
	if( key == 's' ){
		soundStream.start();
	}

	if( key == 'e' ){
		soundStream.stop();
	}
*/
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){

	float curVol = 0.0;

	// samples are "interleaved"
	int numCounted = 0;

	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume
	for (int i = 0; i < bufferSize; i++){
		state.getSharedData().left[i]		= input[i*2]*0.5;
		state.getSharedData().right[i]	= input[i*2+1]*0.5;

		curVol += state.getSharedData().left[i] * state.getSharedData().left[i];
		curVol += state.getSharedData().right[i] * state.getSharedData().right[i];
		numCounted+=2;
	}

	//this is how we get the mean of rms :)
	curVol /= (float)numCounted;

	// this is how we get the root of rms :)
	curVol = sqrt( curVol );

	state.getSharedData().smoothedVol *= 0.93;
	state.getSharedData().smoothedVol += 0.07 * curVol;

	state.getSharedData().bufferCounter++;
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

