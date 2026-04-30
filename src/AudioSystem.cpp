#define MINIAUDIO_IMPLEMENTATION
#define _USE_MATH_DEFINES
#include "miniaudio.h"
#include "pffft.h"
#include "AudioSystem.h"
#include "lookup.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <list>

void InitMicrophone();
void fft(float* samples, ma_uint32 frameCount);

float MIDI(float frequency);

constexpr float AMPLITUTE_THRESHOLD = 0.1f;
float fftOutput[SIZE];  // plain array
float fftMag[SIZE]; //magnitudes
float decimatedMag[SIZE]; 
std::vector<std::string> currentNotes;
PFFFT_Setup* setup;
std::mutex fftMutex;
std::atomic<bool> isMicActive = false;
std::atomic<float> currentVolume = 0.0f;

std::string sharedChordText = "Chord:.....";

ma_device micDevice; 

void AudioInputCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    if (pInput == nullptr) return;

    float* samples = (float*)pInput; 
    float maxAmplitude = 0.0f;

    for (ma_uint32 i = 0; i < frameCount; i++) {
        float absSample = std::abs(samples[i]);
        if (absSample > maxAmplitude) {
            maxAmplitude = absSample;
        }
    }

    currentVolume.store(maxAmplitude);

    if (maxAmplitude > AMPLITUTE_THRESHOLD) {
        isMicActive.store(true);
    }
    else {
        isMicActive.store(false);
    }

    fft(samples, frameCount);
}

void fft(float* samples, ma_uint32 frameCount){    
    float* input = (float*)pffft_aligned_malloc(SIZE * sizeof(float)); //real buffer
    float* output = (float*)pffft_aligned_malloc(SIZE * sizeof(float)); //real
    float* work = (float*)pffft_aligned_malloc(SIZE * sizeof(float)); //scratch
    
    //this is how you set a buffer to an input
    memcpy(input, samples, SIZE * sizeof(float));

    //hann window to smooth everything fix leakage
    for (int i = 0; i < SIZE; ++i) {
        using namespace std;
        input[i] *= 0.5 * (1 - cos((2 * M_PI) * i / (SIZE - 1)));
    }

    pffft_transform_ordered(setup, input, output, work, PFFFT_FORWARD);
    
    //auto lock unlock
    {
        std::lock_guard<std::mutex> lock(fftMutex);
        memcpy(fftOutput, output, (SIZE) * sizeof(float));

    }

    //so this finds all the magnitudes maps it to new array  maps step 2 index back into regular sequence
    for (int i = 0; i < SIZE; i += 2) {
        fftMag[i / 2] = fftMagnitude(fftOutput[i], fftOutput[i + 1]);
    }

    //decimation
    //take every Nth sample from the array, 
    //memcpy(decimatedMag, fftMag, (SIZE) * sizeof(float));

    //for (int i = 0; i < SIZE / (2 * MAX_HARMONICS); ++i) {
    //    decimatedMag[i] *= fftMag[i * 2];
    //    decimatedMag[i] *= fftMag[i * 3];
    //    decimatedMag[i] *= fftMag[i * 4];
    //    decimatedMag[i] *= fftMag[i * 5];
    //}

    std::vector<int> peaks = findPeaks(fftMag, SIZE / (2 * MAX_HARMONICS), 5, 5);
    static const char* noteNames[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    {
        std::lock_guard<std::mutex> lock(fftMutex);
        currentNotes.clear();
        std::vector<int> temp;
    for (auto i : peaks) {
        float binIndex = static_cast<float>(i);

        float frequencyHZ = binIndex * (44100.0f / SIZE);

        int MIDINumber = std::round(MIDI(frequencyHZ));

        int noteNum = ((MIDINumber % 12) + 12) % 12;
        currentNotes.push_back(noteNames[noteNum]);
        temp.push_back(noteNum);
    }

    uint16_t currentMask = makeChordMask(temp);

    if (chordTable.find(currentMask) != chordTable.end()) {
        sharedChordText = chordTable[currentMask];
    }


    }
    pffft_aligned_free(input);
    pffft_aligned_free(output);
    pffft_aligned_free(work);
    
    
}


// Turn on the hardware
void InitMicrophone() {
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format = ma_format_f32; // We want floats
    deviceConfig.capture.channels = 1;             // Mono audio
    deviceConfig.sampleRate = 44100;         // Standard sample rate
    deviceConfig.dataCallback = AudioInputCallback;
    deviceConfig.periodSizeInFrames = SIZE; //chunk
    setup = pffft_new_setup(SIZE, PFFFT_REAL);

    // Initialize and start listening
    if (ma_device_init(NULL, &deviceConfig, &micDevice) == MA_SUCCESS) {
        InitTable();
        ma_device_start(&micDevice);
    }

}

// Turn off the hardware
void CloseMicrophone() {
    ma_device_uninit(&micDevice);
    pffft_destroy_setup(setup);
}


float fftMagnitude(float real, float imaginary) {
    float nr = real * real;
    float ni = imaginary * imaginary;
    float res = nr + ni;
    
    float magnitude = std::sqrt(res);

    return magnitude;
}

std::vector<int> findPeaks(float* magnitudes, int size, int maxPeaks, int distance) {
    float maxMag = *std::max_element(magnitudes, magnitudes + size);
    float threshold = maxMag * 0.25f;

    std::vector<int> indices;
    for (int i = 0; i < size; ++i) {
        if (magnitudes[i] > threshold)
            indices.push_back(i);
    }
    
    std::sort(indices.begin(), indices.end(), [&](int a, int b) {return magnitudes[a] > magnitudes[b];});
    std::vector<int> accepted;

    for (int idgf : indices) {
        using namespace std;
        bool close = false;

        for (int acc : accepted)
            if (abs(idgf - acc) < distance) {
                close = true;
                break;
            }

        if (!close) {
            accepted.push_back(idgf);
        }
        if (static_cast<int>(accepted.size()) == maxPeaks) {
            break;
        }
    }

    return accepted;
}

float MIDI(float frequency) {
    
    return 69 + 12 * log2(frequency / 440);
}

