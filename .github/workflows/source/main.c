#include <3ds.h>
#include <citro2d.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define MAX_POINTS    10000

typedef struct {
    u16 x;
    u16 y;
    u32 color;
    bool is_continuous; 
} CustomPaintPoint;

CustomPaintPoint drawing[MAX_POINTS];
int pointCount = 0;
bool is_drawing_stroke = false;
u32 current_paint_color;

u32 color_palette[4] = {
    C2D_Color32(40, 40, 40, 255),    // Bold Black
    C2D_Color32(235, 70, 70, 255),   // Blush Red
    C2D_Color32(70, 140, 235, 255),  // Vibrant Blue
    C2D_Color32(255, 255, 255, 255)  // Highlight White
};
int active_color_idx = 0;

int main(int argc, char* argv[]) {
    gfxInitDefault();
    C3d_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2d_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2d_Prepare();

    PrintConsole topConsole;
    consoleInit(GFX_TOP, &topConsole);
    C3D_RenderTarget* bottomTarget = C2d_CreateMainTarget(GFX_BOTTOM, GFX_LEFT);
    
    current_paint_color = color_palette[0];

    printf("\x1b[3;2H=== CUSTOM FACE PAINT STUDIO ===");
    printf("\x1b[6;2H[D-PAD Left/Right] - Cycle Paint Color");
    printf("\x1b[8;2H[X BUTTON]         - Cycle Base Skin Tone");
    printf("\x1b[10;2H[START BUTTON]     - Wipe Layer Canvas");
    printf("\x1b[12;2H[SELECT BUTTON]    - Safe System Exit");

    u32 skin_tones[3] = {
        C2D_Color32(255, 220, 195, 255), // Fair
        C2D_Color32(225, 175, 135, 255), // Tan
        C2D_Color32(145, 95, 60, 255)     // Dark
    };
    int skin_idx = 0;

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        u32 kHeld = hidKeysHeld();

        if (kDown & KEY_SELECT) break;        
        
        if (kDown & KEY_START) {
            pointCount = 0; 
            is_drawing_stroke = false;
        }

        if (kDown & (KEY_DLEFT | KEY_DRIGHT)) {
            if (kDown & KEY_DRIGHT) active_color_idx = (active_color_idx + 1) % 4;
            else active_color_idx = (active_color_idx - 1 + 4) % 4;
            current_paint_color = color_palette[active_color_idx];
        }

        if (kDown & KEY_X) {
            skin_idx = (skin_idx + 1) % 3;
        }

        if (kHeld & KEY_TOUCH) {
            touchPosition touch;
            hidTouchRead(&touch);
            if (pointCount < MAX_POINTS) {
                drawing[pointCount].x = touch.px;
                drawing[pointCount].y = touch.py;
                drawing[pointCount].color = current_paint_color;
                drawing[pointCount].is_continuous = is_drawing_stroke;
                pointCount++;
            }
            is_drawing_stroke = true; 
        } else {
            is_drawing_stroke = false; 
        }

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2d_TargetClear(bottomTarget, C2D_Color32(30, 30, 35, 255));
        C2d_SceneBegin(bottomTarget);

        C2d_DrawCircleSolid(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 80, skin_tones[skin_idx]);

        for (int i = 0; i < pointCount; i++) {
            if (drawing[i].is_continuous && i > 0) {
                C2d_DrawLine(
                    drawing[i-1].x, drawing[i-1].y, drawing[i-1].color,
                    drawing[i].x, drawing[i].y, drawing[i].color,
                    4.0f, 0
                );
            } else {
                C2d_DrawCircleSolid(drawing[i].x, drawing[i].y, 0, 2.0f, drawing[i].color);
            }
        }
        C3D_FrameEnd(0);
    }

    C2d_Fini();
    C3d_Fini();
    gfxExit();
    return 0;
}
