
#include "raylib.h"
#include <vector>
#include <fstream>
#include <string>

struct GameData {
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
    Color tint;
};

bool isOnGround(GameData scarfyData, int windowDimentions[]) {
    return scarfyData.pos.y + scarfyData.rec.y >= windowDimentions[1] - scarfyData.rec.height;
}

GameData updateGameData(GameData data, float deltaTime, int maxFrame) {
    if (data.runningTime >= data.updateTime) {
        data.runningTime = 0.0f;
        data.rec.x = data.frame * data.rec.width;
        data.frame = (data.frame + 1) % (maxFrame + 1);
    }
    return data;
}

void updateBackground(Texture2D texture, float deltaTime, float speed, float& xPos) {
    xPos -= speed * deltaTime;
    if (xPos < -GetScreenWidth()) xPos = 0.0f;

    Rectangle src = { 0, 0, (float)texture.width, (float)texture.height };
    Rectangle dst1 = { xPos, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
    Rectangle dst2 = { xPos + GetScreenWidth(), 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
    Vector2 origin = { 0, 0 };

    DrawTexturePro(texture, src, dst1, origin, 0.0f, WHITE);
    DrawTexturePro(texture, src, dst2, origin, 0.0f, WHITE);
}

float getNextNebulaX(float previousX) {
    return previousX + 300 + GetRandomValue(0, 100);
}

void SaveHighScore(int score) {
    std::ofstream out("highscore.txt");
    if (out.is_open()) {
        out << score;
        out.close();
    }
}

int LoadHighScore() {
    std::ifstream in("highscore.txt");
    int score = 0;
    if (in.is_open()) {
        in >> score;
        in.close();
    }
    return score;
}

int main() {
    int windowDimentions[2] = { 800, 460 };
    InitWindow(windowDimentions[0], windowDimentions[1], "Dapper Dasher");
    InitAudioDevice();
    SetTargetFPS(60);

    Sound jumpSound = LoadSound("audio/jump.wav");
    Sound winSound = LoadSound("audio/win.wav");
    Sound loseSound = LoadSound("audio/lose.wav");
    Music music = LoadMusicStream("audio/music.mp3");
    PlayMusicStream(music);

    const int gravity = 1200, jump_velocity = 600;
    int velocity = 0, level = 1, score = 0, highScore = LoadHighScore();
    float scoreTimer = 0.0f, shieldTimer = 0.0f;
    bool is_in_air = false, collision = false, bossMode = false, isShielded = false;
    const float shieldDuration = 10.0f;

    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");

    float bgX = 0.0f, mgX = 0.0f, fgX = 0.0f;

    GameData scarfyData = {
        { 0, 0, (float)scarfy.width / 6, (float)scarfy.height },
        { windowDimentions[0] / 2.0f - scarfy.width / 12.0f, windowDimentions[1] - (float)scarfy.height },
        0, 1.0f / 12.0f, 0.0f
    };

    int baseNebulaVelocity = -300;
    static std::vector<GameData> nebulas;
    static float finishLine = 0.0f;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        const float dt = GetFrameTime();
        UpdateMusicStream(music);

        updateBackground(background, dt, 20, bgX);
        updateBackground(midground, dt, 40, mgX);
        updateBackground(foreground, dt, 80, fgX);

        if (nebulas.empty()) {
            float nebulaX = (float)windowDimentions[0];
            int numNebulas = level < 10 ? (5 + (level - 1) * 3) : 20;
            for (int i = 0; i < numNebulas; i++) {
                GameData neb = {
                    { 0, 0, (float)nebula.width / 8, (float)nebula.height / 8 },
                    { nebulaX, windowDimentions[1] - (float)nebula.height / 8 },
                    0, 1.0f / 16.0f, 0.0f
                };
                neb.tint = (level == 10) ? RED : WHITE;
                nebulas.push_back(neb);
                nebulaX = getNextNebulaX(nebulaX);
            }
            finishLine = nebulas.back().pos.x;
            bossMode = (level == 10);
        }

        if (!collision && finishLine > 0) {
            scoreTimer += dt;
            if (scoreTimer >= 0.5f) {
                score++;
                scoreTimer = 0.0f;
                if (score > highScore) {
                    highScore = score;
                    SaveHighScore(highScore);
                }
            }
        }

        scarfyData.runningTime += dt;
        if (isOnGround(scarfyData, windowDimentions)) {
            velocity = 0;
            is_in_air = false;
        } else velocity += gravity * dt;

        if (IsKeyPressed(KEY_SPACE) && !is_in_air) {
            velocity -= jump_velocity;
            is_in_air = true;
            PlaySound(jumpSound);
        }

        scarfyData.pos.y += velocity * dt;
        scarfyData = is_in_air ? scarfyData : updateGameData(scarfyData, dt, 5);
        if (is_in_air) scarfyData.frame = 1;

        // Activate shield with S key
        if (IsKeyPressed(KEY_S)) {
            isShielded = true;
            shieldTimer = 0.0f;
        }

        if (isShielded) {
            shieldTimer += dt;
            if (shieldTimer > shieldDuration) isShielded = false;
        }

        int speed = baseNebulaVelocity - (level > 10 ? 200 : level * 10);
        for (auto& neb : nebulas) {
            neb.runningTime += dt;
            neb = updateGameData(neb, dt, 7);
            neb.pos.x += speed * dt;
        }
        finishLine += speed * dt;

        for (auto& neb : nebulas) {
            Rectangle nebRec = { neb.pos.x + 50, neb.pos.y + 50, neb.rec.width - 100, neb.rec.height - 100 };
            Rectangle scarfyRec = { scarfyData.pos.x, scarfyData.pos.y, scarfyData.rec.width, scarfyData.rec.height };
            if (CheckCollisionRecs(nebRec, scarfyRec)) {
                if (!isShielded) {
                    collision = true;
                    PlaySound(loseSound);
                }
            }
        }

        if (collision) {
            DrawText("Game Over - Press R to Retry", 150, 200, 30, RED);
            if (IsKeyPressed(KEY_R)) {
                nebulas.clear();
                scarfyData.pos.y = windowDimentions[1] - scarfyData.rec.height;
                score = 0; velocity = 0; level = 1;
                isShielded = false;
                collision = false;
            }
        } else if (finishLine < scarfyData.pos.x) {
            PlaySound(winSound);
            level++;
            nebulas.clear();
        } else {
            for (auto& neb : nebulas)
                DrawTextureRec(nebula, neb.rec, neb.pos, neb.tint);

            Color flashColor = isShielded && ((int)(shieldTimer * 10) % 2 == 0) ? SKYBLUE : WHITE;
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, flashColor);
        }

        DrawText(TextFormat("Score: %d", score), 20, 20, 20, BLACK);
        DrawText(TextFormat("High Score: %d", highScore), 20, 50, 20, DARKGRAY);
        DrawText(TextFormat("Level: %d", level), 20, 80, 20, bossMode ? RED : BLUE);
        if (bossMode) DrawText("\xf0\x9f\x94\xa5 BOSS LEVEL \xf0\x9f\x94\xa5", 600, 20, 20, MAROON);
        if (isShielded) {
            float shieldRemaining = shieldDuration - shieldTimer;
            const char* shieldText = TextFormat("🛡 SHIELD ACTIVE - %.1fs", shieldRemaining);
            int textWidth = MeasureText(shieldText, 30);  // Adjust font size if needed
            DrawText(shieldText, windowDimentions[0]/2 - textWidth/2, windowDimentions[1]/2 - 100, 30, SKYBLUE);
        }
        
        

        EndDrawing();
    }

    UnloadSound(jumpSound);
    UnloadSound(winSound);
    UnloadSound(loseSound);
    UnloadMusicStream(music);
    CloseAudioDevice();

    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(foreground);
    UnloadTexture(midground);
    CloseWindow();
    return 0;
}
