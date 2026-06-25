#include "raylib.h"
#include <fstream>
#include <string>
#include <iostream>
#include <limits>

//Amadeus highscore 14

void drawAxe();
void drawCircle();
void DrawGameOver();
void ResetGame();
void spawn_collect();
void SpawnCollectablePosition();
void DrawHighscore();
void LoadHighScore();
void SaveHighScore(int highScore, const std::string& playerName);

Sound GameOver_sfx;
Sound Collect_sfx;

int HighScore = 0;
const char* HighScoreFile = "highscore.txt";
std::string HighScorePlayerName = "Player";
std::string HighScoreNameInput;
bool isEnteringHighScoreName = false;
const int maxNameLength = 12;

int axeSpeed;
int yPos = 0;
int score = 0;

int width = 800;
int height = 600;
int targetFPS = 60;

int circleX = 0;
int circleY = 0;
int circleSpeed = 25;

int screenWidth;
int screenHeight;

int collectX = 0;
int collectY = 0;
int collectRadius = 15;

bool isGameOver = false;

int main()
{
	const char* title = "The Greatest Game";

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(width, height, title);
	SetTargetFPS(targetFPS);
	InitAudioDevice();
	
	GameOver_sfx = LoadSound("Sounds/GameOver_oaah.wav");
	SetSoundVolume(GameOver_sfx, 0.5f);

	Collect_sfx = LoadSound("Sounds/PickUp.wav");
	SetSoundVolume(Collect_sfx, 0.5f);

	ResetGame();
	

	while (!WindowShouldClose())
	{
		screenWidth = GetScreenWidth();
		screenHeight = GetScreenHeight();

		BeginDrawing();
		ClearBackground(BLACK);

		if (isGameOver)
		{
			DrawGameOver();

			
		}
		else
		{
			drawAxe();
			drawCircle();
			spawn_collect();

			DrawText(TextFormat("Score: %i", score), 20, 20, 30, WHITE);
			DrawHighscore();
		}

		EndDrawing();
	}
	
	UnloadSound(GameOver_sfx);
	UnloadSound(Collect_sfx);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}

void drawAxe()
{
	int axeLength = 50;
	yPos += axeSpeed;

	DrawRectangle(screenWidth / 2, yPos, axeLength, axeLength, RED);

	if (yPos > screenHeight - axeLength || yPos < 0)
	{
		axeSpeed = -axeSpeed;
	}
}

void drawCircle()
{
	int circleRadius = 25;
	int axeX = screenWidth / 2;
	int axeY = yPos;
	int axeSize = 50;

	if (IsKeyDown(KEY_RIGHT))
	{
		circleX += circleSpeed;
	}
	if (IsKeyDown(KEY_LEFT))
	{
		circleX -= circleSpeed;
	}
	if (IsKeyDown(KEY_UP))
	{
		circleY -= circleSpeed;
	}
	if (IsKeyDown(KEY_DOWN))
	{
		circleY += circleSpeed;
	}

	if (circleX < circleRadius) circleX = circleRadius;
	if (circleX > screenWidth - circleRadius) circleX = screenWidth - circleRadius;
	if (circleY < circleRadius) circleY = circleRadius;
	if (circleY > screenHeight - circleRadius) circleY = screenHeight - circleRadius;

	bool collisionWithAxe =
		axeY + axeSize >= circleY - circleRadius &&
		axeY <= circleY + circleRadius &&
		axeX + axeSize >= circleX - circleRadius &&
		axeX <= circleX + circleRadius;

	if (collisionWithAxe)
	{
		PlaySound(GameOver_sfx);
		isGameOver = true;
		return;
	}

	DrawCircle(circleX, circleY, circleRadius, RED);
}

void DrawGameOver()
{
	DrawText("Game Over!", screenWidth / 2 - 80, screenHeight / 2 - 30, 30, RED);

	if (score > HighScore || isEnteringHighScoreName)
	{
		if (!isEnteringHighScoreName)
		{
			isEnteringHighScoreName = true;
			HighScoreNameInput.clear();
		}

		int key = GetCharPressed();
		while (key > 0)
		{
			if (key >= 32 && key <= 125 && HighScoreNameInput.length() < maxNameLength)
			{
				HighScoreNameInput.push_back(static_cast<char>(key));
			}

			key = GetCharPressed();
		}

		if (IsKeyPressed(KEY_BACKSPACE) && !HighScoreNameInput.empty())
		{
			HighScoreNameInput.pop_back();
		}

		DrawText(TextFormat("New High Score: %i", score), screenWidth / 2 - 110, screenHeight / 2 + 10, 20, GREEN);
		DrawText("Enter your name:", screenWidth / 2 - 110, screenHeight / 2 + 45, 20, WHITE);
		DrawRectangleLines(screenWidth / 2 - 110, screenHeight / 2 + 75, 220, 35, WHITE);
		DrawText(HighScoreNameInput.c_str(), screenWidth / 2 - 100, screenHeight / 2 + 82, 20, YELLOW);
		DrawText("Press ENTER to save", screenWidth / 2 - 110, screenHeight / 2 + 120, 18, WHITE);

		if (IsKeyPressed(KEY_ENTER))
		{
			HighScore = score;
			HighScorePlayerName = HighScoreNameInput.empty() ? "Player" : HighScoreNameInput;
			SaveHighScore(HighScore, HighScorePlayerName);
			isEnteringHighScoreName = false;
			ResetGame();
		}

		return;
	}

	DrawText(TextFormat("High Score: %i", HighScore), screenWidth / 2 - 80, screenHeight / 2 + 20, 20, WHITE);
	DrawText("Press any key to play again", screenWidth / 2 - 110, screenHeight / 2 + 55, 15, WHITE);

	if (GetKeyPressed() != 0)
	{
		ResetGame();
	}
}

void ResetGame()
{
	screenWidth = GetScreenWidth();
	screenHeight = GetScreenHeight();

	yPos = screenHeight / 2;
	circleX = 100;
	circleY = screenHeight / 2;
	axeSpeed = 10;
	score = 0;
	isGameOver = false;
	isEnteringHighScoreName = false;
	HighScoreNameInput.clear();

	SpawnCollectablePosition();
	LoadHighScore();
	
}

void SpawnCollectablePosition()
{
	int axeX = screenWidth / 2;
	int axeSize = 50;
	int paddingFromAxe = 30;
	int minX;
	int maxX;

	if (circleX < axeX) // If the circle is to the left of the axe
	{
		minX = axeX + axeSize + paddingFromAxe + collectRadius; // Ensure collectable is to the right of the axe
		maxX = screenWidth - collectRadius;		// Ensure collectable is within the screen bounds
	}
	else
	{
		minX = collectRadius; // Ensure collectable is within the screen bounds
		maxX = axeX - paddingFromAxe - collectRadius; // Ensure collectable is to the left of the axe	
	}

	if (minX > maxX) // If the circle is too close to the axe, adjust the range to avoid overlap
	{
		minX = collectRadius;		
		maxX = screenWidth - collectRadius;
	}

	collectX = GetRandomValue(minX, maxX);
	collectY = GetRandomValue(collectRadius, screenHeight - collectRadius);
}

void spawn_collect()
{
	int circleRadius = 25;
	int dx = circleX - collectX;
	int dy = circleY - collectY;
	int totalRadius = circleRadius + collectRadius;

	bool collected = (dx * dx) + (dy * dy) <= (totalRadius * totalRadius);

	if (collected)
	{
		score++;
		PlaySound(Collect_sfx);

		if (axeSpeed >= 0)
		{
			axeSpeed += 5;
		}
		else
		{
			axeSpeed -= 5;
		}

		SpawnCollectablePosition();
	}

	DrawCircle(collectX, collectY, collectRadius, GREEN);

	
}

void DrawHighscore()
{
	DrawText(TextFormat("High Score: %i", HighScore), 20, 60, 30, YELLOW);
	DrawText(TextFormat("Player: %s", HighScorePlayerName.c_str()), 20, 95, 20, YELLOW);
}

void LoadHighScore()
{

	std::ifstream file(HighScoreFile);
	if (file.is_open())
	{
		file >> HighScore;
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::getline(file, HighScorePlayerName);

		if (HighScorePlayerName.empty())
		{
			HighScorePlayerName = "Player";
		}

		file.close();
	}
	else
	{
		HighScore = 0;
		HighScorePlayerName = "Player";
	}
}

void SaveHighScore(int highScore, const std::string& playerName)
{
	std::ofstream file(HighScoreFile);
	if (file.is_open())
	{
		file << highScore << std::endl;
		file << playerName << std::endl;
		file.close();
	}
	else
	{
		std::cerr << "Unable to open highscore.txt for writing." << std::endl;
	}
}