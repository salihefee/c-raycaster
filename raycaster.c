#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(void) {
    int windowWidth = 1200;
    int windowHeight = windowWidth / 2 + 2;
    int mapSize = 10;
    InitWindow(windowWidth, windowHeight, "OH MY GOD IT IS WORKING");

    // The map is a 2D array of 1s and 0s, where 1s are walls and 0s are empty space
    int gameMap[10][10] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 1, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 1, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 1, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1} };

    SetTargetFPS(60);

    // The angle between every ray being cast
    int degStep = 1;

    typedef struct {
        int playerSize;

        int playerX;
        int playerY;

        int fov;

        int heading;
    } player;

    player player1;
    player1.playerSize = 6;
    int halfSize = player1.playerSize / 2;
    player1.playerX = 75 - halfSize;
    player1.playerY = 75 - halfSize;
    player1.fov = 60;
    player1.heading = 45;
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GRAY);

        // Drawing the background
        DrawRectangle(windowWidth / 2, 0, windowWidth / 2, windowHeight / 2, BLUE);
        DrawRectangle(windowWidth / 2, windowHeight / 2, windowWidth / 2, windowHeight / 2, BROWN);

        int squareSize = windowHeight / mapSize - 2;

        // Offsetting the starting position so we get empty lines at the top left of the canvas as well
        int squareX = 2;
        int squareY = 2;

        // Rendering the grid
        for (int row = 0; row < mapSize; row++) {
            squareX = 2;
            for (int column = 0; column < mapSize; column++) {
                if (gameMap[row][column]) {
                    DrawRectangle(squareX, squareY, squareSize, squareSize, WHITE);
                }
                else {
                    DrawRectangle(squareX, squareY, squareSize, squareSize, BLACK);
                }
                squareX += windowHeight / mapSize;
            }
            squareY += windowHeight / mapSize;
        }

        int tileSize = windowHeight / mapSize;

        // Drawing the player
        DrawRectangle(player1.playerX, player1.playerY, player1.playerSize, player1.playerSize, RED);

        int playerXNormalized = player1.playerX + halfSize;
        int playerYNormalized = player1.playerY + halfSize;

        int movementSpeed = 2;

        // Movement handling
        if (IsKeyDown(KEY_A)) {
            int newXMap = (int)floor((player1.playerX - movementSpeed) / tileSize);
            int playerYMap = (int)floor(playerYNormalized / tileSize);
            if (!gameMap[playerYMap][newXMap])
                player1.playerX -= movementSpeed;
        }
        if (IsKeyDown(KEY_D)) {
            int newXMap = (int)floor((player1.playerX + player1.playerSize + movementSpeed) / tileSize);
            int playerYMap = (int)floor(playerYNormalized / tileSize);
            if (!gameMap[playerYMap][newXMap])
                player1.playerX += movementSpeed;
        }
        if (IsKeyDown(KEY_W)) {
            int newYMap = (int)floor((player1.playerY - movementSpeed) / tileSize);
            int playerXMap = (int)floor(playerXNormalized / tileSize);
            if (!gameMap[newYMap][playerXMap])
                player1.playerY -= movementSpeed;
        }
        if (IsKeyDown(KEY_S)) {
            int newYMap = (int)floor((player1.playerY + player1.playerSize + movementSpeed) / tileSize);
            int playerXMap = (int)floor(playerXNormalized / tileSize);
            if (!gameMap[newYMap][playerXMap])
                player1.playerY += movementSpeed;
        }

        if (IsKeyDown(KEY_Q)) {
            player1.heading -= 1;
        }
        if (IsKeyDown(KEY_E)) {
            player1.heading += 1;
        }

        player1.heading = fmod(player1.heading, 360);
        if (player1.heading < 0)
            player1.heading += 360;

        int deg = player1.heading - player1.fov / 2;

        int lineCount = player1.fov / degStep;

        int* distances = (int*) calloc(lineCount, sizeof(int));

        // Drawing the rays
        for (int lines = 0; lines < lineCount; lines++) {
            float raylength = 0;
            while (raylength < sqrt(pow(windowHeight, 2) * pow(windowWidth, 2))) {
                int xPos = playerXNormalized + raylength * cos(deg * PI / 180);
                int yPos = playerYNormalized + raylength * sin(deg * PI / 180);
                if (gameMap[(int)floor(yPos / tileSize)][(int)floor(xPos / tileSize)]) {
                    DrawLine(playerXNormalized, playerYNormalized, xPos, yPos, RED);
                    if ((int)floor(xPos / tileSize) != (int)floor((playerXNormalized + (raylength - 0.1) * cos(deg * PI / 180)) / tileSize)) {
                        distances[lines] = (int)(raylength * cos(abs(deg - player1.heading) * PI / 180)) | 0x8000;
                    }
                    else {
                        distances[lines] = (int) raylength * cos(abs(deg - player1.heading) * PI / 180);
                    }
                    break;
                }
                raylength += 0.1;
            }
            deg += degStep;
        }

        int lineSize = (windowWidth / 2) / lineCount;

        for (int i = 0; i < lineCount; i++) {
            int distance = distances[i] & 0x7FFF;
            int lineHeight = (64 * windowHeight) / distance;
            if (lineHeight > windowHeight)
                lineHeight = windowHeight;
            if (distances[i] & 0x8000) {
                DrawRectangle(windowWidth / 2 + lineSize * i, windowHeight / 2 - lineHeight / 2, lineSize, lineHeight, DARKGREEN);
            }
            else {
                DrawRectangle(windowWidth / 2 + lineSize * i, windowHeight / 2 - lineHeight / 2, lineSize, lineHeight, GREEN);
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}