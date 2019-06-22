// プレイヤーのオブジェクト生成関数
var obj_player_create = function(game, ref_player_pos) {

    // このオブジェクトのサイズ
    const OBJ_SIZE = 32;

    // 左から8ピクセル、上から16ピクセルは透明部分
    const SKIP_WIDTH = 8;
    const SKIP_HEIGHT = 16;

    // オブジェクト生成 指定位置にセット
    var player = new Sprite(OBJ_SIZE, OBJ_SIZE);
    myfunc.set_xy(player, (ref_player_pos.x * CONST.BLOCK_SIZE) - SKIP_WIDTH, (ref_player_pos.y * CONST.BLOCK_SIZE) - SKIP_HEIGHT);

    // オブジェクトの画像イメージ領域
    var image = new Surface((OBJ_SIZE * 3), (OBJ_SIZE * 4));
    image.draw(game.assets['image/chara0.gif'], 0, 0, (OBJ_SIZE * 3), (OBJ_SIZE * 4), 0, 0, (OBJ_SIZE * 3), (OBJ_SIZE * 4));
    player.image = image;

    // enterframe処理
    player['MyActionFrame'] = (function() {

        // クロージャ変数
        var isMoving = false;    // 移動中半端フラグ
        var direction = 0;       // 方向 初期値:下(正面)
        var walk = 1;            // 現在歩き画像パターン 画像数(3パターン)と違い0-3(4パターン) 初期値:ニュートラル
        var image_frame = 0;     // セット中の画像フレーム 初期値はindex0

        // 移動方向と速度値
        var vx = 0;
        var vy = 0;
        var degree = CONST.BLOCK_SIZE / 4;

        // 処理
        return function(map, collision_list, keyinput) {

            // 表示画像フレームの指定 正面(0,1,2), 左(3,4,5)...
            // (右足出し, ニュートラル, 左足出し)
            var tmp_image_frame = direction * 3 + ((walk < 3) ? walk : 1);
            if (tmp_image_frame !== image_frame) {
                // 更新があれば置き換え
                image_frame = tmp_image_frame;
                this.frame = image_frame;
            }

            // 移動処理中フラグがONの場合
            if (isMoving) {

                // 相対座標で移動 入力開始時の値を用いる
                this.moveBy(vx, vy);

                // ゲームの現在フレーム数をチェック。3フレームに1回、歩き画像パターンを更新
                if (!(game.frame % 3)) {
                    // 歩き画像パターンのインクリメント 4なら1に戻る
                    walk ++;
                    walk %= 4;
                }

                // moveBy後、ブロック区切りに達するなら(キャラ画像の実位置を考慮)
                if ((vx && (this.x + SKIP_WIDTH) % CONST.BLOCK_SIZE === 0) || (vy && (this.y + SKIP_HEIGHT) % CONST.BLOCK_SIZE === 0)) {
                    // 移動中半端フラグを解除 入力を受け付ける
                    isMoving = false;
                    // 歩き画像ニュートラル 連続入力時を考慮しセットしない
                    //walk = 1;
                }

                return;
            }

            // 進行値パラメータをリセット
            vx = vy = 0;

            // 入力受付 上下左右のうち1つが有効 向きと進行値をセット
            switch (true) {
                case keyinput.status.left:       // 左
                    direction = 1;
                    vx = -(degree);
                    break;
                case keyinput.status.right:      // 右
                    direction = 2;
                    vx = degree;
                    break;
                case keyinput.status.up:         // 上
                    direction = 3;
                    vy = -(degree);
                    break;
                case keyinput.status.down:       // 下(正面)
                    direction = 0;
                    vy = degree;
                    break;
                default:                    // 入力なし
                    walk = 1;               // 歩き画像パターンをニュートラルに
                    return;
            }

            // 入力ありの場合 進もうとする先のxy座標の取得
            // プレイヤーの現在座標 + (左右進行あり ? (+or-のブロックサイズ) : 0)[進む先] + [プレイヤー実位置]
            var next_x = this.x + (vx ? (vx / Math.abs(vx)) * CONST.BLOCK_SIZE : 0) + SKIP_WIDTH;
            var next_y = this.y + (vy ? (vy / Math.abs(vy)) * CONST.BLOCK_SIZE : 0) + SKIP_HEIGHT;

            // game領域内かチェック
            if (!(next_x >= 0 && next_x < game.width && next_y >= 0 && next_y < game.height)) {
                return;
            }

            // MAP衝突判定をチェック
            if (map.hitTest(next_x, next_y) === true) {
                return;
            }

            // 既存オブジェクトが既に配置されていないかブロック座標を用いてチェック
            var next_block_pos = myfunc.pixel_to_block(next_x, next_y);
            if (myfunc.collision_check(collision_list, next_block_pos.block_x, next_block_pos.block_y) === true) {
                return;
            }

            // 移動処理中フラグON
            isMoving = true;

            // プレイヤー位置参照の更新
            ref_player_pos.x = next_block_pos.block_x;
            ref_player_pos.y = next_block_pos.block_y;

            // この関数(enterframeイベント)をもう１度呼び出し、移動処理
            // 連続入力時の移動がスムーズになる
            // arguments.callee : この関数自身 / call : Functionが共通で持つメソッド
            arguments.callee.call(this, map, collision_list, keyinput);
        };
    })();

    return player;
};
