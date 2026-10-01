# ofxSonyDeckControl

openFrameworks addon for controlling decks that speak the Sony 9-pin RS-422 protocol (also called P2), such as Blackmagic HyperDecks and broadcast VTRs, through a USB to RS-422 adapter. It sends transport and edit commands and polls the deck for status flags and LTC timecode.

```cpp
ofxSonyDeckControl deck;
deck.setup("/dev/tty.usbserial-A1234");   // or a device index
deck.update();                            // every frame
deck.play();
if (deck.isPlaying()) ofLog() << deck.getTimecode();
```

The port is opened at 38400 baud with odd parity, as the protocol requires. `ofSerial` can't set parity on its own, so the addon does it through a small subclass (termios on macOS/Linux, DCB on Windows).

Based on Kasper Skaarhoj's Arduino library for HyperDeck serial control.

## Example

`example/` lists the serial ports, connects to the first USB adapter (press 0-9 to pick another), shows the deck status and timecode, and maps the keyboard to transport commands. Generate it with projectGenerator.

## Requirements

- openFrameworks 0.12 or later, no other addons
- A USB to RS-422 adapter wired for Sony 9-pin, and remote control enabled on the deck

Written in 2014, rewritten in 2026 for current openFrameworks with corrected command codes.
