#include "raylib.h"
#include <time.h>
#include <stdio.h>
    #define SCREEN_WIDTH 335
    #define SCREEN_HEIGHT 143
    #define TITLE "Boom Pomodoro"
    #define TIMER_SCREEN_X 92
    #define TIMER_SCREEN_Y 54
    #define TIMER_SCREEN_WIDTH 102
    #define TIMER_SCREEN_HEIGHT 34

void time_to_string(int target, char result[200])
{
    int minutes = target / 60;
    int seconds = (target - (minutes * 60));

    if (seconds >= 0 && seconds <= 9 && minutes >= 0 && minutes <= 9) {
        sprintf_s(result, 200, "0%d : 0%d", minutes, seconds);
    }
    else if (minutes >= 0 && minutes <= 9)
    {
        sprintf_s(result, 200, "0%d : %d", minutes, seconds);
    }
    else if (seconds >= 0 && seconds <= 9) {
        sprintf_s(result, 200, "%d : 0%d", minutes, seconds);
    }
    else {
        sprintf_s(result, 200, "%d : %d", minutes, seconds);
    }
}

int main(int ac, char **av)
{
    /* Window */
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
    int current_monitor = GetCurrentMonitor();
    int width = GetMonitorWidth(current_monitor);
    int height = GetMonitorHeight(current_monitor);
    Vector2 win_pos = { width - SCREEN_WIDTH - 30,  60};

    SetWindowPosition(win_pos.x, win_pos.y);
    SetWindowState(FLAG_WINDOW_TOPMOST | FLAG_WINDOW_UNDECORATED);
    SetTargetFPS(60);

    /* Sounds */
    InitAudioDevice();
    Sound beep = LoadSound("beep.wav"), boom = LoadSound("boom.wav");

    /* dynamite */
    Texture dynamite = LoadTexture("dynamite.png");
    dynamite.width = SCREEN_WIDTH;
    dynamite.height = SCREEN_HEIGHT;

    /* explosion */
    Texture explosion = LoadTexture("explosion.png");

    /* timer && explosion */
    char displayed_time[200]="";
    
    int time_set_in_sec = 1800;
    int remaining_time = time_set_in_sec;
    time_to_string(time_set_in_sec, displayed_time);

    clock_t last_call,elapsed,boom_clock;
    int boom_rec_x = 0;
    bool end_boom_sfx = false;


    /* Core */
    bool started = false;


    /*Moveable Window*/
    Vector2 offset;

    while (!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            /*capture win and mouse pos*/
            Vector2 win_pos = GetWindowPosition();
            Vector2 mouse_pos = GetMousePosition();
            offset = (Vector2) { mouse_pos.x - win_pos.x, mouse_pos.y - win_pos.y };
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            Vector2 new_mouse_pos = GetMousePosition();
            int x = new_mouse_pos.x - offset.x;
            int y = new_mouse_pos.y - offset.y;
            SetWindowPosition(x, y);
        }
        if (IsKeyPressed(KEY_S) && !started)
        {
            PlaySound(beep);
            started = true;
            last_call = clock();
            boom_clock = clock();
        }

        if (IsKeyPressed(KEY_UP) && !started)
        {
            PlaySound(beep);
            time_set_in_sec += 1800;
            remaining_time = time_set_in_sec;
            time_to_string(time_set_in_sec, displayed_time);
        }

        if (IsKeyPressed(KEY_DOWN) && !started)
        {
            PlaySound(beep);
            time_set_in_sec -= (time_set_in_sec > 1800) ? 1800 : 0;
            time_to_string(time_set_in_sec, displayed_time);
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            break;
        }

        if (IsKeyPressed(KEY_R)) {
            PlaySound(beep);
            time_set_in_sec = 1800;
            remaining_time = time_set_in_sec;
            started = false;
            time_to_string(remaining_time, displayed_time);
            last_call = clock();
            boom_clock = clock();
        }

        if (remaining_time == time_set_in_sec / 2)
        {
            PlaySound(beep);
        }

        if (remaining_time != 0 && started)
        {
            elapsed = (clock() - last_call);
            if ( elapsed/CLOCKS_PER_SEC == 1)
            {
                if (remaining_time <= 10)
                {
                    PlaySound(beep);
                }
                remaining_time -= 1;
                time_to_string(remaining_time, displayed_time);
                last_call = clock();
            }
        }


        BeginDrawing();
            ClearBackground(BLANK);
            DrawTexture(dynamite, 0, 0, WHITE);
            DrawRectangle(TIMER_SCREEN_X, TIMER_SCREEN_Y, TIMER_SCREEN_WIDTH,TIMER_SCREEN_HEIGHT, BLACK);
            DrawText(displayed_time, TIMER_SCREEN_X + (TIMER_SCREEN_WIDTH - 75) / 2 , TIMER_SCREEN_Y + (TIMER_SCREEN_HEIGHT - 16) / 2, 24, RED);
            if (remaining_time == 0 && boom_rec_x < explosion.width) {
                if (!end_boom_sfx)
                {
                    PlaySound(boom);
                    end_boom_sfx = true;
                }
            
                if (((double)(clock() - boom_clock) / CLOCKS_PER_SEC) * 1000.0 >= 100) {
                    boom_rec_x += explosion.width/8;
                    boom_clock = clock();
                }
                DrawTextureRec(explosion, (Rectangle) { boom_rec_x , 0, explosion.width/8, explosion.height }, (Vector2) { SCREEN_WIDTH/2 - (explosion.width/8/2), SCREEN_HEIGHT/2 - (explosion.height / 2)}, WHITE);
            }
        EndDrawing();
    }
    UnloadTexture(dynamite);
    UnloadTexture(explosion);
    UnloadSound(beep);
    UnloadSound(boom);
    CloseWindow();
}