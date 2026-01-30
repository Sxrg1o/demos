#include "raylib.h"
#include <math.h>
#include <stdbool.h>


#define SQUARE_SIZE 31
#define STOP_EPS 5
#define elasticity 1.0f

typedef struct Ball{
    Vector2 position;
    int size;
    Vector2 speed;
    Color color;
    
}Ball;


const int screenWidth  = 1800;
const int screenHeight = 1500;
const Vector2 acel = {0 , 0};

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

void ColitionBalls(Ball *ball_1,Ball *ball_2){
    float dif_x = ball_2->position.x - ball_1->position.x;
    float dif_y = ball_2->position.y - ball_1->position.y;
    
    float norm_dif = sqrt(dif_x * dif_x + dif_y * dif_y );

    Vector2 normal_dif = {dif_x/norm_dif, dif_y/norm_dif};
    
    float v_nomal = ball_1->size + ball_2->size;
    float impulse = -((1 + elasticity)/2) * v_nomal;

    ball_1->speed.x = ball_1->speed.x - impulse * normal_dif.x;
    ball_1->speed.y = ball_1->speed.y - impulse * normal_dif.y;
    ball_2->speed.x = ball_2->speed.x - impulse * normal_dif.x;
    ball_2->speed.y = ball_2->speed.y - impulse * normal_dif.y;
}

void Friction(Ball *ball, float frime){
    const float k = 0.1f;
    float factor = expf(-k * frime);
    ball->speed.x *= factor;
    ball->speed.y *= factor;
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
    ball_1.speed = (Vector2) {400, 200};
    

    Ball ball_2;
    ball_2.color = BLACK;
    ball_2.position = (Vector2) {(float)screenWidth/4, (float)screenHeight/4};
    ball_2.size = 50;
    ball_2.speed = (Vector2) {800, 1000};
    


    InitWindow(screenWidth, screenHeight, "Billiards");
    while (!WindowShouldClose())
    {
        float time = GetFrameTime()*0.6f;

        for(int i = 0; i < 10; i++){
            StateUpdate(&ball_1, time);
            StateUpdate(&ball_2, time);
            //ColitionBalls(&ball_1, &ball_2);
            Friction(&ball_1,  time);
            Friction(&ball_2, time);
            StateLogic(&ball_1);
            StateLogic(&ball_2);

        }

        BeginDrawing();
    
        ClearBackground(RAYWHITE);
        DrawCircle(ball_1.position.x, ball_1.position.y, ball_1.size, ball_1.color);
        DrawCircle(ball_2.position.x, ball_2.position.y, ball_2.size, ball_2.color);

        DrawText(
    TextFormat("speed: (%.2f, %.2f)", ball_1.speed.x, ball_1.speed.y),
    10, 10, 20, BLACK
);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}





