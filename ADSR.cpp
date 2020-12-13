/*
 * ADSR.cpp
 *
 *  Created on: 12 Dec 2020
 *      Author: Chris
 */
#include <ADSR.h>

/***** public *****/

ADSR::ADSR(uint8_t pin, uint8_t pwmchannel){
	speaker_pin = pin;
	pwm_channel = pwmchannel;
}
void ADSR::setup(){
	ledcSetup(pwm_channel, 0, pwm_resolution);	//access PWM
	ledcAttachPin(speaker_pin, pwm_channel);	//access pin to PWM
}
void ADSR::run(){
	switch(tone.adsrPhase){

	case 0:	play_stop();	//stop
	break;
	case 1: play_attack();	//attack
	break;
	case 2: play_decay();	//decay
	break;
	case 3: play_sustain();	//sustain
	break;
	case 4: play_release();
	break;
	default: play_stop();	//stop
	break;
	}

}


void ADSR::trigger(double freq, uint16_t loudness){

	if(tone.adsrPhase == 0 || synth.retrigger == true){
		tone.freq = freq;
		tone.loudnessMax = loudness;
		tone.attackTimestamp = millis();	//(re)trigger
		tone.decayTimestamp = tone.attackTimestamp + synth.attack;
		tone.sustainTimestamp = tone.decayTimestamp + synth.decay;
		tone.adsrPhase = 1;	//set phase to attack (retrigger mode)
		ledcWriteTone(pwm_channel, tone.freq);

		Serial.println("trigger");
	}

}

void ADSR::trigger(note_t note, uint8_t octave, uint16_t loudness){
	if(tone.adsrPhase == 0 || synth.retrigger == true){
		tone.loudnessMax = loudness;
		tone.attackTimestamp = millis();	//(re)trigger
		tone.decayTimestamp = tone.attackTimestamp + synth.attack;
		tone.sustainTimestamp = tone.decayTimestamp + synth.decay;
		tone.adsrPhase = 1;	//set phase to attack (retrigger mode)

// note to frequency
	    const uint16_t noteFrequencyBase[12] = {
	    //   C        C#       D        Eb       E        F       F#        G       G#        A       Bb        B
	        4186,    4435,    4699,    4978,    5274,    5588,    5920,    6272,    6645,    7040,    7459,    7902
	    };

	    if(octave > 8 || note >= NOTE_MAX){
	        tone.freq = 0;
	    }
	    tone.freq =  (double)noteFrequencyBase[note] / (double)(1 << (8-octave));

	    ledcWriteTone(pwm_channel, tone.freq);


		Serial.println("trigger");
	}
}

void ADSR::release(){

	if(tone.adsrPhase > 0 || tone.adsrPhase < 3){
		releaseWhenReached = true;
	}

	if(tone.adsrPhase == 3){
		tone.adsrPhase = 4;
		tone.releaseTimestamp = millis();
		releaseWhenReached = false;	//reset

		Serial.println("release");
	}
}


void ADSR::setAttack(uint16_t value_ms){
	synth.attack = value_ms;
}

void ADSR::setDecay(uint16_t value_ms){
	synth.decay = value_ms;
}

void ADSR::setSustain(double value_loudness){
	synth.sustain = value_loudness;
}

void ADSR::setRelease(uint16_t value_ms){
	synth.release = value_ms;
}

/****** private *****/


void ADSR::play_stop(){
	ledcWrite(pwm_channel, 0);
}
void ADSR::play_attack(){

	if(millis() >= tone.attackTimestamp + synth.attack){	//next phase when reached
		tone.adsrPhase++;
	}else{
		tone.loudness = map(millis(),tone.attackTimestamp,tone.decayTimestamp,0,tone.loudnessMax);
	    ledcWrite(pwm_channel, tone.loudness);
//	    Serial.println(tone.loudness);
	}

}


void ADSR::play_decay(){
	if(millis() >= tone.decayTimestamp + synth.decay){	//next phase when reached
		tone.adsrPhase++;
	}else{
		tone.loudness = map(millis(),tone.decayTimestamp,tone.sustainTimestamp,tone.loudnessMax, synth.sustain);
	    ledcWrite(pwm_channel, tone.loudness);
//	    Serial.println(tone.loudness);
	}
}

void ADSR::play_sustain(){
	if(releaseWhenReached == true){	//skip sustain when already released
//		tone.adsrPhase++;
//		releaseWhenReached = false;	//reset

		release();
	}else{
		tone.loudness = synth.sustain;
		ledcWrite(pwm_channel, tone.loudness);
	//	Serial.println(tone.loudness);
	}


}

void ADSR::play_release(){
	if(millis() >= tone.releaseTimestamp + synth.release){	//next phase when reached
		tone.adsrPhase = 0;
	}else{

		if(synth.release > 0){
			tone.loudness = map(millis(),tone.releaseTimestamp,(tone.releaseTimestamp + synth.release),synth.sustain, 0);
//			Serial.println(tone.loudness);
		}else{
			tone.loudness = 0;
			tone.adsrPhase = 0;
		}

	    ledcWrite(pwm_channel, tone.loudness);
//	    Serial.println(tone.loudness);
	}
}
