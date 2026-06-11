#include <windows.h>
#include "core.h"
#include "ui.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    if (!UI_Init()) return -1;

    GameState state;
    Core_Init(&state);
    InputState input = {0};

    LARGE_INTEGER frequency, t1, t2;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);

    bool running = true;
    while (running) {
        QueryPerformanceCounter(&t2);
        float delta_time = (float)(t2.QuadPart - t1.QuadPart) / (float)frequency.QuadPart;
        
        if (delta_time > 0.1f) {
            delta_time = 0.1f; 
        }
        
        t1 = t2;

        if (!UI_ProcessMessages()) {
            break;
        }

        UI_UpdateInput(&input);
        Core_Update(&state, &input, delta_time);

        if (state.play_damage_sound) {
            MessageBeep(MB_ICONERROR);
            state.play_damage_sound = false;
        }

        UI_Render(&state, &input);
    }

    UI_Shutdown();
    return 0;
}