#include "ofApp.h"

#define LISTEN_OSC_PORT 12345

using namespace randpool;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);

    if( geteuid() != 0) {
        ofLogWarning() << "(!!!)";
        ofLogWarning() << "(!!!) HACKETY HACK! THIS GOTTA RUN AS ROOT!";
        ofLogWarning() << "(!!!) otherwise this software cannot add to the entropy pool";
        ofLogWarning() << "(!!!) this is worse than bad, I know.";
        ofLogWarning() << "(!!!)";
    }

    int res;
    //res = kernel_rng_open();
    pool.open("/dev/urandom");
    ofLogVerbose() << "Opening kernel rand pool result = " << res;

	ofSetVerticalSync(true);
	ofBackground(54, 54, 54);

    state.getSharedData().recording = false;
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
	state.addState<ThankyouScreen>();
	state.changeState("scnStart");
	//state.changeState("scnKeygen");

    // AUDIO INPUT

	// 0 output channels,
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)
	soundStream.listDevices();

	//if you want to set a different device id
	soundStream.setDeviceID(3); // bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.

	int bufferSize = 256;

	state.getSharedData().left.assign(bufferSize, 0.0);
	state.getSharedData().right.assign(bufferSize, 0.0);
	state.getSharedData().volHistory.assign(400, 0.0);

	state.getSharedData().bufferCounter	= 0;
	state.getSharedData().drawCounter		= 0;
	state.getSharedData().smoothedVol     = 0.0;
	state.getSharedData().scaledVol		= 0.0;

    trigger = false;
    poolfeed = new unsigned char[512];
    //poolfeed.resize(bufferSize*2);
    //poolfeed = new unsigned char[bufferSize*4];  // times number of channels

	//soundStream.listDevices();
	//if you want to set the device id to be different than the default
	//soundStream.setDeviceID(1); 	//note some devices are input only and some are output only

    // bool ofSoundStream::setup(ofBaseApp *app, int outChannels, int inChannels, int sampleRate, int bufferSize, int nBuffers)
	int ret = soundStream.setup(this, 2, 2, 44100, bufferSize, 4);
    ofLogVerbose() << "soundStream.setup: " << ret;
}

void ofApp::exit() {
    ofLogVerbose() << "Closing kernel RNG";
    //kernel_rng_close();
    pool.close();
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

		if(m.getAddress() == "/k2h/done"){
            ofLogVerbose() << "Keys have been generated";
			string action = m.getArgAsString(0);
			if(state.getSharedData().currentState == "scnKeygen") {
                ofLogVerbose() << "Changing state";
                state.changeState("scnThankyou");
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

    // bufferSize 256, nChannels 2
    //ofLogVerbose() << "audioIn, bufferSize: " << bufferSize << " nChannels " << nChannels;

	// samples are "interleaved"
	int numCounted = 0;

    //kernel_rng_add_entropy();

    int bcount = 0;
	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume
	for (int i = 0; i < bufferSize; i++){
		state.getSharedData().left[i]   = input[i*2]*0.5;
		state.getSharedData().right[i]	= input[i*2+1]*0.5;

        // convert to bytes of entropy
		poolfeed[i*2] =  (unsigned char)255*input[i*2];
		//unsigned char uc1 = 255*input[i*2];
		//poolfeed[i*2] = uc1;
		//if(!trigger) ofLogVerbose() << "uc1 = " << 255*input[i*2]; //(int)poolfeed[i*2];
		//poolfeed.push_back(uc1);
		poolfeed[i*2+1] =  (unsigned char)255*input[i*2+1];
		//unsigned char uc2 = 255*input[i*2+1];
		//if(!trigger) ofLogVerbose() << "uc2 = " << 255*input[i*2+1]; //(int)poolfeed[i*2+1];

		//poolfeed.push_back(uc2);

		curVol += state.getSharedData().left[i] * state.getSharedData().left[i];
		curVol += state.getSharedData().right[i] * state.getSharedData().right[i];
		numCounted+=2;
	}

    //if(!trigger) {
    //    ofLogVerbose() << "byte count = " << bcount;
        trigger = true;
    //}

    //kernel_rng_add_entropy(poolfeed, bufferSize*2, ((bufferSize*2)<<3) ); // yolo
    if( state.getSharedData().recording ) {
        pool.add_entropy(poolfeed, bufferSize*2, ((bufferSize*2)<<3));
    }

	//this is how we get the mean of rms :)
	curVol /= (float)numCounted;

	// this is how we get the root of rms :)
	curVol = sqrt( curVol );

	state.getSharedData().smoothedVol *= 0.93;
	state.getSharedData().smoothedVol += 0.07 * curVol;

	state.getSharedData().bufferCounter++;

	//poolfeed.clear();
}

//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels){
    for (int i = 0; i < bufferSize; i++){
        if( state.getSharedData().recording ) {
            output[i*2    ] = state.getSharedData().left[i];
            output[i*2 + 1] = state.getSharedData().right[i];
        } else {
            output[i*2    ] = 0;
            output[i*2 + 1] = 0;
        }
    }
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

