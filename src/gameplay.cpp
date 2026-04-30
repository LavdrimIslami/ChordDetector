#include "gameplay.h"
#include "AudioSystem.h"
#include <vector>
#include <algorithm>


 
TargetNote targetNote;

struct {

	std::string type; 
	std::string name;
	std::vector<std::string> notes; 

} targetNotes;


	Tile::Tile(float startX, float startY, Color color) {
		position = { startX, startY };
		this->color = color; 
		isMatched = false;
	}
    
   
    void Tile::update() {
        if (!currentNotes.empty()) {
            std::vector<std::string> sortedCurrent = currentNotes;
            std::vector<std::string> sortedTarget = targetNote.notes;
            std::sort(sortedCurrent.begin(), sortedCurrent.end());
            std::sort(sortedTarget.begin(), sortedTarget.end());

            isMatched = (sortedCurrent == sortedTarget);
        }

        if (isMatched) {
            DrawRectangle(200, 150, 400, 300, GREEN);
        }
    }

