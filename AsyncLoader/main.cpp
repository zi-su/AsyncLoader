#include "AsyncLoadRequestManager.h"

#include <chrono>
#include <thread>

const float FPS = 60.0f;
int main() {
	auto loader = AsyncLoadRequestManager::getInstance();

	
	{//メインループ
		auto frameStart = std::chrono::high_resolution_clock::now();
		//色々更新処理中ロードリクエストを積む
		for(int i = 0 ; i < 100 ; i++){

			//読み込むファイル
			struct _stat buf;
			int fd=0;
			_stat("test.jpg", &buf);
			void* buff = nullptr;	//アロケータからアロケートしてきたバッファ

			loader->PushRequest(RequestInfo(buff, "test.jpg", buf.st_size));
		}
		
		//非同期ロードスタート
		auto loadstart = std::chrono::system_clock::now();
		if (loader->IsRequested()) {
			loader->ThreadStart();
		}

		//色々更新処理中非同期ロード待ち.NowLoading描画積みとか。待機とか。
		while (loader->IsLoading()) {
			//printf("Waiting...\n");

			//ユーザー入力によるキャンセル処理
			{
			}
		}
		auto loadend = std::chrono::system_clock::now();
		auto elaps = std::chrono::duration_cast<std::chrono::microseconds>(loadend - loadstart);
		printf("load elaps %lld ms", elaps.count());
		//メイン処理

		//描画処理

		//タイマー待ち計算
		auto frameEnd = std::chrono::high_resolution_clock::now();
		auto frameElaps = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart);
		auto remain = std::chrono::microseconds((long long)(1000000 * (1 / FPS))) - frameElaps;
		auto wait = std::chrono::high_resolution_clock::now() + remain;
		while (std::chrono::high_resolution_clock::now() < wait) {
		}
		auto frameFinish = std::chrono::high_resolution_clock::now();
		auto frameElapsFinish = std::chrono::duration_cast<std::chrono::microseconds>(frameFinish - frameStart);
		printf("FrameElaps %lld \n", frameElapsFinish.count());
	}

	//メインループ終了
	loader->Finish();
	return 0;
}