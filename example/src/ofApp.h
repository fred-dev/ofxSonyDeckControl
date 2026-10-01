#pragma once

#include "ofMain.h"
#include "ofxSonyDeckControl.h"

class ofApp : public ofBaseApp {
public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyPressed(int key) override;

	ofxSonyDeckControl deck;
	std::vector<ofSerialDeviceInfo> ports;
	int portIndex = -1;
};
