#pragma once

#include "ofMain.h"

#include "ofxUI.h"
#include "ofxStateMachine.h"
#include "ofxOsc.h"
#include "ofxWaveHandler.h"
#include "SharedData.h"
#include "screens.h"
#include "randpool.h"
#include "entropy.h"


class ofApp : public ofBaseApp{

	public:

		void setup();
        void exit();

		void update();
		void draw();

        void oscLoop();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        void audioIn(float * input, int bufferSize, int nChannels);
		//void audioOut(float * input, int bufferSize, int nChannels);

        ofSoundStream soundStream;
		ofxOscReceiver oscr;
        ofxWaveHandler* waveObject;

    private:
        itg::ofxStateMachine<SharedData> state;
        //vector<unsigned char> poolfeed;
        bool trigger;
        unsigned char *poolfeed;
        Randpool pool;
};


namespace randpool {
    int getEntropyPoolAvailable();
    int getEntropyPoolSize();
};
