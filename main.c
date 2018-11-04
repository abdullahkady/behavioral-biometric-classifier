#include <REG52.H>
#include <stdio.h>
#include <string.h>




char pass[11] = {'.', 't', 'i', 'e', '5', 'R', 'o', 'n', 'a', 'l', '\0'};

/*
	Up to 255 decimal count of "overflows", ranging from 0 to ~15 seconds.
*/
unsigned int timingsA[9];
unsigned int timingsB[9];
unsigned char timer = 0; // In overflows :D (~0.06sec)

void init() {
	/*------------------------------------------------
	Setup the serial port for 2400 baud at 12MHz.
	------------------------------------------------*/
	SCON = 0x50; // SCON: mode 1, 8-bit UART, enable reciever
	TMOD = 0x21; // TMOD: timer 1, mode 2, 8-bit reload <> timer 0, mode 1
	TH1 = 0xF3; // TH1: reload value for 2400 baud @ 12MHz
	TR1 = 1; // TR1: timer 1 run
	TI = 1; // TI: set TI to send first char of UART
}

// Up to ~15 seconds
void handleOverflow(void) interrupt 1 {
	timer ++;
}

// unsigned int readTimer () {
// 	return (TH0 << 8) | TL0 ;
// }

void stopTimer() {
	TR0 = 0;
}

void startTimer() {
	TR0 = 1;
}

void resetTimer() {
	TL0 = 0x00;
	TH0 = 0x00;
	timer = 0;
}

void trainUser(unsigned int timings[]) {
	unsigned char count = 0, charIndex = 0;
	unsigned char localTimings[9];
	unsigned char i;

	resetTimer();
	startTimer();

	while (count < 5) {
		char c = _getkey();
		if (c == pass[charIndex]) {
			stopTimer();
			if (charIndex != 0)
				localTimings[charIndex - 1] = timer;
			charIndex++;
		} else {
			charIndex = 0;
		}

		resetTimer();
		startTimer();

		// One run of correct input password
		if (charIndex == 9) {
			charIndex = 0;
			for (i = 0; i < 9; i++ ) {
				timings[i] += localTimings[i];
			}
			count++;
		}
	}

	// Done 5 successful inputs, just average them.
	for (i = 0; i < 9; i++ ) {
		timings[i] = (timings[i] / 5);
	}

}

void main (void) {

	init();
	trainUser(timingsA);

}
