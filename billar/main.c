#include "raylib.h"
#include <math.h>
#include <stdbool.h>


#define SQUARE_SIZE 31
#define STOP_EPS 0.01

typedef struct Ball{
    Vector2 position;
    int size;
    Vector2 speed;
    Color color;
}Ball;


const int screenWidth  = 1800;
const int screenHeight = 1500;
const float acel_x = 0;
const float acel_y = 0;
const Vector2 acel = {acel_x , acel_y};
float position_relative(float speed, float aceleration, float position, float frame){
    return position + ( (speed + (0.5f) * aceleration * frame )* frame);
}
float speed_relative(float speed, float aceleration, float frame){
    return  speed + aceleration * frame;
}

void Stop(Ball *ball){
    if(fabs(ball->speed.x) < STOP_EPS) ball->speed.x = 0.0f;
    if(fabs(ball->speed.y) < STOP_EPS) ball->speed.y = 0.0f;
}

void StateUpdate(Ball *ball, float frame){
    ball->speed.x = speed_relative(ball->speed.x, acel.x, frame);
    ball->speed.y = speed_relative(ball->speed.y, acel.y, frame);
    ball->position.x = position_relative(ball->speed.x, acel.x, ball->position.x, frame);
    ball->position.y = position_relative(ball->speed.y, acel.y,  ball->position.y, frame);
}

void StateLogic(Ball *ball){
    const float epsilon = 0.8f;
    float r= (float)ball->size;

    if( ball->position.x > screenWidth - r){
        ball->position.x = screenWidth - r;
        ball->speed.x = -ball->speed.x * epsilon;
    }
    else if(ball->position.x < r){
        ball->position.x = r;
        ball->speed.x = -ball->speed.x * epsilon;
    }
    
    if( ball->position.y > screenHeight - r){
        ball->position.y = screenHeight - r;
        ball->speed.y = -ball->speed.y * epsilon;
    }
    else if(ball->position.y < r){
        ball->position.y = r;
        ball->speed.y = -ball->speed.y * epsilon;
    }
    Stop(ball);
}
int main(void)
{

    
    Ball ball_1;
    ball_1.color = BLACK;
    ball_1.position = (Vector2) {(float)screenWidth/2, (float)screenHeight/2};
    ball_1.size = 50;
    ball_1.speed = (Vector2) {0, 1};


    InitWindow(screenWidth, screenHeight, "Billiards");
    while (!WindowShouldClose())
    {
        float time = GetFrameTime() * 0.6f;

        for (int i = 0; i < 10; i++){
            StateUpdate(&ball_1, time);
            }
        StateLogic(&ball_1);

        BeginDrawing();
    
        ClearBackground(RAYWHITE);
        DrawCircle(ball_1.position.x, ball_1.position.y, ball_1.size, ball_1.color);
        DrawText(
    TextFormat("speed: (%.2f, %.2f)", ball_1.speed.x, ball_1.speed.y),
    10, 10, 20, BLACK
);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}




