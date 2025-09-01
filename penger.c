#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BACKGROUND CLITERAL(Color) { 0x16, 0x16, 0x16, 0xFF }

static const int WINDOW_WIDTH = 600;
static const int WINDOW_HEIGHT = 500;
static const char* WINDOW_TITLE = "Timer";

typedef struct {
    int hours;
    int minutes;
    int seconds;
    int milliseconds;
    double lastTime;
} Timer;

static Timer timer = {0};

void InitTimer(void)
{
    timer.hours = 0;
    timer.minutes = 0;
    timer.seconds = 0;
    timer.milliseconds = 0;
    timer.lastTime = GetTime();
}

void UpdateTimer(void)
{
    double currentTime = GetTime();
    double deltaTime = currentTime - timer.lastTime;
    
    int deltaMs = (int)(deltaTime * 1000);
    timer.milliseconds += deltaMs;
    
    if (timer.milliseconds >= 1000) {
        timer.seconds += timer.milliseconds / 1000;
        timer.milliseconds %= 1000;
    }
    
    if (timer.seconds >= 60) {
        timer.minutes += timer.seconds / 60;
        timer.seconds %= 60;
    }
    
    if (timer.minutes >= 60) {
        timer.hours += timer.minutes / 60;
        timer.minutes %= 60;
    }
    
    if (timer.hours >= 24) {
        timer.hours = 0;
    }
    
    timer.lastTime = currentTime;
}

Vector2 GetCenteredTextPosition(Font font, const char* text, float fontSize, float spacing)
{
    Vector2 textSize = MeasureTextEx(font, text, fontSize, spacing);
    Vector2 position = {
        (WINDOW_WIDTH - textSize.x) / 2.0f,
        (WINDOW_HEIGHT - textSize.y) / 2.0f
    };
    return position;
}

void DrawCenteredText(Font font, const char* text, float fontSize, float spacing, Color color)
{
    Vector2 position = GetCenteredTextPosition(font, text, fontSize, spacing);
    DrawTextEx(font, text, position, fontSize, spacing, color);
}

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    Image logo = LoadImage("penger.png");
    SetWindowIcon(logo);
    SetWindowPosition(1920 - GetScreenWidth(), 0);
    SetWindowOpacity(0.6f);
    SetTargetFPS(60);

    const char* home = getenv("HOME");
    const char *font_name = TextFormat("%s/%s", home, ".fonts/Iosevka-Regular.ttf");
    Font font = LoadFontEx(font_name, 120, 0, 0);
    
    if (font.texture.id == 0) {
        font = LoadFontEx("", 120, 0, 0);
        if (font.texture.id == 0) {
            font = GetFontDefault();
        }
    }
    
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    
    const float FONT_SIZE = 60.0f;
    const float SPACING = 2.0f;
    const Color TEXT_COLOR = WHITE;
    
    InitTimer();
    
    while (!WindowShouldClose())
    {
        UpdateTimer();
        
        BeginDrawing();
        ClearBackground(BACKGROUND);
        
        const char* timeText = TextFormat("%02d:%02d:%02d", 
                                        timer.hours, 
                                        timer.minutes, 
                                        timer.seconds);
        
        DrawCenteredText(font, timeText, FONT_SIZE, SPACING, TEXT_COLOR);
        
        EndDrawing();
    }
    
    if (font.texture.id != GetFontDefault().texture.id) {
        UnloadFont(font);
    }
    CloseWindow();
    
    return 0;
}
