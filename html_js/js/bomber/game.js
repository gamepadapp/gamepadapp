// enchant JS
enchant();

// 定数として扱う
var CONST = {
    BLOCK_SIZE : 16,
};

// 共通関数
var myfunc = {
    set_xy : function(obj, x, y) {
        obj.x = x;
        obj.y = y;
    },
    pixel_to_block : function(x, y) {
        return {
            block_x : x / CONST.BLOCK_SIZE,
            block_y : y / CONST.BLOCK_SIZE,
        };
    },
    block_to_pixel : function(x, y) {
        return {
            pixel_x : x * CONST.BLOCK_SIZE,
            pixel_y : y * CONST.BLOCK_SIZE,
        };
    },
    collision_set : function(collision_list, block_x, block_y) {
        var keyname = '' + block_x + ':' + block_y;
        collision_list[keyname] = 1;
    },
    collision_check : function(collision_list, block_x, block_y) {
        var keyname = '' + block_x + ':' + block_y;
        return collision_list[keyname] ? true : false;
    },
    collision_unset : function(collision_list, block_x, block_y) {
        var keyname = '' + block_x + ':' + block_y;
        collision_list[keyname] = 0;
    },
    deleteFromArray : function(list, obj) {
        for (var i = 0, len = list.length; i < len; i++) {
           if (list[i] === obj) {
               list.splice(i, 1);
               return;
           }
        }
    },
    addChildAll : function() {        // 連続addChild関数 applyで用いる
        for (var i = 0, len = arguments.length; i < len; i++) {
            this.addChild(arguments[i]);
        }
    },
};

// ページ初期処理に登録
window.onload = function() {

    // ステージサイズを指定しgame作成
    var game = new Core(CONST.BLOCK_SIZE * 20, CONST.BLOCK_SIZE * 18);
    game.fps = 15;
    game.preload('image/map1.gif', 'image/chara0.gif', 'image/icon0.png', 'image/effect0.gif');

    // 操作 キー入力
    game.keybind(90, 'a');    // zキーをAボタンとして使えるようにする
    game.keybind(88, 'b');    // xキーをBボタンとして使えるようにする

    // 操作 netPad
    webSocketStart();         // WebSocket接続スタート

    // 実処理
    game.onload = function() {

        // プレイヤー位置参照 初期位置は中央
        var ref_player_pos = { x:9, y:7 };
        // プレイヤーオブジェクト生成
        var player = obj_player_create(game, ref_player_pos);

        // ステージ用のGroup
        var stageGroup = new Group();
        // オブジェクト用のGroup
        var objectGroup = new Group();

        // MAP battle_area_01 を取得しステージにセット
        var mapObjAry = map_create(game, 'battle_area_01');
        myfunc.addChildAll.apply(stageGroup, mapObjAry);

        // キー入力オブジェクト
        var keyinput = obj_keyinput_create(game, objectGroup);

        // ステージ,オブジェクト,プレイヤーの順にrootSceneに追加
        myfunc.addChildAll.apply(game.rootScene, [stageGroup, objectGroup, player]);

        // 爆弾オブジェクトのリスト
        var bombs_array = [];
        var explosion_array = [];
        // 障害物リスト
        var collision_list = {};

        // 毎フレームのメインループ処理を登録
        game.addEventListener('enterframe', function(e) {
            var bombs_del_array = [];
            var explosion_del_array = [];

            // キー入力
            keyinput.MyActionFrame();

            // プレイヤー(ボタン移動)
            player.MyActionFrame(mapObjAry[1], collision_list, keyinput);

            // 爆弾
            for (var i = 0, len = bombs_array.length; i < len; i++) {
               var active = bombs_array[i].MyActionFrame(game, mapObjAry[1], objectGroup, explosion_array);
               if (!active) {
                   // 削除対象
                   bombs_del_array.push(bombs_array[i]);
               }
            }
            // 爆発
            for (var i = 0, len = explosion_array.length; i < len; i++) {
               var active = explosion_array[i].MyActionFrame();
               if (!active) {
                   // 削除対象
                   explosion_del_array.push(explosion_array[i])
               }
            }

            // 爆弾の削除
            for (var i = 0, len = bombs_del_array.length; i < len; i++) {
                myfunc.collision_unset(collision_list, bombs_del_array[i].pos_x, bombs_del_array[i].pos_y);
                objectGroup.removeChild(bombs_del_array[i]);
                myfunc.deleteFromArray(bombs_array, bombs_del_array[i]);
            }
            // 爆発の削除
            for (var i = 0, len = explosion_del_array.length; i < len; i++) {
                objectGroup.removeChild(explosion_del_array[i]);
                myfunc.deleteFromArray(explosion_array, explosion_del_array[i]);
            }

            // Aボタン処理
            if (keyinput.status.a) {
                // 障害物リストの確認
                if (myfunc.collision_check(collision_list, ref_player_pos.x, ref_player_pos.y)) {
                    return;
                }

                // 爆弾生成しSceneにセット
                var bomb = new Bomb(game, ref_player_pos.x, ref_player_pos.y);
                objectGroup.addChild(bomb);
                // リストに追加
                bombs_array.push(bomb);

                // 障害物リストにセット
                myfunc.collision_set(collision_list, ref_player_pos.x, ref_player_pos.y);
            }
        });
    };

    // 処理開始
    game.start();
};
