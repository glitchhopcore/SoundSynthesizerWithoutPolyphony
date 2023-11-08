// SoundSynthesizer.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Remember to add winmm.lib in the input dependencies from the project menu, also set the debug from x64 to x86 to avoid exception thrown error

#include <iostream>
using namespace std;
#include "olcNoiseMaker.h"

// Global Variables
atomic<double> dFrequencyOutput = 0.0; 
const double dOctaveBaseFrequency = 110.0; // Base Frequency of the A2 Note
const double d12thRootOf2 = pow(2.0, 1.0 / 12.0); // Since 12 Notes per octave and the frequency doubles
sEnvelopeADSR envelope;

//Function for converting Hertz to Angular Frequency for math functions 
double w(double dHertz) {
	return dHertz * 2.0 * PI;
}

double osc(double dHertz, double dTime, int nType) {
	switch(nType) {
	case 0: // Sine Wave
		return sin(w(dHertz) * dTime);
	case 1: // Square Wave
		return sin(w(dHertz) * dTime) > 0.0 ? 1.0 : -1.0;
	case 2: // Triangle Wave
		return asin(sin(w(dHertz) * dTime)) * (2.0/PI);
	case 3: // Saw Wave 
		return (2.0 / PI) * (dHertz * PI * fmod(dTime, 1.0 / dHertz) - (PI / 2.0));
	case 4:
		return 2.0 * ((double)rand() / (double)(RAND_MAX)) - 1.0;
	default: 
		return 0.0;
	}
}

// Creating a function to change the shape of the waveform that we have created, dTime tells about the time that has elapsed.
double MakeNoise(double dTime) {
	// Returning oscillators, based on the type provided to the function, here we can also use osc part as a composite of more than 1 function generations
	double dOutput = envelope.GetAmplitude(dTime) * osc(dFrequencyOutput, dTime, 3); 
	return dOutput * 0.2; // Master Volume
}

struct sEnvelopeADSR {
	double dAttackTime;
	double dDecayTime;
	double dReleaseTime;

	
	double dSustainAmplitude;
	
	// The amplitude of the initial attack may go to a different amplitude than the one it will rest at, thus to keep track of the starting amplitude
	double dStartAmplitude;
	
	// Times for when the user has pressed and released the key, based on which the envelope duration will be decided
	double dTriggerOnTime;
	double dTriggerOffTime;

	// State of Key 
	bool bNoteOn;

	// Default Values
	sEnvelopeADSR() {
		dAttackTime = 0.01;
		dDecayTime = 0.01;
		dReleaseTime = 0.02;
		dSustainAmplitude = 0.8;
		dStartAmplitude = 0.02;
		dTriggerOnTime = 0.0;
		dTriggerOffTime = 0.0;
		bNoteOn = false;
	}

	// The Envelope can be indexed at any point in time by the make noise function, thus we should return the amplitude of the function for the given time

	double GetAmplitude(double dTime) {
		double dAmplitude = 0.0;
		// The time after the note was pressed 
		double dLifeTime = dTime - dTriggerOnTime;
		if (bNoteOn) {
			// Attack
			if (dLifeTime <= dAttackTime) {
				dAmplitude = (dLifeTime / dAttackTime) * dStartAmplitude;
			}
			// Decay
			if (dLifeTime > dAttackTime && dLifeTime <= (dAttackTime+dDecayTime)) {
				dAmplitude = ((dLifeTime - dAttackTime)/ dDecayTime) * (dSustainAmplitude - dStartAmplitude) + dStartAmplitude;
			}
			// Sustain 
			if (dLifeTime > (dAttackTime + dDecayTime)) {
				dAmplitude = dSustainAmplitude;
			}
		}
		else {
			// Release
			dAmplitude = ((dTime - dTriggerOffTime) / (dReleaseTime)) * (0.0 - dSustainAmplitude) + dSustainAmplitude;
		}

		// When dAmplitude is too small, set it to zero
		if (dAmplitude <= 0.0001) {
			dAmplitude = 0;
		}

		return dAmplitude;
	}

	// Methods for Note ON and OFF
	
	void NoteOn(double dTime) {
		dTriggerOnTime = dTime;
		bNoteOn = true;
	}
	
	void NoteOff(double dTime) {
		dTriggerOffTime = dTime;
		bNoteOn = false;
	}

};

int main()
{
	// Get a list of all sound hardware, Enumerate uses mmepi.h command to get the count of connected audio devices and then iterates over them returning as a vector of strings
	vector<wstring> devices = olcNoiseMaker<short>::Enumerate();
	
	// Display all the found devices
	for (auto d : devices)
		wcout << "Found Object Device: " << d << endl;

	// Display a keyboard
	wcout << endl <<
		"|   |   |   |   |   | |   |   |   |   | |   | |   |   |   |" << endl <<
		"|   | S |   |   | F | | G |   |   | J | | K | | L |   |   |" << endl <<
		"|   |___|   |   |___| |___|   |   |___| |___| |___|   |   |__" << endl <<
		"|     |     |     |     |     |     |     |     |     |     |" << endl <<
		"|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |" << endl <<
		"|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|" << endl << endl;

	// Create a sound instance, using short as a 16 bit (4 byte) estimation of the sine wave, round((2^b - 1) * A)/(2^b - 1) approximates the sine wave to closest bitwise form
	// Arguments: OutputDevice, SampleRate, Channels, Blocks and BlockSamples 
	olcNoiseMaker<short> sound(devices[0], 44100, 1, 8, 512);

	// Linking MakeNoise with the created sound instance
	sound.SetUserFunction(MakeNoise);


	while (1) {
		// Adding a keyboard control to the sound played 
		
		bool bKeyPressed = false;
		for (int k = 0; k <= 15; k++) {
			// The 0x8000 bit of GetAsyncKeyState gives you the real time state of the key 
			if (GetAsyncKeyState((unsigned char)("ZSXCFVGBNJMK\xbcL\xbe\xbf"[k])) & 0x8000) {
				dFrequencyOutput = dOctaveBaseFrequency * pow(d12thRootOf2, k);
				envelope.NoteOn(sound.GetTime());
				bKeyPressed = true;
			}
		}

		if(!bKeyPressed) {
			dFrequencyOutput = 0.0;
			envelope.NoteOff(sound.GetTime());
		}
	}
}
