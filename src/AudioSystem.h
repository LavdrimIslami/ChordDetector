#pragma once
#include <atomic>
#include <mutex>
#include <vector>
#define SIZE 8192

extern std::atomic<bool> isMicActive;
extern std::atomic<float> currentVolume;
extern float fftOutput[SIZE ];  // plain array
extern float fftMag[SIZE]; //magnitudes
extern float decimatedMag[SIZE];
extern std::mutex fftMutex;
extern std::vector<std::string> currentNotes;
extern std::string sharedChordText;


constexpr int MAX_HARMONICS = 5;

// Our custom microphone engine controls
void InitMicrophone();
void CloseMicrophone();
float fftMagnitude(float real, float imaginary);
std::vector<int> findPeaks(float* magnitudes, int size, int maxPeaks,int distance);
float MIDI(float frequency);