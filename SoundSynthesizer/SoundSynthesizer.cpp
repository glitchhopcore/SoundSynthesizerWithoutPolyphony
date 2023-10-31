// SoundSynthesizer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
using namespace std;
#include "olcNoiseMaker.h"

int main()
{
	//Get a list of all sound hardware, Enumerate uses mmepi.h command to get the count of connected audio devices and then iterates over them returning as a vector of strings
	vector<wstring> devices = olcNoiseMaker<short>::Enumerate();
	
	//Display all the found devices
	for (auto d : devices)
		wcout << "Found Object Device: " << d << endl;

	//Create a sound instance
	olcNoiseMaker<short> sound(devices[0], 44100, 1, 8, 512);

}
