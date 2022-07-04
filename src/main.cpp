#include "core.h"
#include "TEMPORARY_NEEDS_CLEANUP/imgui/imgui_impl_glfw.h"
#include "TEMPORARY_NEEDS_CLEANUP/imgui/imgui_impl_opengl3.h"
#include "TEMPORARY_NEEDS_CLEANUP/glfw/glfw3.h"

ENUM(Direction)
{
	DIRECTION_RIGHT, // The order of these matters. Start at (1, 0) then rotate counter-clockwise (+y = up) by 45 degrees.
	DIRECTION_UP_RIGHT,
	DIRECTION_UP,
	DIRECTION_UP_LEFT,
	DIRECTION_LEFT,
	DIRECTION_DOWN_LEFT,
	DIRECTION_DOWN,
	DIRECTION_DOWN_RIGHT,
	DIRECTION_ENUM_COUNT
};

STRUCT(Player)
{
	Vector2 pos;
	Direction direction; // Determines which sprite to use.
	Texture *textures[DIRECTION_ENUM_COUNT];
};

Texture *background;
Image *collisionMap;
Font roboto;
Player player;

Direction DirectionFromVector(Vector2 v)
{
	float angleTaus = Wrap(Vector2Atan(v), 0, 2 * PI) / (2 * PI);
	float floatIndex = roundf(angleTaus * DIRECTION_ENUM_COUNT);
	int index = (int)floatIndex;
	if (index >= DIRECTION_ENUM_COUNT)
		index = 0;
	if (index < 0)
		index = DIRECTION_ENUM_COUNT - 1;
	return (Direction)index;
}

void Game(void)
{
	InitWindow(1280, 720, "Who Stole The Sun");
	SetTargetFPS(60);
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
	ImGui_ImplOpenGL3_Init("#version 130");
	roboto = LoadFontAscii("res/Roboto.ttf", 32);
	background = LoadTextureAndTrackChanges("res/background.png");
	collisionMap = LoadImageAndTrackChanges("res/collision-map.png");
	player.textures[DIRECTION_RIGHT]      = LoadTextureAndTrackChanges("res/player-right.png");
	player.textures[DIRECTION_UP_RIGHT]   = LoadTextureAndTrackChanges("res/player-up-right.png");
	player.textures[DIRECTION_UP]         = LoadTextureAndTrackChanges("res/player-up.png");
	player.textures[DIRECTION_UP_LEFT]    = LoadTextureAndTrackChanges("res/player-up-left.png");
	player.textures[DIRECTION_LEFT]       = LoadTextureAndTrackChanges("res/player-left.png");
	player.textures[DIRECTION_DOWN_LEFT]  = LoadTextureAndTrackChanges("res/player-down-left.png");
	player.textures[DIRECTION_DOWN]       = LoadTextureAndTrackChanges("res/player-down.png");
	player.textures[DIRECTION_DOWN_RIGHT] = LoadTextureAndTrackChanges("res/player-down-right.png");

	player.pos.x = 1280 / 2;
	player.pos.y = 720 / 2;
	player.direction = DIRECTION_DOWN;

	// In the isometric perspective, the y direction is squished down a little bit.
	float ySquish = sqrtf(2) * sinf(PI / 8);

	while (not WindowShouldClose())
	{
		HotReloadAllTrackedItems();
		TempReset(0);

		BeginDrawing();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			ClearBackground(BLACK); // @TODO: We might not need to clear if we always overwrite the whole screen.
			DrawTexture(*background, 0, 0, WHITE);

			float moveSpeed = 5;
			if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
				moveSpeed = 10;

			Vector2 move = { 0 };
			if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
				move.x -= 1;
			if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
				move.x += 1;
			if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
				move.y -= 1;
			if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
				move.y += 1;
			if (move.x != 0 || move.y != 0)
			{
				move = Vector2Normalize(move);
				Vector2 dirVector = move;
				dirVector.y *= -1;
				player.direction = DirectionFromVector(dirVector);
				Vector2 deltaPos = Vector2Scale(move, moveSpeed);
				deltaPos.y *= ySquish;
				Vector2 newPos = Vector2Add(player.pos, deltaPos);
				Vector2 feetPos = newPos;
				feetPos.y += 0.5f * player.textures[player.direction]->height;

				int footX = (int)roundf(feetPos.x);
				int footY = (int)roundf(feetPos.y);
				footX = ClampInt(footX, 0, collisionMap->width - 1);
				footY = ClampInt(footY, 0, collisionMap->height - 1);
				Color collision = GetImageColor(*collisionMap, footX, footY);

				LogInfo("%d", collision.r);
				if (collision.r >= 128)
					player.pos = newPos;
			}

			Vector2 playerSize = { 50, 90 };
			DrawTextureCentered(*player.textures[player.direction], player.pos, WHITE);

			if (IsKeyDown(KEY_GRAVE))
				ImGui::ShowDemoWindow();
		}			
		rlDrawRenderBatchActive();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		EndDrawing();
	}
}

void GameLoopOneIteration(void)
{

}
