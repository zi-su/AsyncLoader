#include "AsyncLoadRequestManager.h"

#include <chrono>
#include <thread>

int main() {
	AsyncLoadRequestManager loader;

	
	{//メインループ
		
		//色々更新処理中ロードリクエストを積む
		for(int i = 0 ; i < 100 ; i++){

			//読み込むファイル
			struct _stat buf;
			int fd=0;
			_stat("test.jpg", &buf);
			void* buff = nullptr;	//アロケータからアロケートしてきたバッファ

			loader.PushRequest(RequestInfo(buff, "test.jpg", buf.st_size));
		}
		
		//非同期ロードスタート
		auto loadstart = std::chrono::system_clock::now();
		if (loader.IsRequested()) {
			loader.ThreadStart();
		}

		//色々更新処理中非同期ロード待ち.NowLoading描画積みとか。待機とか。
		while (loader.IsLoading()) {
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
	}

	//メインループ終了
	loader.Finish();
	return 0;
}