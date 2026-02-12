/**
 * Author: Will Roche
 * Assignment: Simple 2D Scene Project
 * Date due: 02/14/2026
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/

#include "raylib.h"
#include <math.h>
#include <stdio.h>

// enums!
enum AppStatus  { TERMINATED, RUNNING };
enum TimeOfDay  { MORNING, NOON, EVENING, NIGHT };

// global constants for screen and object specs
constexpr int   SCREEN_WIDTH  = 1280,
                SCREEN_HEIGHT = 720,
                FPS           = 60;

constexpr float OBJ_SIZE     = 200.0f,
                ORBIT_RADIUS  = 200.0f,
                ORBIT_SPEED   = 1.5f,
                MAX_AMP       = 20.0f,
                PULSE_SPEED   = 2.0f,
                ROTATION_SPEED     = 120.0f;

constexpr Vector2 ORIGIN = {SCREEN_WIDTH  / 2.0f, SCREEN_HEIGHT / 2.0f};

//image paths (make sure they're there!)
constexpr char OBJ1_TEXTURE_FP[] = "assets/seaturtle.png";
constexpr char OBJ2_TEXTURE_FP[] = "assets/jellyfish.png";
constexpr char OBJ3_TEXTURE_FP[] = "assets/pufferfish.png";

AppStatus gAppStatus     = RUNNING;
float     gPreviousTicks = 0.0f;

// sea turtle
Texture2D gObj1Texture;
Vector2   gObj1Position  = ORIGIN;
float     gObj1Angle     = 0.0f;
Vector2   gObj1Scale     = { OBJ_SIZE, OBJ_SIZE };
float     gObj1Time      = 0.0f;

// jellyfish (relative to seaturtle so has orbit angle and slightly smaller)
Texture2D gObj2Texture;
Vector2   gObj2Position  = { 0.0f, 0.0f }; //doesn't matter since based on sea turtle (relative)
float     gObj2Angle     = 0.0f;
Vector2   gObj2Scale     = { OBJ_SIZE * 0.6f, OBJ_SIZE * 0.6f };
float     gObj2OrbitAngle = 0.0f;

// pufferfish (will pulse so also smaller to start, positioned away from sea turtle & jelly)
Texture2D gObj3Texture;
Vector2   gObj3Position  = { SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.25f };
float     gObj3Angle     = 0.0f;
Vector2   gObj3Scale     = { OBJ_SIZE * 0.5f, OBJ_SIZE * 0.5f };
float     gObj3PulseTime = 0.0f;

// background change optional
TimeOfDay gTimeOfDay     = MORNING; //start in morning
float     backgroundtime = 0.0f;
constexpr float daylength = 2;  // seconds per time of day


// function declarations (standard)
void initialise();
void processInput();
void update();
void render();
void shutdown();

// function definitions

//same init as lec3 just 3 objects
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple 2D Scene");
    
    gObj1Texture = LoadTexture(OBJ1_TEXTURE_FP);
    gObj2Texture = LoadTexture(OBJ2_TEXTURE_FP);
    gObj3Texture = LoadTexture(OBJ3_TEXTURE_FP);

    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    // delta time update (happens every time)
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    // sea turtle moves in a diagonal oval shape
    gObj1Time  += deltaTime;
    gObj1Position.x = ORIGIN.x + 200.0f + 100.0f * cosf(gObj1Time * 0.8f);
    gObj1Position.y = ORIGIN.y + 50.0f  * sinf(gObj1Time * 0.8f);

    // jellyfish orbits relative to sea turtle + rotates
    gObj2OrbitAngle += ORBIT_SPEED * deltaTime;
    gObj2Angle -= ROTATION_SPEED * deltaTime; //rotates counterclockwise so it's easier to notice
    gObj2Position.x = gObj1Position.x + ORBIT_RADIUS * cosf(gObj2OrbitAngle);
    gObj2Position.y = gObj1Position.y + ORBIT_RADIUS * sinf(gObj2OrbitAngle);

    // pufferfish pulses + swims in a circle
    gObj3PulseTime += deltaTime;
    float scaleAmount = OBJ_SIZE * 0.5f + MAX_AMP * sinf(gObj3PulseTime * PULSE_SPEED);
    gObj3Scale = { scaleAmount, scaleAmount };
    gObj3Position.x = SCREEN_WIDTH  * 0.25f + 150.0f * cosf(gObj3PulseTime * 0.4f);
    gObj3Position.y = SCREEN_HEIGHT * 0.5f  + 150.0f * sinf(gObj3PulseTime * 0.4f);

    // background cycle through the times of day enum (based on how long the day is set as)
    backgroundtime += deltaTime;
    int phase = (int)(backgroundtime / daylength) % 4;
    gTimeOfDay = (TimeOfDay)phase;
}

void render()
{
    BeginDrawing();

    // background color based on time of day
    Color waterColor;
    switch (gTimeOfDay) {
        case MORNING:  waterColor = BLUE;     break;
        case NOON:   waterColor = SKYBLUE;  break;
        case EVENING:  waterColor = DARKBLUE; break;
        case NIGHT:    waterColor = BLACK;    break;
    }

    ClearBackground(waterColor);

    // essentially the same render code as lecture 3 but for each object
    // texture area -> destination area -> object origin -> draw texture

    Rectangle textureArea1 = {
        0.0f, 0.0f,
        static_cast<float>(gObj1Texture.width),
        static_cast<float>(gObj1Texture.height)
    };
    Rectangle destinationArea1 = {
        gObj1Position.x,
        gObj1Position.y,
        static_cast<float>(gObj1Scale.x),
        static_cast<float>(gObj1Scale.y)
    };
    Vector2 objectOrigin1 = {
        static_cast<float>(gObj1Scale.x) / 2.0f,
        static_cast<float>(gObj1Scale.y) / 2.0f
    };
    DrawTexturePro(
        gObj1Texture,
        textureArea1,
        destinationArea1,
        objectOrigin1,
        gObj1Angle,
        WHITE
    );

    Rectangle textureArea2 = {
        0.0f, 0.0f,
        static_cast<float>(gObj2Texture.width),
        static_cast<float>(gObj2Texture.height)
    };
    Rectangle destinationArea2 = {
        gObj2Position.x,
        gObj2Position.y,
        static_cast<float>(gObj2Scale.x),
        static_cast<float>(gObj2Scale.y)
    };
    Vector2 objectOrigin2 = {
        static_cast<float>(gObj2Scale.x) / 2.0f,
        static_cast<float>(gObj2Scale.y) / 2.0f
    };
    DrawTexturePro(
        gObj2Texture,
        textureArea2,
        destinationArea2,
        objectOrigin2,
        gObj2Angle,
        WHITE
    );

    Rectangle textureArea3 = {
        0.0f, 0.0f,
        static_cast<float>(gObj3Texture.width),
        static_cast<float>(gObj3Texture.height)
    };
    Rectangle destinationArea3 = {
        gObj3Position.x,
        gObj3Position.y,
        static_cast<float>(gObj3Scale.x),
        static_cast<float>(gObj3Scale.y)
    };
    Vector2 objectOrigin3 = {
        static_cast<float>(gObj3Scale.x) / 2.0f,
        static_cast<float>(gObj3Scale.y) / 2.0f
    };
    DrawTexturePro(
        gObj3Texture,
        textureArea3,
        destinationArea3,
        objectOrigin3,
        gObj3Angle,
        WHITE
    );

    EndDrawing();
}

void shutdown()
{
    CloseWindow();
    UnloadTexture(gObj1Texture);
    UnloadTexture(gObj2Texture);
    UnloadTexture(gObj3Texture);
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}
