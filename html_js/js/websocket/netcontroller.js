/* WebSocket相手サーバ,独自プロトコル情報 */
const SERVER_ADDRESS = "xxx.xxx.xxx.xxx";
const SERVER_PORT    = "xxxx";
const WS_PROTOCOL = "netcontroller";

/* WebSocket */
var ws;
/* パッド状態オブジェクト */
var netPad = {};

/* WebSocketスタート */
function webSocketStart() {
    if ( isWebSocketActive() ) {
        return;
    }

    // WebSocket通信開始 ハンドシェイク
    var ws_url = "ws://" + SERVER_ADDRESS + ":" + SERVER_PORT;
    console.log("[WebSocket] connect :" + ws_url + ", protocol :" + WS_PROTOCOL);
    ws = new WebSocket(ws_url, WS_PROTOCOL);

    // イベント登録
    registerWsEvents();

    // パッド状態初期化
    netPadDefault();
}

/* WebSocketイベント登録 */
function registerWsEvents() {

    // 通信開始イベント登録
    ws.onopen = function(event) {
        console.log("[WebSocket] Open");
    }

    // 通信終了イベント登録
    ws.onclose = function(evnet) {
        console.log("[WebSocket] Close");
        ws = undefined;
    }
  
    // エラー発生イベント登録
    ws.onerror =function(event) {
        console.log("[WebSocket] Error");
        alert("WebSocket Error");
    }

    // データ受信イベント登録
    ws.onmessage = function(event) {
        // パッド状態更新
        netPadUpdate(event.data);
    }
}

/* WebSocketアクティブチェック */
function isWebSocketActive() {
    return (ws !== undefined);
}

const NUM_0_CODE = 48;

/* パッド状態初期化 */
function netPadDefault() {
    netPad.A = 0
    netPad.B = 0;
    netPad.U = 0;
    netPad.D = 0;
    netPad.L = 0;
    netPad.R = 0;
}

/* パッド状態更新 */
function netPadUpdate(infoStr) {
    if (infoStr.length !== 12) {
        return;
    }
    netPad.A = (infoStr.charCodeAt(1)  - NUM_0_CODE);
    netPad.B = (infoStr.charCodeAt(3)  - NUM_0_CODE);
    netPad.U = (infoStr.charCodeAt(5)  - NUM_0_CODE);
    netPad.D = (infoStr.charCodeAt(7)  - NUM_0_CODE);
    netPad.L = (infoStr.charCodeAt(9)  - NUM_0_CODE);
    netPad.R = (infoStr.charCodeAt(11) - NUM_0_CODE);
}

/* ページunload時イベント登録 */
window.onunload = function(event) {
    // WebSocket close通知
    if ( isWebSocketActive() ) {
        ws.close(1000, "unload event");
        ws = undefined;
    }
}
