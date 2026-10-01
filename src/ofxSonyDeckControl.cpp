//
//  ofxSonyDeckControl.cpp
//
//  Packet format: CMD1 CMD2 [DATA...] CHECKSUM
//  The low nibble of CMD1 is the number of data bytes; the checksum is the
//  low byte of the sum of all preceding bytes.
//

#include "ofxSonyDeckControl.h"

#ifndef TARGET_WIN32
#include <termios.h>
#endif

namespace {
	int fromBCD(uint8_t b){ return ((b >> 4) & 0x0F) * 10 + (b & 0x0F); }
	uint8_t toBCD(int v){ return uint8_t(((v / 10) << 4) | (v % 10)); }
}

//--------------------------------------------------------------
bool ofxSonyDeckControl::Serial::setOddParity(){
#ifdef TARGET_WIN32
	DCB dcb = {};
	dcb.DCBlength = sizeof(DCB);
	if (!GetCommState(hComm, &dcb)) return false;
	dcb.Parity = ODDPARITY;
	dcb.fParity = TRUE;
	return SetCommState(hComm, &dcb);
#else
	struct termios options;
	if (tcgetattr(fd, &options) != 0) return false;
	options.c_cflag |= PARENB | PARODD;
	return tcsetattr(fd, TCSANOW, &options) == 0;
#endif
}

//--------------------------------------------------------------
ofxSonyDeckControl::~ofxSonyDeckControl(){
	close();
}

bool ofxSonyDeckControl::openPort(const std::function<bool()> & open){
	close();
	if (!open()) {
		ofLogError("ofxSonyDeckControl") << "Could not open serial port";
		return false;
	}
	if (!serial.setOddParity()) {
		ofLogWarning("ofxSonyDeckControl") << "Could not set odd parity; the deck may ignore commands";
	}
	serial.flush();
	return true;
}

bool ofxSonyDeckControl::setup(const std::string & portName){
	return openPort([&]{ return serial.setup(portName, 38400); });
}

bool ofxSonyDeckControl::setup(int deviceIndex){
	return openPort([&]{ return serial.setup(deviceIndex, 38400); });
}

void ofxSonyDeckControl::listDevices(){
	serial.listDevices();
}

void ofxSonyDeckControl::close(){
	if (serial.isInitialized()) serial.close();
	online = false;
	rx.clear();
	awaitingReply = false;
}

//--------------------------------------------------------------
void ofxSonyDeckControl::update(){
	if (!serial.isInitialized()) return;
	readBytes();

	uint64_t now = ofGetElapsedTimeMillis();

	// Drop a half-received packet if the rest never arrives.
	if (!rx.empty() && now - lastByteTime > 100) {
		if (verbose) ofLogNotice("ofxSonyDeckControl") << "Incomplete packet dropped";
		rx.clear();
	}
	// Give up waiting for a reply after 100 ms.
	if (awaitingReply && now - lastRequestTime > 100) {
		awaitingReply = false;
	}
	// Alternate status and timecode requests, one at a time.
	if (!awaitingReply && now - lastRequestTime > (uint64_t)pollInterval) {
		if (askTimeNext) {
			sendCommand(0x61, 0x0C, {0x01});   // current time sense: LTC
		} else {
			sendCommand(0x61, 0x20, {0x09});   // status sense: bytes 0-8
		}
		askTimeNext = !askTimeNext;
	}
	// Offline if no status has come back for a while.
	if (now - lastStatusTime > 1000) {
		online = false;
	}
}

void ofxSonyDeckControl::readBytes(){
	while (serial.available() > 0) {
		int b = serial.readByte();
		if (b < 0) break;   // OF_SERIAL_NO_DATA or OF_SERIAL_ERROR
		lastByteTime = ofGetElapsedTimeMillis();
		rx.push_back(uint8_t(b));
		if (rx.size() == 1) {
			expectedLength = (rx[0] & 0x0F) + 3;   // CMD1 + CMD2 + data + checksum
		}
		if (rx.size() == expectedLength) {
			uint8_t sum = 0;
			for (size_t i = 0; i + 1 < rx.size(); i++) sum += rx[i];
			if (sum == rx.back()) {
				parsePacket();
			} else if (verbose) {
				ofLogNotice("ofxSonyDeckControl") << "Bad checksum";
			}
			rx.clear();
			awaitingReply = false;
		}
	}
}

void ofxSonyDeckControl::parsePacket(){
	uint16_t cmd = (uint16_t(rx[0]) << 8) | rx[1];
	const uint8_t * d = rx.data() + 2;
	size_t n = rx.size() - 3;

	if ((cmd & 0xF0FF) == 0x7020 && n >= 9) {   // status data
		online = true;
		lastStatusTime = ofGetElapsedTimeMillis();
		cassetteOut = d[0] & 0x20;
		localOnly   = d[0] & 0x01;
		standby     = d[1] & 0x80;
		stopped     = d[1] & 0x20;
		rewinding   = d[1] & 0x08;
		forwarding  = d[1] & 0x04;
		recording   = d[1] & 0x02;
		playing     = d[1] & 0x01;
		jogMode     = d[2] & 0x10;
		backwards   = d[2] & 0x04;
		still       = d[2] & 0x02;
		nearEOT     = d[8] & 0x20;
		eot         = d[8] & 0x10;
	} else if (rx[0] == 0x74 && n >= 4) {          // time data: frames, seconds, minutes, hours in BCD
		timecode = ofToString(fromBCD(d[3] & 0x3F), 2, '0') + ":" + ofToString(fromBCD(d[2] & 0x7F), 2, '0') + ":"
		         + ofToString(fromBCD(d[1] & 0x7F), 2, '0') + ":" + ofToString(fromBCD(d[0] & 0x3F), 2, '0');
	} else if (cmd == 0x1001) {
		if (verbose) ofLogNotice("ofxSonyDeckControl") << "ACK";
	} else if (cmd == 0x1112) {
		ofLogWarning("ofxSonyDeckControl") << "NAK " << (n ? ofToHex(d[0]) : "");
	} else if (verbose) {
		ofLogNotice("ofxSonyDeckControl") << "Unhandled packet " << ofToHex(rx[0]) << " " << ofToHex(rx[1]);
	}
}

//--------------------------------------------------------------
void ofxSonyDeckControl::sendCommand(uint8_t cmd1, uint8_t cmd2, const std::vector<uint8_t> & data){
	if (!serial.isInitialized()) return;
	std::vector<uint8_t> packet;
	packet.push_back((cmd1 & 0xF0) | uint8_t(data.size() & 0x0F));
	packet.push_back(cmd2);
	packet.insert(packet.end(), data.begin(), data.end());
	uint8_t sum = 0;
	for (auto b : packet) sum += b;
	packet.push_back(sum);
	serial.writeBytes(packet.data(), packet.size());
	lastRequestTime = ofGetElapsedTimeMillis();
	awaitingReply = true;
}

// Transport
void ofxSonyDeckControl::stop()        { sendCommand(0x20, 0x00); }
void ofxSonyDeckControl::play()        { sendCommand(0x20, 0x01); }
void ofxSonyDeckControl::record()      { sendCommand(0x20, 0x02); }
void ofxSonyDeckControl::standbyOff()  { sendCommand(0x20, 0x04); }
void ofxSonyDeckControl::standbyOn()   { sendCommand(0x20, 0x05); }
void ofxSonyDeckControl::eject()       { sendCommand(0x20, 0x0F); }
void ofxSonyDeckControl::fastForward() { sendCommand(0x20, 0x10); }
void ofxSonyDeckControl::rewind()      { sendCommand(0x20, 0x20); }
void ofxSonyDeckControl::preroll()     { sendCommand(0x20, 0x30); }
void ofxSonyDeckControl::preview()     { sendCommand(0x20, 0x40); }
void ofxSonyDeckControl::review()      { sendCommand(0x20, 0x41); }
void ofxSonyDeckControl::localDisable(){ sendCommand(0x00, 0x0C); }
void ofxSonyDeckControl::localEnable() { sendCommand(0x00, 0x1D); }

void ofxSonyDeckControl::cueUpWithData(int hours, int minutes, int seconds, int frames){
	sendCommand(0x20, 0x31, {toBCD(frames), toBCD(seconds), toBCD(minutes), toBCD(hours)});
}

void ofxSonyDeckControl::jogForward(uint8_t speed)     { sendCommand(0x20, 0x11, {uint8_t(speed & 0x7F)}); }
void ofxSonyDeckControl::varForward(uint8_t speed)     { sendCommand(0x20, 0x12, {uint8_t(speed & 0x7F)}); }
void ofxSonyDeckControl::shuttleForward(uint8_t speed) { sendCommand(0x20, 0x13, {uint8_t(speed & 0x7F)}); }
void ofxSonyDeckControl::jogReverse(uint8_t speed)     { sendCommand(0x20, 0x21, {uint8_t(speed & 0x7F)}); }
void ofxSonyDeckControl::varReverse(uint8_t speed)     { sendCommand(0x20, 0x22, {uint8_t(speed & 0x7F)}); }
void ofxSonyDeckControl::shuttleReverse(uint8_t speed) { sendCommand(0x20, 0x23, {uint8_t(speed & 0x7F)}); }

// Editing
void ofxSonyDeckControl::inEntry()       { sendCommand(0x40, 0x10); }
void ofxSonyDeckControl::outEntry()      { sendCommand(0x40, 0x11); }
void ofxSonyDeckControl::inShiftPlus()   { sendCommand(0x40, 0x18); }
void ofxSonyDeckControl::inShiftMinus()  { sendCommand(0x40, 0x19); }
void ofxSonyDeckControl::outShiftPlus()  { sendCommand(0x40, 0x1A); }
void ofxSonyDeckControl::outShiftMinus() { sendCommand(0x40, 0x1B); }
