// 爆弾オブジェクト
var Bomb = Class.create(Sprite, {
    OBJ_SIZE:16,

    initialize:function(game, pos_x, pos_y) {
        Sprite.call(this, this.OBJ_SIZE, this.OBJ_SIZE);


        // 参照用位置情報
        this.pos_x = pos_x;
        this.pos_y = pos_y;

        this.count_num = 0;           // 発生からの経過カウント
        this.image_frame_idx = 0;     // セット中の画像フレームindex 初期値は0
        this.is_explosion = false;    // 爆発中
        this.next_image = null;       // 切り替え用イメージ


        // 指定位置にセット
        myfunc.set_xy(this, (pos_x * CONST.BLOCK_SIZE), (pos_y * CONST.BLOCK_SIZE));

        // オブジェクトの画像イメージ領域
        var obj_image = new Surface((this.OBJ_SIZE * 2), (this.OBJ_SIZE * 1));
        obj_image.draw(game.assets['image/icon0.png'], (this.OBJ_SIZE * 8), (this.OBJ_SIZE * 1), (this.OBJ_SIZE * 2), (this.OBJ_SIZE * 1), 0, 0, (this.OBJ_SIZE * 2), (this.OBJ_SIZE * 1));
        this.image = obj_image;

        // 爆発時に用いる画像イメージ領域
        var explosion_image = new Surface((this.OBJ_SIZE * 5), (this.OBJ_SIZE * 1));
        explosion_image.draw(game.assets['image/effect0.gif'], 0, 0, (this.OBJ_SIZE * 5), (this.OBJ_SIZE * 1), 0, 0, (this.OBJ_SIZE * 5), (this.OBJ_SIZE * 1));
        this.next_image = explosion_image;
    },

    MyActionFrame:function(game, map, objectGroup, explosion_array) {

        // 経過カウントにより自オブジェクトを消去
        if (this.is_explosion && this.count_num >= 9) {
            return false;
        }

        // 経過カウントにより爆発
        if (!this.is_explosion && this.count_num === 37) {

            this.is_explosion = true;
            this.count_num = 0;
            this.image = this.next_image;
            this.image_frame_idx = 0;

            // 周囲の爆発を生成
            if (map.hitTest((this.pos_x - 1) * CONST.BLOCK_SIZE, this.pos_y * CONST.BLOCK_SIZE) === false) {
                this.aroundExplosion(game, this.pos_x - 1, this.pos_y, objectGroup, explosion_array);
            }
            if (map.hitTest((this.pos_x + 1) * CONST.BLOCK_SIZE, this.pos_y * CONST.BLOCK_SIZE) === false) {
                this.aroundExplosion(game, this.pos_x + 1, this.pos_y, objectGroup, explosion_array);
            }
            if (map.hitTest(this.pos_x * CONST.BLOCK_SIZE, (this.pos_y - 1) * CONST.BLOCK_SIZE) === false) {
                this.aroundExplosion(game, this.pos_x, this.pos_y - 1, objectGroup, explosion_array);
            }
            if (map.hitTest(this.pos_x * CONST.BLOCK_SIZE, (this.pos_y + 1) * CONST.BLOCK_SIZE) === false) {
                this.aroundExplosion(game, this.pos_x, this.pos_y + 1, objectGroup, explosion_array);
            }
        }

        // 表示画像フレームの計算
        if (!this.is_explosion) {
            var tmp_idx = (this.count_num % 18 < 9) ? 0 : 1;
        } else {
            var tmp_idx = (this.count_num / 2);
        }

        // フレーム更新
        if (this.image_frame_idx !== tmp_idx) {
            this.frame           = tmp_idx;
            this.image_frame_idx = tmp_idx;
        }

        // 経過カウントをインクリメント
        this.count_num ++;

        return true;
    },

    // private 周囲の爆発を生成
    aroundExplosion:function(game, obj_x, obj_y, objectGroup, explosion_array) {
        // 爆発生成しSceneにセット
        var explosion = new Explosion(game, obj_x, obj_y);
        objectGroup.addChild(explosion);
        // リストに追加
        explosion_array.push(explosion);
    },
});
