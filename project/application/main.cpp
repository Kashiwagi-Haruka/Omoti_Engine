#include "Game/Game.h"
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ゲームの枠生成
	std::unique_ptr<FrameWork> game = std::make_unique<Game>();

	//ゲーム
	game->Run();

	return 0;
}