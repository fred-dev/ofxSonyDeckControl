//
//  ofxSonyDeckControl.h
//  hyperDeckSerial
//
//  Created by Fred on 25/09/14.
//
//


#include <stdio.h>
#include "ofMain.h"

class ofxSonyDeckControl
{
public:
     
     bool isOnline();
     bool TXready();
     
     
     /********************************
      * HyperDeckStudio State methods
      * Returns the most recent information we've
      * got about the device state
      ********************************/
     
     // Deck status:
     bool isPlaying();		// If playing
     bool isRecording();		// If recording
     bool isForwarding();		// If fast forwarding x2 or more
     bool isRewinding();		// If rewinding x1 or more
     bool isStopped();		// If stopped
     
     // Other status registers:
     bool isCassetteOut();
     bool isInLocalModeOnly();
     bool isStandby();		// If
     bool isInJogMode();
     bool isDirectionBackwards();
     bool isStill();
     bool isNearEOT();	// 3 minutes before
     bool isEOT();	// 30 seconds before
     
     
     /********************************
      * HyperDeckStudio Command methods
      * Asks the deck to do something
      ********************************/
     
     void setup(int port);
     void update();
     
     void doPlay();
     void doRecord();
     void doFastForward();
     void doRewind();
     void doStop();
     void localDisable();
     void standbyOff();
     void standbyOn();
     void eject();
     void jogForward();
     void varForward();
     void shuttleForward();
     void jogRev();
     void varRev();
     void shuttleRev();
     void preview();
     void review();
     void outpointPreview();
     void inEntry();
     void outEntry();
     void inShiftPlus();
     void inShiftMinus();
     void outShiftPLus();
     void outShiftMinus();
     void tcGenSense();
     
     
     
     
     
     ofSerial deck;
     
private:
     void parsePacket();
     bool _isOnline;
     bool _serialOutput;
     bool _isPlaying;
     bool _isRecording;
     bool _isForwarding;
     bool _isRewinding;
     bool _isStopped;
     
     
     bool _isCassetteOut;
     bool _isInLocalModeOnly;
     bool _isStandby;
     bool _isInJogMode;
     bool _isDirectionBackwards;
     bool _isStill;
     bool _isNearEOT;
     bool _isEOT;
     
     uint8_t _RXbuffer[15];
     uint8_t _RXbufferPointer;
     uint8_t _RXbufferCheckSum;
     uint8_t _RXbufferExpectedLength;
     uint8_t _lastStatusCheckSum;
     
     unsigned long _lastStatusRequestTime;
     unsigned long _lastDataRXTime;
     unsigned long _statusRequestPeriodLength;
     unsigned long _lastStatusReceivedTime;
     bool _awaitingResponse;

};


