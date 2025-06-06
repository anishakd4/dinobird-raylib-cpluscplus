#include <iostream>
#include <time.h>
#include "raylib.h"

typedef struct Sprite {
    Texture2D tex;
    Vector2 pos;
    Rectangle srect[4];

    int frames;
    int currentFrame;
    int actualWidth;
    int actualHeight;

    long double time;
    float animationTime;
} Sprite;

typedef struct Cactus {
    Texture2D tex;

    Rectangle srect[60];
    Vector2 pos[60];

    int size;
    int actualWidth;
    int actualHeight;
    int speed;
} Cactus;

void setupDinoSprite(Sprite* dino, int screenHeight) {
    dino->tex = LoadTexture("../res/dino.png");
    dino->frames = 3;
    dino->currentFrame = 0;
    dino->actualWidth = 64;
    dino->actualHeight = 64;
    dino->pos = (Vector2) { 150, screenHeight*0.7f - dino->actualHeight };
    for(int i = 0; i < dino->frames; i++) {
        dino->srect[i] = (Rectangle) { (float)dino->actualWidth*i, 0, (float)dino->actualWidth, (float)dino->actualHeight };
    }
    dino->time = 0;
    dino->animationTime = 0.16;
}

void setupDinoBirdSprite(Sprite* dinobird, int screenHeight) {
    dinobird->tex = LoadTexture("../res/dinobird.png");
    dinobird->frames = 4;
    dinobird->currentFrame = 0;
    dinobird->actualWidth = 32;
    dinobird->actualHeight = 18;
    dinobird->pos = (Vector2) { 700, screenHeight*0.6f };
    for(int i = 0; i < dinobird->frames; i++) {
        dinobird->srect[i] = (Rectangle) { (float)dinobird->actualWidth*i, 0, (float)dinobird->actualWidth, (float)dinobird->actualHeight };
    }
    dinobird->time = 0;
    dinobird->animationTime = 0.07;
}

void setupCactusSprite(Cactus* cactus, int screenWidth, int screenHeight){
    cactus->tex = LoadTexture("../res/cactus.png");
    cactus->size = sizeof(cactus->srect)/sizeof(cactus->srect[0]);
    cactus->actualWidth = cactus->actualHeight = 64;
    cactus->pos[0] = (Vector2) { (float)screenWidth, (float) (screenHeight*0.7f - cactus->actualHeight) };
    cactus->srect[0] = (Rectangle) { (float)(2 * cactus->actualWidth), 0, (float)cactus->actualWidth, (float)cactus->actualHeight };
    std::cout << "cactus->size : " << cactus->size << std::endl;
    for(int i = 1; i < cactus->size; i++) {
        cactus->pos[i] = (Vector2) { cactus->pos[i-1].x+rand()%screenWidth+screenWidth/2, screenHeight*0.7f - cactus->actualHeight };
        cactus->srect[i] = (Rectangle) { (float)(2 * cactus->actualWidth), 0, (float)cactus->actualWidth, (float)cactus->actualHeight };
    }
    cactus->speed = 9;
}

void sprite_animate(Sprite* s) {
    s->time += GetFrameTime();
    if(s->time > s->animationTime) {
        s->currentFrame++;
        s->time = 0;
    }

    if(s->currentFrame >= s->frames) {
        s->currentFrame = 0;
    }
}

bool dinobird_fly(Sprite* s, int speed) {
    s->pos.x -= speed;

    if(s->pos.x < -s->actualWidth) {
        int range = GetScreenHeight()*0.7 - GetScreenHeight()*0.5;
        int y = rand()%range + GetScreenHeight()*0.45;
        s->pos = (Vector2) { (float)GetScreenWidth(), (float)y };
        return false;
    }

    return true;
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    srand(time(NULL));

    // init
    InitWindow(screenWidth, screenHeight, "CLang Dino Run");
    SetTargetFPS(30);

    Sprite dino; 
    setupDinoSprite(&dino, screenHeight);

    Sprite dinobird; 
    setupDinoBirdSprite(&dinobird, screenHeight);

    Cactus cactus; 
    setupCactusSprite(&cactus, screenWidth, screenHeight);
    
    // variables
    float jump_v = 24;
    float dv = jump_v;
    float dg = 2.2;

    int birdSpeed = cactus.speed;
    long double speedUpTime = 0;
    long double birdFlyTime = 0;

    bool jump = false;
    bool birdfly = false;
    bool gameOver = false;

    while (!WindowShouldClose()) {
        // process events
        if(IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            jump = true;
        }

        // update
        if(!gameOver) {
            sprite_animate(&dino);
            sprite_animate(&dinobird);

            // jumping
            if(jump) {
                dino.pos.y -= dv;
                dv -= dg;

                if(dino.pos.y > screenHeight*0.7 - dino.actualHeight) {
                    dino.pos.y = screenHeight*0.7 - dino.actualHeight;
                    dv = jump_v;
                    jump = false;
                }
            }

            if(birdfly) {
                birdfly = dinobird_fly(&dinobird, birdSpeed);
                birdFlyTime = 0;
            }

            for(int i = 0; i < cactus.size; i++) {
                if(cactus.pos[i].x < -cactus.actualWidth*2) {
                    continue;
                }
                cactus.pos[i].x -= cactus.speed;
            }

            // speeding up the game
            speedUpTime += GetFrameTime();
            if(speedUpTime > 1.0) {
                cactus.speed += 1;
                speedUpTime = 0;
            }

            birdFlyTime += GetFrameTime();
            if(birdFlyTime > 6) {
                birdSpeed = cactus.speed+1;
                birdfly = true;
            }

            // collision detection
            for(int i = 0; i < cactus.size; i++) {
                if(cactus.pos[i].x > 0 && cactus.pos[i].x < screenWidth) {
                    if(CheckCollisionRecs((Rectangle) { dino.pos.x, dino.pos.y, (float)dino.actualWidth, (float)dino.actualHeight },
                        (Rectangle) { cactus.pos[i].x, cactus.pos[i].y, (float)cactus.actualWidth, (float)cactus.actualHeight })) {
                        gameOver = true;
                    }
                }
            }

            if(birdfly) {
                if(CheckCollisionRecs((Rectangle) { dino.pos.x, dino.pos.y, (float)dino.actualWidth, (float)dino.actualHeight },
                    (Rectangle) { dinobird.pos.x, dinobird.pos.y, (float)dinobird.actualWidth, (float)dinobird.actualHeight })) {
                        gameOver = true;
                    }
            }
        }

        // draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawLineEx((Vector2) { 0, screenHeight*0.7 }, (Vector2) { screenWidth, screenHeight*0.7 }, 5, BLACK);

        DrawRectangle(dino.pos.x, dino.pos.y, dino.actualWidth, dino.actualHeight, RED);
        DrawTextureRec(dino.tex, dino.srect[dino.currentFrame], dino.pos, WHITE);

        if(birdfly) {
            DrawTextureRec(dinobird.tex, dinobird.srect[dinobird.currentFrame], dinobird.pos, WHITE);
        }

        for(int i = 0; i < cactus.size; i++) {
            if(cactus.pos[i].x > -cactus.actualWidth && cactus.pos[i].x < screenWidth) {
                DrawRectangle(cactus.pos[i].x, cactus.pos[i].y, cactus.actualWidth, dino.actualHeight, GREEN);
                DrawTextureRec(cactus.tex, cactus.srect[i], cactus.pos[i], WHITE);
            }
        }

        if(gameOver) {
            DrawText("You lost!", screenWidth*0.35, screenHeight*0.3, 60, LIGHTGRAY);
        }

        EndDrawing();
    }

    // free mem
    UnloadTexture(cactus.tex);
    UnloadTexture(dinobird.tex);
    UnloadTexture(dino.tex);

    // quit
    CloseWindow();
    return 0;
}


