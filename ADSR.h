#ifndef ADSR_H
#define ADSR_H
#include <Arduino.h>

struct tone_struct{
	double freq = 0;
	uint16_t loudness = 0;	//current loudness
	uint16_t loudnessMax = 10000;	// 0 ... 65535
	uint8_t adsrPhase = 0;	//0=none, 1=attack, 2=decay, 3=sustain, 4= release
	unsigned long attackTimestamp = 0;	//timestamp when attack starts
	unsigned long decayTimestamp = 0;	//timestamp when decay starts
	unsigned long sustainTimestamp = 0;	//timestamp	when sustain starts
	unsigned long releaseTimestamp = 0;	//timestamp	when release starts

};

struct wavetable_struct{
	byte sinwave[112] = {
		0x80, 0x83, 0x87, 0x8A, 0x8E, 0x91, 0x95, 0x98, 0x9B, 0x9E, 0xA2, 0xA5, 0xA7, 0xAA, 0xAD, 0xAF,
		0xB2, 0xB4, 0xB6, 0xB8, 0xB9, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xBF, 0xBF, 0xC0, 0xBF, 0xBF, 0xBF,
		0xBE, 0xBD, 0xBC, 0xBB, 0xB9, 0xB8, 0xB6, 0xB4, 0xB2, 0xAF, 0xAD, 0xAA, 0xA7, 0xA5, 0xA2, 0x9E,
		0x9B, 0x98, 0x95, 0x91, 0x8E, 0x8A, 0x87, 0x83, 0x80, 0x7C, 0x78, 0x75, 0x71, 0x6E, 0x6A, 0x67,
		0x64, 0x61, 0x5D, 0x5A, 0x58, 0x55, 0x52, 0x50, 0x4D, 0x4B, 0x49, 0x47, 0x46, 0x44, 0x43, 0x42,
		0x41, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x41, 0x42, 0x43, 0x44, 0x46, 0x47, 0x49, 0x4B,
		0x4D, 0x50, 0x52, 0x55, 0x58, 0x5A, 0x5D, 0x61, 0x64, 0x67, 0x6A, 0x6E, 0x71, 0x75, 0x78, 0x7C
	};
};

struct synth_struct{
	uint16_t attack = 0;	// in ms
	uint16_t decay = 0;	// in ms
	uint16_t sustain = 0;	// loudness
	uint16_t release = 0;	// in ms
	bool retrigger = false;
};

class ADSR{
public:
	ADSR(uint8_t pin, uint8_t pwmchannel);
	void setup();
//	void setup(uint8_t pin);
	void run();

	void trigger(double freq, uint16_t loudness);	//trigger tone
	void trigger(note_t note, uint8_t octave, uint16_t loudness);
	void release();	//release tone

	void generateSound(uint8_t type, double freq );


	//set Synth parameter
	void setAttack(uint16_t value_ms);
	void setDecay(uint16_t value_ms);
	void setSustain(double value_loudness);
	void setRelease(uint16_t value_ms);
	void setADSR(uint16_t attack, uint16_t decay, uint8_t sustain, uint16_t release);

private:
	void play_stop();
	void play_attack();
	void play_decay();
	void play_sustain();
	void play_release();

	void generateSquare(uint16_t freq);






	// output parameter
	uint8_t speaker_pin  = 19;
	uint8_t pwm_channel = 0;
	uint8_t pwm_resolution = 8;	//bit
	// structs
	tone_struct tone;	//tone object
	synth_struct synth;
	wavetable_struct wavetable;

	bool releaseWhenReached = false;
};


#endif
