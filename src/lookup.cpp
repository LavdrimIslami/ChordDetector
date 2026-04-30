#include "lookup.h"
#include <unordered_map>
#include <vector>
#include <string>


std::unordered_map<uint16_t, std::string> chordTable;

uint16_t makeChordMask(const std::vector<int>& notes) {
    uint16_t mask = 0;
    for (int note : notes) {
        mask |= (1 << (note % 12)); 
    }
    return mask;
}

void InitTable() {

    const std::string rootNames[12] = {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };


    //im not a music major, i copied this from gemini.
    for (int i = 0; i < 12; ++i) {
        std::string root = rootNames[i];

        // --- TRIADS ---
        // Major: Root, +4 semitones, +7 semitones
        chordTable[makeChordMask({ i, i + 4, i + 7 })] = root + " Major";

        // Minor: Root, +3, +7
        chordTable[makeChordMask({ i, i + 3, i + 7 })] = root + " Minor";

        // Diminished: Root, +3, +6
        chordTable[makeChordMask({ i, i + 3, i + 6 })] = root + " Diminished";

        // Augmented: Root, +4, +8
        chordTable[makeChordMask({ i, i + 4, i + 8 })] = root + " Augmented";

        // Suspended 4 (Sus4): Root, +5, +7
        chordTable[makeChordMask({ i, i + 5, i + 7 })] = root + " Sus4";

        // Suspended 2 (Sus2): Root, +2, +7
        chordTable[makeChordMask({ i, i + 2, i + 7 })] = root + " Sus2";

        // --- 7th CHORDS ---
        // Major 7th: Root, +4, +7, +11
        chordTable[makeChordMask({ i, i + 4, i + 7, i + 11 })] = root + " Maj7";

        // Minor 7th: Root, +3, +7, +10
        chordTable[makeChordMask({ i, i + 3, i + 7, i + 10 })] = root + " Min7";

        // Dominant 7th (Dom7): Root, +4, +7, +10
        chordTable[makeChordMask({ i, i + 4, i + 7, i + 10 })] = root + " Dom7";

        // Fully Diminished 7th: Root, +3, +6, +9
        chordTable[makeChordMask({ i, i + 3, i + 6, i + 9 })] = root + " Dim7";

        // Half-Diminished 7th (m7b5): Root, +3, +6, +10
        chordTable[makeChordMask({ i, i + 3, i + 6, i + 10 })] = root + " Half-Dim7";
    }
}

