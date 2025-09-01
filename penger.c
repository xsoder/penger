#include <raylib.h>
#include <stdlib.h>

#define BACKGROUND CLITERAL(Color) { 0x16, 0x16, 0x16, 0xFF }

#define WINDOW_WIDTH 600 //int
#define WINDOW_HEIGHT 500 //int
static const char WINDOW_TITLE[] = "Penger";
#define FONT_SIZE 60.0f
#define TEXT_COLOR WHITE

static struct{
    int hours;
    int minutes;
    int seconds;
    int milliseconds;
    double lastTime;
}timer;

void InitTimer(void){
    timer.hours = 0;
    timer.minutes = 0;
    timer.seconds = 0;
    timer.milliseconds = 0;
    timer.lastTime = GetTime();
}

void UpdateTimer(void){
    double currentTime = GetTime();
    double deltaTime = currentTime - timer.lastTime;
    
    int deltaMs = (int)(deltaTime * 1000);
    timer.milliseconds += deltaMs;
    
    if (timer.milliseconds >= 1000){
        ++timer.seconds;
        timer.milliseconds %= 1000;
    }
    
    if (timer.seconds >= 60){
        ++timer.minutes;
        timer.seconds %= 60;
    }
    
    if (timer.minutes >= 60){
        ++timer.hours;
        timer.minutes %= 60;
    }
    
    if (timer.hours >= 24)
        timer.hours = 0;

    timer.lastTime = currentTime;
}

void DrawCenteredText(Font font, const char* text, float fontSize, float spacing, Color color){
    Vector2 textSize = MeasureTextEx(font, text, fontSize, spacing);
    Vector2 position = {
        (WINDOW_WIDTH - textSize.x) / 2.0f,
        (WINDOW_HEIGHT - textSize.y) / 2.0f
    };
    DrawTextEx(font, text, position, fontSize, spacing, color);
}

int main(void){
    SetTraceLogLevel(LOG_NONE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    
    SetWindowPosition(GetMonitorWidth(0) - GetScreenWidth(), 0);

    SetWindowIcon(LoadImage("penger.png"));
    SetTargetFPS(60);


    Font font;
    {
        const char *font_name = TextFormat("%s/%s", getenv("HOME"), ".fonts/Iosevka-Regular.ttf");
        font = LoadFontEx(font_name, 120, 0, 0);
    }
    
    if(!font.texture.id){
        font = LoadFontEx("", 120, 0, 0);//raylib garbage
        if(!font.texture.id)
            font = GetFontDefault();//mental retardation
    }
    
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    
    InitTimer();
    
    while(!WindowShouldClose()){
        UpdateTimer();
        
        BeginDrawing();
        ClearBackground(BACKGROUND);//raylib garbage retardation
        
        const char* timeText = TextFormat("%02d:%02d:%02d", 
                                        timer.hours, 
                                        timer.minutes, 
                                        timer.seconds);
        
        DrawCenteredText(font, timeText, FONT_SIZE, 2.f, TEXT_COLOR);
        
        EndDrawing();
    }
    return 0;
}
