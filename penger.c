#include <raylib.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <libnotify/notify.h>
#include <string.h>

#define BACKGROUND CLITERAL(Color) { 0x16, 0x16, 0x16, 0xFF }

#define WINDOW_WIDTH 400 //int
#define WINDOW_HEIGHT 300 //int
static const char WINDOW_TITLE[] = "Penger";
#define FONT_SIZE 60.0f

static struct {
    int hours;
    int minutes;
    int seconds;
    int milliseconds;
    double lastTime;
    int isPaused;
    int isCountingDown;
    int isFinished;
} timer;

Sound bibSound;

void parseTime(char* input, int* hours, int* minutes, int* seconds) {
    *hours = 0;
    *minutes = 0;
    *seconds = 0;

    char* current = input;

    while (*current != '\0') {
        while (*current == ' ' || *current == '\t') {
            current++;
        }

        if (*current == '\0') break;

        int number = 0;
        char* start = current;

        while (*current >= '0' && *current <= '9') {
            number = number * 10 + (*current - '0');
            current++;
        }

        if (current == start) {
            current++;
            continue;
        }
        while (*current == ' ' || *current == '\t') {
            current++;
        }

        // Time unit
        if (*current == 'h') {
            *hours += number;
            current++;
            // Skip 'r' if it's "hr"
            if (*current == 'r') {
                current++;
            }
        } else if (*current == 'm') {
            *minutes += number;
            current++;
        } else if (*current == 's') {
            *seconds += number;
            current++;
        } else {
            current++;
        }
    }
}

void InitTimer(int hours, int minutes, int seconds, int isCountingDown) {
    timer.hours = hours;
    timer.minutes = minutes;
    timer.seconds = seconds;
    timer.milliseconds = 0;
    timer.lastTime = GetTime();
    timer.isPaused = 0;
    timer.isCountingDown = isCountingDown;
    timer.isFinished = 0;
}

void UpdateTimer(void) {
    if (timer.isPaused || timer.isFinished) return;

    double currentTime = GetTime();
    double deltaTime = currentTime - timer.lastTime;

    int deltaMs = (int)(deltaTime * 1000);
    timer.milliseconds += deltaMs;

    if (timer.milliseconds >= 1000) {
        if (timer.isCountingDown) {
            // Count down
            if (timer.seconds > 0) {
                --timer.seconds;
            } else if (timer.minutes > 0) {
                --timer.minutes;
                timer.seconds = 59;
            } else if (timer.hours > 0) {
                --timer.hours;
                timer.minutes = 59;
                timer.seconds = 59;
            }
            
            if (timer.hours == 0 && timer.minutes == 0 && timer.seconds == 0) {
                timer.isFinished = 1;
            }
        } else {
            // Count up
            ++timer.seconds;
            if (timer.seconds >= 60) {
                ++timer.minutes;
                timer.seconds = 0;
            }

            if (timer.minutes >= 60) {
                ++timer.hours;
                timer.minutes = 0;
            }

            if (timer.hours >= 24) timer.hours = 0;
        }
        timer.milliseconds %= 1000;
    }

    timer.lastTime = currentTime;
}

void DrawCenteredText(Font font, const char* text, float fontSize, float spacing, Color color) {
    Vector2 textSize = MeasureTextEx(font, text, fontSize, spacing);
    Vector2 position = {
        (WINDOW_WIDTH - textSize.x) / 2.0f,
        (WINDOW_HEIGHT - textSize.y) / 2.0f
    };
    DrawTextEx(font, text, position, fontSize, spacing, color);
}

void SendNotification(const char* title, const char* message) {
    notify_init("Penger Timer");
    NotifyNotification* notification = notify_notification_new(title, message, NULL);
    notify_notification_show(notification, NULL);
    g_object_unref(G_OBJECT(notification));
    notify_uninit();
}

void PlaySoundWhenTimeIsUp(void) {
    PlaySound(bibSound);
    while (IsSoundPlaying(bibSound)) {
        usleep(100000); 
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <time arguments>\n", argv[0]);
        printf("Examples:\n");
        printf("  %s 5s          - 5 seconds\n", argv[0]);
        printf("  %s 10m         - 10 minutes\n", argv[0]);
        printf("  %s 1hr         - 1 hour\n", argv[0]);
        printf("  %s 1hr 30m 15s - 1 hour 30 minutes 15 seconds\n", argv[0]);
        printf("  %s 10m 5s      - 10 minutes 5 seconds\n", argv[0]);
        return -1;
    }

    int hours = 0, minutes = 0, seconds = 0;
    int isCountingDown = 0;

    for (int i = 1; i < argc; i++) {
        int h = 0, m = 0, s = 0;
        parseTime(argv[i], &h, &m, &s);
        hours += h;
        minutes += m;
        seconds += s;
    }

    if (seconds >= 60) {
        minutes += seconds / 60;
        seconds = seconds % 60;
    }
    if (minutes >= 60) {
        hours += minutes / 60;
        minutes = minutes % 60;
    }

    if (hours > 0 || minutes > 0 || seconds > 0) {
        isCountingDown = 1;
    }

    SetTraceLogLevel(LOG_NONE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    InitAudioDevice();
    SetWindowPosition(GetMonitorWidth(0) - GetScreenWidth(), 0);
    SetWindowIcon(LoadImage("penger.png"));
    SetTargetFPS(60);

    Font font = {0};
    const char* homeDir = getenv("HOME");
    if (homeDir == NULL) {
        fprintf(stderr, "Error: HOME environment variable is not set.\n");
        exit(1);
    }

    char fontPath[256];
    snprintf(fontPath, sizeof(fontPath), "%s/.fonts/Iosevka-Regular.ttf", homeDir);

    font = LoadFontEx(fontPath, 120, 0, 0);
    if (!font.texture.id) {
        fprintf(stderr, "Error: Failed to load font from path: %s\n", fontPath);
        font = GetFontDefault();
    }

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    char soundPath[256];
    snprintf(soundPath, sizeof(soundPath), "%s/.local/audio/bip.mp3", homeDir);
    bibSound = LoadSound(soundPath);
    float vol = 0.3f;
    SetSoundVolume(bibSound, vol);
    InitTimer(hours, minutes, seconds, isCountingDown);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) {
            timer.isPaused = !timer.isPaused;
        }

        UpdateTimer();

        if (timer.isFinished) {
            SendNotification("Ur mom!!", "Get Ur Ass Up.");
            PlaySoundWhenTimeIsUp();
            break;
        }

        BeginDrawing();
        ClearBackground(BACKGROUND);

        const char* timeText = TextFormat("%02d:%02d:%02d", timer.hours, timer.minutes, timer.seconds);

        Color currentColor = timer.isPaused ? RED : WHITE;
        DrawCenteredText(font, timeText, FONT_SIZE, 2.f, currentColor);
        EndDrawing();
    }

    UnloadSound(bibSound);
    UnloadFont(font);
    CloseWindow();
    CloseAudioDevice();
    return 0;
}
