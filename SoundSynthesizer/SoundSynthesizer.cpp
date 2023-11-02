// SoundSynthesizer.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Remember to add winmm.lib in the input dependencies from the project menu, also set the debug from x64 to x86 to avoid exception thrown error

#include <iostream>
using namespace std;
#include "olcNoiseMaker.h"

// Creating a function to change the shape of the waveform that we have created, dTime tells about the time that has elapsed.
double MakeNoise(double dTime) {
	// Returning a sine wave function with 440Hz being the note 'A'
	return 0.5 * sin(440.0 * 2 * PI * dTime);
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

	// Adding a while loop to keep the program running, will change this later
	while (1) {
	
	}
}
