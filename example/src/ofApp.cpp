#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60);
	ofBackground(20);

	ofSerial serial;
	ports = serial.getDeviceList();
	// Pick the first USB serial adapter; press 0-9 to choose another port.
	for (size_t i = 0; i < ports.size(); i++) {
		if (ofIsStringInString(ofToLower(ports[i].getDeviceName()), "usb")) {
			portIndex = i;
			break;
		}
	}
	if (portIndex >= 0) deck.setup(ports[portIndex].getDevicePath());
}

//--------------------------------------------------------------
void ofApp::update(){
	deck.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	std::stringstream s;
	s << "Sony 9-pin deck control\n\n";
	s << "Port: " << (portIndex >= 0 ? ports[portIndex].getDevicePath() : "none") << "   "
	  << (deck.isOnline() ? "deck online" : "deck not responding") << "\n";
	s << "Timecode: " << deck.getTimecode() << "\n\n";
	s << "playing " << deck.isPlaying() << "  recording " << deck.isRecording() << "  stopped " << deck.isStopped()
	  << "  ff " << deck.isForwarding() << "  rew " << deck.isRewinding() << "\n";
	s << "still " << deck.isStill() << "  jog " << deck.isInJogMode() << "  reverse " << deck.isDirectionBackwards()
	  << "  standby " << deck.isStandby() << "  local only " << deck.isInLocalModeOnly() << "\n";
	s << "cassette out " << deck.isCassetteOut() << "  near end " << deck.isNearEOT() << "  end " << deck.isEOT() << "\n\n";
	s << "Keys: space play  s stop  r record  f fast forward  w rewind\n"
	  << "      left/right jog  , . shuttle reverse/forward  l local disable (remote on)\n\n";
	s << "Serial ports (press the number to use one):\n";
	for (size_t i = 0; i < ports.size() && i < 10; i++) {
		s << "  " << i << "  " << ports[i].getDevicePath() << (int(i) == portIndex ? "  <" : "") << "\n";
	}
	ofDrawBitmapString(s.str(), 20, 30);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == ' ') deck.play();
	if (key == 's') deck.stop();
	if (key == 'r') deck.record();
	if (key == 'f') deck.fastForward();
	if (key == 'w') deck.rewind();
	if (key == OF_KEY_RIGHT) deck.jogForward();
	if (key == OF_KEY_LEFT) deck.jogReverse();
	if (key == '.') deck.shuttleForward(64);
	if (key == ',') deck.shuttleReverse(64);
	if (key == 'l') deck.localDisable();
	if (key >= '0' && key <= '9' && key - '0' < (int)ports.size()) {
		portIndex = key - '0';
		deck.setup(ports[portIndex].getDevicePath());
	}
}
