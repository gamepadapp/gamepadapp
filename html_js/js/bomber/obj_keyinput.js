// キー入力のオブジェクト生成関数
var obj_keyinput_create = function(game, objectGroup) {

    // ボタンアイコンのサイズ
    const ICON_SIZE = 16;

    var obj = {};

    // 方向ボタンの画像イメージ
    var cross_image = new Surface(ICON_SIZE, ICON_SIZE);
    cross_image.draw(game.assets['image/icon0.png'], (ICON_SIZE * 11), (ICON_SIZE * 2), ICON_SIZE, ICON_SIZE, 0, 0, ICON_SIZE, ICON_SIZE);

    // 方向ボタン 上
    var cross_u = new Sprite(ICON_SIZE, ICON_SIZE);
    myfunc.set_xy(cross_u, (7 * CONST.BLOCK_SIZE), (15 * CONST.BLOCK_SIZE));
    cross_u.image = cross_image;
    cross_u.visible = false;
    objectGroup.addChild(cross_u);
    obj['cross_u'] = cross_u;

    // 方向ボタン 下
    var cross_d = new Sprite(ICON_SIZE, ICON_SIZE);
    myfunc.set_xy(cross_d, (7 * CONST.BLOCK_SIZE), (17 * CONST.BLOCK_SIZE));
    cross_d.image = cross_image;
    cross_d.rotation = 180;
    cross_d.visible = false;
    objectGroup.addChild(cross_d);
    obj['cross_d'] = cross_d;

    // 方向ボタン 左
    var cross_l = new Sprite(ICON_SIZE, ICON_SIZE);
    myfunc.set_xy(cross_l, (6 * CONST.BLOCK_SIZE), (16 * CONST.BLOCK_SIZE));
    cross_l.image = cross_image;
    cross_l.rotation = 270;
    cross_l.visible = false;
    objectGroup.addChild(cross_l);
    obj['cross_l'] = cross_l;

    // 方向ボタン 右
    var cross_r = new Sprite(ICON_SIZE, ICON_SIZE);
    myfunc.set_xy(cross_r, (8 * CONST.BLOCK_SIZE), (16 * CONST.BLOCK_SIZE));
    cross_r.image = cross_image;
    cross_r.rotation = 90;
    cross_r.visible = false;
    objectGroup.addChild(cross_r);
    obj['cross_r'] = cross_r;

    // ABボタンの画像イメージ
    var abutton_image = new Surface(ICON_SIZE, ICON_SIZE);
    abutton_image.draw(game.assets['image/icon0.png'], (ICON_SIZE * 3), (ICON_SIZE * 1), ICON_SIZE, ICON_SIZE, 0, 0, ICON_SIZE, ICON_SIZE);
    var bbutton_image = new Surface(ICON_SIZE, ICON_SIZE);
    bbutton_image.draw(game.assets['image/icon0.png'], (ICON_SIZE * 4), (ICON_SIZE * 1), ICON_SIZE, ICON_SIZE, 0, 0, ICON_SIZE, ICON_SIZE);

    // Aボタン
    var a_button = new Sprite(ICON_SIZE, ICON_SIZE);
    myfunc.set_xy(a_button, (12 * CONST.BLOCK_SIZE), (16 * CONST.BLOCK_SIZE));
    a_button.image = abutton_image;
    a_button.visible = false;
    objectGroup.addChild(a_button);
    obj['a_button'] = a_button;

    // Bボタン
    var b_button = new Sprite(ICON_SIZE, ICON_SIZE);
    myfunc.set_xy(b_button, (11 * CONST.BLOCK_SIZE), (16 * CONST.BLOCK_SIZE));
    b_button.image = bbutton_image;
    b_button.visible = false;
    objectGroup.addChild(b_button);
    obj['b_button'] = b_button;

    // 入力状態の参照
    obj['status'] = {
        up    : false,
        down  : false,
        left  : false,
        right : false,
        a     : false,
        b     : false
    };

    // enterframe処理
    obj['MyActionFrame'] = function() {

        var ws_active = isWebSocketActive();

        var up    = false;
        var down  = false;
        var left  = false;
        var right = false;
        var abtn  = false;
        var bbtn  = false;

        if (ws_active && netPad.U === 1) {
            up = true;
        }
        else if (game.input.up) {
            up = true;
        }

        if (ws_active && netPad.D === 1) {
            down = true;
        }
        else if (game.input.down) {
            down = true;
        }

        if (ws_active && netPad.L === 1) {
            left = true;
        }
        else if (game.input.left) {
            left = true;
        }

        if (ws_active && netPad.R === 1) {
            right = true;
        }
        else if (game.input.right) {
            right = true;
        }

        if (ws_active && netPad.A === 1) {
            abtn = true;
        }
        else if (game.input.a) {
            abtn = true;
        }

        if (ws_active && netPad.B === 1) {
            bbtn = true;
        }
        else if (game.input.b) {
            bbtn = true;
        }

        // 入力状態
        this.status = {
            up    : up,
            down  : down,
            left  : left,
            right : right,
            a     : abtn,
            b     : bbtn
        };

        // アイコン表示
        this.cross_u.visible  = up;
        this.cross_d.visible  = down;
        this.cross_l.visible  = left;
        this.cross_r.visible = right;
        this.a_button.visible = abtn;
        this.b_button.visible = bbtn;
    };

    return obj;
};
