# AsyncLoader
FirstCommit

# 使い方
1. AsyncLoadRequetManagerインスタンスを作成  
2. PushRequestでロードリクエスト情報を追加  
3. IsRequested()でリクエストが溜まっているかを調べてThreadStartで読み込み開始  
3.1 内部でCPUコア数分のロードスレッドが作成される  
3.2 ロードリクエストをスレッドに分割してロード  
3.3 ロードリクエスト情報に指定したバッファに読み込み結果を格納して終了  
4. アプリケーション終了時にFinish()呼び出しでjoin