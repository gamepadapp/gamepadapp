## 【概要】
	overview.pdf を参照  

---
## 【動作環境】
* [サーバ] RedHat系Linux  (CentOS 6/7系環境でテスト済み)
* [ブラウザ] Chrome / Safari / Firefox (WebSocketのVer-13以上に対応のもの)
* [iOS端末] iPhone / iPad / iPod Touch

---
### 手順１：Daemonが動作するLinuxサーバのアドレス、ポートをファイル内に指定
#### *【Webサーバ用ファイル】*
###### [書き換えファイル]
	html_js/js/websocket/netcontroller.js
###### [サーバのアドレス、ポートを指定]
	const SERVER_ADDRESS = "xxx.xxx.xxx.xxx";  
	const SERVER_PORT    = "xxxx";
#### *【Linuxサーバー用Daemon起動スクリプトファイル】*
###### [書き換えファイル]
	linux_daemon/script/netcontrollerd
###### [サーバのポートを指定]
	ACCEPTPORT="xxxx"
#### *【iOS用ファイル】*
###### [書き換えファイル]
	ios_xcode/NetController/CommonDefine.h
###### [サーバのアドレス、ポートを指定]
	#define SERVER_ADDRESS  @"xxx.xxx.xxx.xxx"  
	#define SERVER_PORT     xxxx

---
### 手順２−１：iOS側プログラムのビルド
	XCODEでプロジェクトを読み込みビルド

---
### 手順２−２：Linuxサーバー側プログラムのビルド、起動
###### OpenSSH系ライブラリを使用しているため、未導入の環境であればインストールが必要
	yum install openssl-devel
###### [ビルドを実行]
	linux_daemon/daemon/Release 内でmakeコマンドを実行  
	実行ファイル netcontrollerd　が生成される
##### *(Daemonプログラムのため、以下はroot権限で操作)*
###### [実行ファイルを配置]
	/usr/local/bin/  
	実行ファイルの配置ディレクトリは下記起動スクリプトに記載されているもの
###### [起動スクリプトファイルを配置]
	linux_daemon/script/netcontrollerd ファイルを下記ディレクトリに移動  
	/etc/init.d/  
	ファイル権限を他のスクリプトファイルに合わせる
###### [Daemon起動コマンドを実行]
	service netcontrollerd start  
	停止の場合はstopを指定
###### [以下の表示があれば成功]
	Starting netcontrollerd...

---
### 手順２−３：Webサーバにファイルを配置
###### [html_js/ 配下をWebサーバに配置]
* imageディレクトリ
* jsディレクトリ
* index.htmlファイル  
	配置するWebサーバはDaemonプログラムと別サーバでもよい

---
### 手順３：通信開始
###### [ブラウザでゲームHTMLを開く]
	HTMLを開くと自動でサーバ通信を開始
###### [iOS端末にてコントローラアプリを起動]
    中央のスイッチをON状態にするとサーバ通信を開始

---
## 【操作方法】
###### [iOSアプリで操作]
* 上下左右：移動
* Aボタン：爆弾を設置
* Bボタン：割り当てられた動作なし

	※PCブラウザで実行している場合、コントローラアプリを使用しない場合でもカーソルキー、Zキー、Xキーで上記操作が可能
