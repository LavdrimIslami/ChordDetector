#include "raylib.h"
#include "AudioSystem.h"
#include "gameplay.h"

int main() {
    InitWindow(800, 600, "Be My Rhythm Hero");
    SetTargetFPS(60);

    // Start our custom microphone engine!
    InitMicrophone();
    Tile myTile(200.0f, 150.0f, GREEN);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        float vol = currentVolume.load();
        const char* volText = TextFormat("Current Volume: %0.5f", vol);
        

        if (isMicActive.load()) {
            DrawText("Mic Active", 20, 20, 30, BLACK);
            std::lock_guard<std::mutex> lock(fftMutex);
            for (size_t i = 0; i < currentNotes.size(); ++i) {
                //DrawText(currentNotes[i].c_str(), 280, 280 + (int)i * 35, 30, BLACK);
                DrawText(sharedChordText.c_str(), 280, 280, 30, RED);

            }
            
        }
        else {
            DrawText("Mic Not Active", 20, 20, 30, RED);
        }
        DrawText(volText, 200, 500, 20, BLACK);    
        EndDrawing();
    }

    // Cleanup
    CloseMicrophone();
    CloseWindow();

    return 0;
}