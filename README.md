 
# Project

Build an RFID control system that can register and unregister cards individually using either the RFID reader, or the IR receiver.

# Pin Details and Project Requirements

- Pin 2 (int0) => DATA0 of RFID Reader (Wiegand 34 bits protocol)
- Pin 3 (int1) => DATA1 of RFID Reader (Wiegand 34 bits protocol)
- Requires ATMEGA328 or ATMEGA328p Arduino to work

# RFID Reader Operation
- Pass Master Tag 1 time and wait to access door
- Pass it 2 times to register the next non-master card
- Pass it 3 times to unregister the next non-master card

# IR Reader Operation
- Buttons 0-9 are numerical buttons and are parsed to unsigned long once confirmed. 
- Buttons Two-Arrow-Left-Right and U/SD are OK and Cancel buttons respectively. So they confirm or cancel the operation/code.

# Change RFID Master Code

 1. In the normal operation mode (do not pass master rfid card in the rfid reader not even once), using the IR, press MODE btn.
 2. Enter the actual MASTER RFID TAG and press OK.
 3. If incorrect, buzzer will fire. If the code was correct, then type in the new MASTER RFID and press OK.
 4. If operation succeeded, LED will flash.

# More Details
See the [manual/main.pdf](blob/master/manual/main.pdf) for more details about the operation of the system.