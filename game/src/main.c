/*
* Init commit written in just under 1 hour. Some extra debugging around the QuadTreeQueryResult's calloc / realloc pushed it from being a clean 48:45.
* Written for raylib v4.6-dev https://github.com/raysan5/raylib/commit/bc15c19
* 
* FIXES
* - The QuadTree struct lacks Dispose (free) methods. How will freeing the memory work since the 4 children are now allocated contiguously? (see QuadTreeSubdivide)
* 
* IMPROVEMENTS
* - Expand to V2 by adding some info drawn to the screen comparing the projected n^2 vs. the nlogn estimate, and finally the actual point count vs. the number of points checked (similar to my JS example)
* - Expand to V3 by adding a practical example of a bunch of objects colliding with each other (rock paper scissors game of life!)
*/

#include "raylib.h"
#include "raymath.h"

#include "quadtree.h"

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

#define POINT_RATE 30.0f
#define TIME_BETWEEN_POINTS 1.0f / POINT_RATE
#define NEARBLACK (Color){15, 15, 15}

QuadTree qtree;

float nextPointTime = 0;

void UpdateDrawFrame(void);

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib game");
    SetTargetFPS(60);

    qtree = QuadTreeCreate(0, 0, 500, 500);

    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }

    CloseWindow();

    return 0;
}

void UpdateDrawFrame(void)
{
    float dt = GetFrameTime();
    float time = GetTime();
    Vector2 mPos = GetMousePosition();

    QuadTreeQueryResult result = { 0 };
    Rectangle query = {mPos.x - 50, mPos.y - 50, 100, 100};

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && time > nextPointTime)
    {
        QuadTreeInsert(&qtree, GetMousePosition());
        nextPointTime = time + TIME_BETWEEN_POINTS;
    }

    QuadTreeQuery(qtree, query, &result);

    BeginDrawing();
    {
        ClearBackground(NEARBLACK);

        QuadTreeDebugDraw(qtree);
        QuadTreeQueryDebugDraw(result, query);
    }
    EndDrawing();
}