/*
 *
 * Links: https://techtutorialsx.com/2017/07/01/esp32-arduino-controlling-a-buzzer-with-pwm/
 *
 *
 */
#include "Arduino.h"
#include <ADSR.h>

#define BTN_PIN 13
#define SPK_PIN 19


ADSR synth(SPK_PIN,0);
bool triggered = false;

void setup(){
	Serial.begin(115200);
	Serial.println("ADSR test");

	synth.setup();

	synth.setAttack(10);	//set synth parameter
	synth.setDecay(300);
	synth.setSustain(3);
	synth.setRelease(300);


	pinMode(BTN_PIN, INPUT);

}

void loop(){


	synth.run();

	if(!digitalRead(BTN_PIN) && triggered == false){
		Serial.println("pressed");
		synth.trigger(440, 8);	//trigger frequency
//		synth.trigger(NOTE_C, 3, 10);	//trigger note
		triggered = true;
	}


//	Serial.println(digitalRead(BTN_PIN));

	if(digitalRead(BTN_PIN) && triggered == true){
		synth.release();

		triggered = false;
	}
}
