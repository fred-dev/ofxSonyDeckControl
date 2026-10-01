//
//  ofxSonyDeckControl.h
//
//  Control decks that speak the Sony 9-pin RS-422 protocol (also called P2),
//  such as Blackmagic HyperDecks and broadcast VTRs, through a USB to RS-422
//  adapter. Sends transport commands and polls status and timecode.
//
//  Based on Kasper Skaarhoj's Arduino library for HyperDeck serial control.
//

#pragma once

#include "ofMain.h"

class ofxSonyDeckControl {
public:
	~ofxSonyDeckControl();

	/// Opens the port at 38400 baud, 8 data bits, odd parity, 1 stop bit.
	bool setup(const std::string & portName);   // e.g. "/dev/tty.usbserial-A1234" or "COM3"
	bool setup(int deviceIndex);                // index from listDevices()
	void listDevices();
	/// Call every frame: reads replies and polls status and timecode.
	void update();
	void close();

	/// True while the deck answers status requests.
	bool isOnline() const { return online; }

	// Transport
	void play();
	void stop();
	void record();
	void fastForward();
	void rewind();
	void eject();
	void standbyOn();
	void standbyOff();
	void localDisable();   // allow remote control
	void localEnable();
	void preroll();
	void preview();
	void review();
	void cueUpWithData(int hours, int minutes, int seconds, int frames);

	// Speed is 0-127 (Sony scale: 32 is about normal play, 64 about 10x).
	void jogForward(uint8_t speed = 32);
	void jogReverse(uint8_t speed = 32);
	void varForward(uint8_t speed = 32);
	void varReverse(uint8_t speed = 32);
	void shuttleForward(uint8_t speed = 32);
	void shuttleReverse(uint8_t speed = 32);

	// Editing
	void inEntry();
	void outEntry();
	void inShiftPlus();
	void inShiftMinus();
	void outShiftPlus();
	void outShiftMinus();

	// Status, from the most recent status reply
	bool isPlaying() const { return playing; }
	bool isRecording() const { return recording; }
	bool isForwarding() const { return forwarding; }      // fast forward x2 or more
	bool isRewinding() const { return rewinding; }
	bool isStopped() const { return stopped; }
	bool isCassetteOut() const { return cassetteOut; }
	bool isInLocalModeOnly() const { return localOnly; }  // remote control disabled on the deck
	bool isStandby() const { return standby; }
	bool isInJogMode() const { return jogMode; }
	bool isDirectionBackwards() const { return backwards; }
	bool isStill() const { return still; }
	bool isNearEOT() const { return nearEOT; }           // about 3 minutes left
	bool isEOT() const { return eot; }                   // about 30 seconds left

	/// Latest LTC timecode from the deck, "hh:mm:ss:ff".
	std::string getTimecode() const { return timecode; }

	void setPollInterval(int millis) { pollInterval = millis; }
	void setVerbose(bool v) { verbose = v; }

private:
	// ofSerial has no parity setting; this subclass adds odd parity.
	class Serial : public ofSerial {
	public:
		bool setOddParity();
	};

	bool openPort(const std::function<bool()> & open);
	void sendCommand(uint8_t cmd1, uint8_t cmd2, const std::vector<uint8_t> & data = {});
	void readBytes();
	void parsePacket();

	Serial serial;
	std::vector<uint8_t> rx;
	size_t expectedLength = 0;
	uint64_t lastByteTime = 0;
	uint64_t lastRequestTime = 0;
	uint64_t lastStatusTime = 0;
	bool awaitingReply = false;
	bool askTimeNext = false;
	int pollInterval = 50;
	bool verbose = false;

	bool online = false;
	bool playing = false;
	bool recording = false;
	bool forwarding = false;
	bool rewinding = false;
	bool stopped = false;
	bool cassetteOut = false;
	bool localOnly = false;
	bool standby = false;
	bool jogMode = false;
	bool backwards = false;
	bool still = false;
	bool nearEOT = false;
	bool eot = false;
	std::string timecode = "--:--:--:--";
};
