// SoundSynthesizer.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Remember to add winmm.lib in the input dependencies from the project menu, also set the debug from x64 to x86 to avoid exception thrown error

#include <iostream>
using namespace std;
#include "olcNoiseMaker.h"

// Global variable that gets updated, responsible for setting output frequency
double dFrequencyOutput = 0.0;

// Creating a function to change the shape of the waveform that we have created, dTime tells about the time that has elapsed.
double MakeNoise(double dTime) {
	// Returning a sine wave function with 440Hz being the note 'A'
	return 0.2 * sin(dFrequencyOutput * 2 * PI * dTime);
}

int main()
{
	// Get a list of all sound hardware, Enumerate uses mmepi.h command to get the count of connected audio devices and then iterates over them returning as a vector of strings
	vector<wstring> devices = olcNoiseMaker<short>::Enumerate();
	
	// Display all the found devices
	for (auto d : devices)
		wcout << "Found Object Device: " << d << endl;

	// Create a sound instance, using short as a 16 bit (4 byte) estimation of the sine wave, round((2^b - 1) * A)/(2^b - 1) approximates the sine wave to closest bitwise form
	// Arguments: OutputDevice, SampleRate, Channels, Blocks and BlockSamples 
	olcNoiseMaker<short> sound(devices[0], 44100, 1, 8, 512);

	// Linking MakeNoise with the created sound instance
	sound.SetUserFunction(MakeNoise);

	const double dOctaveBaseFrequency = 110.0; // Base Frequency of the A2 Note
	const double d12thRootOf2 = pow(2.0, 1.0 / 12.0); // Since 12 Notes per octave and the frequency doubles

	while (1) {
		// Adding a keyboard control to the sound played 
		
		bool bKeyPressed = false;
		for (int k = 0; k < 15; k++) {
			// The 0x8000 bit of GetAsyncKeyState gives you the real time state of the key 
			if (GetAsyncKeyState((unsigned char)("ZSXCFVGBNJMK\xbcL\xbe"[k])) & 0x8000) {
				dFrequencyOutput = dOctaveBaseFrequency * pow(d12thRootOf2, k);
				bKeyPressed = true;
			}
		}

		if(!bKeyPressed) {
			dFrequencyOutput = 0.0;
		}
	}
}
