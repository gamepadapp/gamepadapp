// 爆発オブジェクト
var Explosion = Class.create(Sprite, {
    OBJ_SIZE:16,

    initialize:function(game, pos_x, pos_y) {
        Sprite.call(this, this.OBJ_SIZE, this.OBJ_SIZE);


        this.count_num = 0;           // 発生からの経過カウント
        this.image_frame_idx = 0;     // セット中の画像フレームindex 初期値は0


        // 指定位置にセット
        myfunc.set_xy(this, (pos_x * CONST.BLOCK_SIZE), (pos_y * CONST.BLOCK_SIZE));

        // オブジェクトの画像イメージ領域
        var obj_image = new Surface((this.OBJ_SIZE * 5), (this.OBJ_SIZE * 1));
        obj_image.draw(game.assets['image/effect0.gif'], 0, 0, (this.OBJ_SIZE * 5), (this.OBJ_SIZE * 1), 0, 0, (this.OBJ_SIZE * 5), (this.OBJ_SIZE * 1));
        this.image = obj_image;
    },

    MyActionFrame:function() {

        // 経過カウントにより自オブジェクトを消去
        if (this.count_num >= 9) {
            return false;
        }

        // 表示画像フレームの計算
        var tmp_idx = (this.count_num / 2);

        // フレーム更新
        if (this.image_frame_idx !== tmp_idx) {
            this.frame           = tmp_idx;
            this.image_frame_idx = tmp_idx;
        }

        // 経過カウントをインクリメント
        this.count_num ++;

        return true;
    }
});
