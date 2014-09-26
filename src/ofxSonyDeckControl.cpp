//
//  ofxSonyDeckControl.cpp
//  hyperDeckSerial
//
//  Created by Fred on 25/09/14.
//
//

#include "ofxSonyDeckControl.h"
#include "Binary.h"

void ofxSonyDeckControl::setup(int port){

     
     deck.setup(port, 38400);  		 // Used for communication with Hyper Deck Studio
     

}

/**
 */
void ofxSonyDeckControl::parsePacket(){
     unsigned int cmd = ((unsigned int)_RXbuffer[0]<<8) + (unsigned int)_RXbuffer[1];
     
     switch(cmd)	{
          case 0x7920:
               _isOnline = true;
               _lastStatusReceivedTime = ofGetElapsedTimeMillis();
               if (_RXbufferCheckSum != _lastStatusCheckSum)	{	// Only update if new information (we use the checksum to determine that, since it depends entirely on the data transmitted)
                    _lastStatusCheckSum = _RXbufferCheckSum;
                    if (_serialOutput)	cout<<"*** Updating states:"<<endl;
                    
                    // Playing:
                    if (_isPlaying != ((_RXbuffer[3] & B00000001)>0))	{
                         _isPlaying = ((_RXbuffer[3] & B00000001)>0);
                         if (_serialOutput)	cout<<"isPlaying: "<<endl;
                         if (_serialOutput)	cout<<ofToString(isPlaying());
                    }
                    
                    // Recording
                    if (_isRecording != ((_RXbuffer[3] & B00000010)>0))	{
                         _isRecording = ((_RXbuffer[3] & B00000010)>0);
                         if (_serialOutput)	cout<<"isRecording: "<<endl;
                         if (_serialOutput)	cout<<ofToString(isRecording());
                    }
                    
                    // Forwarding x2 or more:
                    if (_isForwarding != ((_RXbuffer[3] & B00000100)>0))	{
                         _isForwarding = ((_RXbuffer[3] & B00000100)>0);
                         if (_serialOutput)	cout<<"isForwarding: "<<endl;
                         if (_serialOutput)	cout<<ofToString(isForwarding());
                    }
                    
                    // Rewinding x1 or more:
                    if (_isRewinding != ((_RXbuffer[3] & B00001000)>0))	{
                         _isRewinding = ((_RXbuffer[3] & B00001000)>0);
                         if (_serialOutput)	cout<<"isRewinding: "<<endl;
                         if (_serialOutput)	cout<<ofToString(isRewinding());
                    }
                    
                    // Stopped:
                    if (_isStopped != ((_RXbuffer[3] & B00100000)>0))	{
                         _isStopped = ((_RXbuffer[3] & B00100000)>0);
                         if (_serialOutput)	cout<<"isStopped: "<<endl;
                         if (_serialOutput)	cout<<ofToString(isStopped());
                    }
                    
                    // Cassette Out:
                    if (_isCassetteOut != ((_RXbuffer[2] & B00100000)>0))	{
                         _isCassetteOut = ((_RXbuffer[2] & B00100000)>0);
                         if (_serialOutput)	cout<<"isCassetteOut: "<<endl;
                         if (_serialOutput)	cout<<ofToString(isCassetteOut());
                    }
                    
                    // Local mode (If REM is not enabled):
                    if (_isInLocalModeOnly != ((_RXbuffer[2] & B00000001)>0))	{
                         _isInLocalModeOnly = ((_RXbuffer[2] & B00000001)>0);
                         if (_serialOutput)	cout<<"isInLocalModeOnly: "<<endl;
                         if (_serialOutput)	cout<<ofToString(isInLocalModeOnly());
                    }
                    
                    // Stand By:
                    if (_isStandby != ((_RXbuffer[3] & B10000000)>0))	{
                         _isStandby = ((_RXbuffer[3] & B10000000)>0);
                         if (_serialOutput)	cout<<"isStandby: "<<endl;
                         if (_serialOutput)	cout<<ofToString(isStandby());
                    }
                    
                    // Jog Mode:
                    if (_isInJogMode != ((_RXbuffer[4] & B00010000)>0))	{
                         _isInJogMode = ((_RXbuffer[4] & B00010000)>0);
                         if (_serialOutput)	cout<<"isInJogMode: "<<endl;
                         if (_serialOutput)	cout<<ofToString(isInJogMode());
                    }
                    
                    // Direction Backwards:
                    if (_isDirectionBackwards != ((_RXbuffer[4] & B00000100)>0))	{
                         _isDirectionBackwards = ((_RXbuffer[4] & B00000100)>0);
                         if (_serialOutput)	cout<<"isDirectionBackwards: "<<endl;
                         if (_serialOutput)	cout<<ofToString(isDirectionBackwards());
                    }
                    
                    // Still:
                    if (_isStill != ((_RXbuffer[4] & B00000010)>0))	{
                         _isStill = ((_RXbuffer[4] & B00000010)>0);
                         if (_serialOutput)	cout<<"isStill: "<<endl;
                         if (_serialOutput)	cout<<ofToString(isStill());
                    }
                    
                    // Near EOT:
                    if (_isNearEOT != ((_RXbuffer[10] & B00100000)>0))	{
                         _isNearEOT = ((_RXbuffer[10] & B00100000)>0);
                         if (_serialOutput)	cout<<"isNearEOT: "<<endl;
                         if (_serialOutput)	cout<<ofToString(isNearEOT());
                    }
                    
                    // EOT:
                    if (_isEOT != ((_RXbuffer[10] & B00010000)>0))	{
                         _isEOT = ((_RXbuffer[10] & B00010000)>0);
                         if (_serialOutput)	cout<<"isEOT: "<<endl;
                         if (_serialOutput)	cout<<ofToString(isEOT());
                    }
               }
               break;
          case 0x1001:
               if (_serialOutput)	cout<<"ACK"<<endl;
               break;
          case 0x1112:
               if (_serialOutput)	cout<<"NACK"<<endl;
               if (_serialOutput)	cout<<ofToString(_RXbuffer[2]);
               break;
          default:
               if (_serialOutput)	{
                    if (_serialOutput)	cout<<"Unsupported Data Packet:"<<endl;
                    for(uint8_t i=0; i<_RXbufferExpectedLength; i++)	{
                         cout<<ofToString(_RXbuffer[i]);
                    }
                    if (_serialOutput)	cout<<"==="<<endl;
               }
               break;
     }
}
void ofxSonyDeckControl::update() {
     
    
          _RXbuffer[_RXbufferPointer]=deck.readByte();
          _lastDataRXTime = ofGetElapsedTimeMillis();
          _awaitingResponse = false;
          
          if (_RXbufferPointer==0)	{	// start of session... init it
               _RXbufferCheckSum = 0;	// Reset checksum register
               _RXbufferExpectedLength = (_RXbuffer[0] & B00001111)+2;	// Get length of the packet
          }
          // If the pointer is equal to the expected length it means we have just read the checksum byte, so lets evaluate that:
          if (_RXbufferPointer == _RXbufferExpectedLength)	{
               if (_RXbufferCheckSum==_RXbuffer[_RXbufferPointer])	{
                    parsePacket();
               } else {
                    if (_serialOutput)	cout<<"Bad checksum"<<endl;
                    //delay(5);	// Let more data arrive, so we can flush it.
                    deck.readByte();
               
               _RXbufferPointer=-1;	// Will be increased to zero in a moment.
          }
          }
          else{
               // If we are not at the end (checksum byte), we continously calculate the checksum:
               _RXbufferCheckSum+=_RXbuffer[_RXbufferPointer];
          
          _RXbufferPointer++;
          
          // Finally: Make sure we don't create a buffer overrun in memory at any rate:
          if (_RXbufferPointer>=15)	_RXbufferPointer=0;
     }
     
     
     // Make sure we don't stall in data reception for any reason:
     if ((_RXbufferPointer > 0 || _awaitingResponse) && ((unsigned long)_lastDataRXTime+1000 < (unsigned long)ofGetElapsedTimeMillis()))	{
          // Reset:
          _RXbufferPointer=0;
          _awaitingResponse = false;
          if (_serialOutput)	cout<<"Had to reset RX situation."<<endl;
     }
     
     // Send a status request to the HyperDeck once every XXX ofGetElapsedTimeMilliseonds:
     if (TXready() && ((unsigned long)_lastStatusRequestTime+_statusRequestPeriodLength < (unsigned long)ofGetElapsedTimeMillis()))  {
          deck.writeByte(0x61);  // CMD1
          deck.writeByte(0x20);  // CMD2
          deck.writeByte(0x09);
          deck.writeByte(0x61+0x20+0x09);  // Checksum byte
          _lastStatusRequestTime = ofGetElapsedTimeMillis();
          _awaitingResponse = true;
     }
     
     // Go offline if a status has not been received recently:
     if ((unsigned long)_lastStatusReceivedTime+_statusRequestPeriodLength*3 < (unsigned long)ofGetElapsedTimeMillis())	{
          _isOnline = false;
     }
    
}

void ofxSonyDeckControl::localDisable(){
     if (TXready())  {
          deck.writeByte(0x20);  // CMD1
          deck.writeByte(0x01);  // CMD2
          deck.writeByte(0x20+0x01);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::standbyOff(){
     if (TXready())  {
          deck.writeByte(0x20);  // CMD1
          deck.writeByte(0x01);  // CMD2
          deck.writeByte(0x20+0x01);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::standbyOn(){
     if (TXready())  {
          deck.writeByte(0x20);  // CMD1
          deck.writeByte(0x01);  // CMD2
          deck.writeByte(0x20+0x01);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::eject(){
     if (TXready())  {
          deck.writeByte(0x20);  // CMD1
          deck.writeByte(0x01);  // CMD2
          deck.writeByte(0x20+0x01);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::jogForward(){
     if (TXready())  {
          deck.writeByte(0x21);  // CMD1
          deck.writeByte(0x11);  // CMD2
          deck.writeByte(0x11+0x21);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::varForward(){
     if (TXready())  {
          deck.writeByte(0x21);  // CMD1
          deck.writeByte(0x12);  // CMD2
          deck.writeByte(0x21+0x12);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::shuttleForward(){
     if (TXready())  {
          deck.writeByte(0x21);  // CMD1
          deck.writeByte(0x13);  // CMD2
          deck.writeByte(0x21+0x13);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::jogRev(){
     if (TXready())  {
          deck.writeByte(0x21);  // CMD1
          deck.writeByte(0x21);  // CMD2
          deck.writeByte(0x21+0x21);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::varRev(){
     if (TXready())  {
          deck.writeByte(0x21);  // CMD1
          deck.writeByte(0x22);  // CMD2
          deck.writeByte(0x21+0x22);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::shuttleRev(){
     if (TXready())  {
          deck.writeByte(0x21);  // CMD1
          deck.writeByte(0x23);  // CMD2
          deck.writeByte(0x21+0x23);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::preview(){
     if (TXready())  {
          deck.writeByte(0x20);  // CMD1
          deck.writeByte(0x40);  // CMD2
          deck.writeByte(0x20+0x40);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::review(){
     if (TXready())  {
          deck.writeByte(0x20);  // CMD1
          deck.writeByte(0x01);  // CMD2
          deck.writeByte(0x20+0x01);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::outpointPreview(){
     if (TXready())  {
          deck.writeByte(0x20);  // CMD1
          deck.writeByte(0x41);  // CMD2
          deck.writeByte(0x20+0x41);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::inEntry(){
     if (TXready())  {
          deck.writeByte(0x40);  // CMD1
          deck.writeByte(0x10);  // CMD2
          deck.writeByte(0x40+0x10);  // Checksum byte
          _awaitingResponse = true;
     }
}

void ofxSonyDeckControl::outEntry(){
     if (TXready())  {
          deck.writeByte(0x40);  // CMD1
          deck.writeByte(0x11);  // CMD2
          deck.writeByte(0x40+0x11);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::inShiftPlus(){
     if (TXready())  {
          deck.writeByte(0x40);  // CMD1
          deck.writeByte(0x18);  // CMD2
          deck.writeByte(0x40+0x18);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::inShiftMinus(){
     if (TXready())  {
          deck.writeByte(0x40);  // CMD1
          deck.writeByte(0x19);  // CMD2
          deck.writeByte(0x40+0x19);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::outShiftPLus(){
     if (TXready())  {
          deck.writeByte(0x40);  // CMD1
          deck.writeByte(0x1A);  // CMD2
          deck.writeByte(0x40+0x1A);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::outShiftMinus(){
     if (TXready())  {
          deck.writeByte(0x40);  // CMD1
          deck.writeByte(0x1B);  // CMD2
          deck.writeByte(0x40+0x1B);  // Checksum byte
          _awaitingResponse = true;
     }
}
void ofxSonyDeckControl::tcGenSense(){
     if (TXready())  {
          deck.writeByte(0x61);  // CMD1
          deck.writeByte(0x0A);  // CMD2
          deck.writeByte(0x61+0x0A);  // Checksum byte
          _awaitingResponse = true;
     }
}

bool ofxSonyDeckControl::TXready(){
     return true;
}

bool ofxSonyDeckControl::isOnline() {
     return _isOnline;
}



/**
 * 
 */
bool ofxSonyDeckControl::isPlaying() {
     return _isPlaying;
}

/**
 * 
 */
bool ofxSonyDeckControl::isRecording() {
     return _isRecording;
}

/**
 * 
 */
bool ofxSonyDeckControl::isForwarding() {
     return _isForwarding;
}

/**
 * 
 */
bool ofxSonyDeckControl::isRewinding() {
     return _isRewinding;
}

/**
 * 
 */
bool ofxSonyDeckControl::isStopped() {
     return _isStopped;
}

// Other status registers:

/**
 * 
 */
bool ofxSonyDeckControl::isCassetteOut() {
     return _isCassetteOut;
}

/**
 * 
 */
bool ofxSonyDeckControl::isInLocalModeOnly() {
     return _isInLocalModeOnly;
}

/**
 * 
 */
bool ofxSonyDeckControl::isStandby() {
     return _isStandby;
}

/**
 * 
 */
bool ofxSonyDeckControl::isInJogMode() {
     return _isInJogMode;
}

/**
 * 
 */
bool ofxSonyDeckControl::isDirectionBackwards() {
     return _isDirectionBackwards;
}

/**
 * 
 */
bool ofxSonyDeckControl::isStill() {
     return _isStill;
}

/**
 * 
 */
bool ofxSonyDeckControl::isNearEOT() {
     return _isNearEOT;
}

/**
 * 
 */
bool ofxSonyDeckControl::isEOT() {
     return _isEOT;
}









/**
 * 
 */
void ofxSonyDeckControl::doPlay() {
     if (TXready())  {
          deck.writeByte(0x20);  // CMD1
          deck.writeByte(0x01);  // CMD2
          deck.writeByte(0x20+0x01);  // Checksum byte
          _awaitingResponse = true;
     } 
}

/**
 * 
 */
void ofxSonyDeckControl::doRecord() {
     if (TXready())  {
          deck.writeByte(0x20);  // CMD1
          deck.writeByte(0x02);  // CMD2
          deck.writeByte(0x20+0x02);  // Checksum byte
          _awaitingResponse = true;
     } 
}

/**
 * 
 */
void ofxSonyDeckControl::doFastForward() {
     if (TXready())  {
          deck.writeByte(0x20);  // CMD1
          deck.writeByte(0x10);  // CMD2
          deck.writeByte(0x20+0x10);  // Checksum byte
          _awaitingResponse = true;
     } 
}

/**
 * 
 */
void ofxSonyDeckControl::doRewind() {
     if (TXready())  {
          deck.writeByte(0x20);  // CMD1
          deck.writeByte(0x20);  // CMD2
          deck.writeByte(0x20+0x20);  // Checksum byte
          _awaitingResponse = true;
     } 
}

/**
 * 
 */
void ofxSonyDeckControl::doStop() {
     if (TXready())  {
          deck.writeByte(0x20);  // CMD1
          deck.writeByte((uint8_t) 0x00);  // CMD2
          deck.writeByte(0x20+0x00);  // Checksum byte
          _awaitingResponse = true;
     } 
}