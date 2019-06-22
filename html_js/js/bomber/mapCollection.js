// マップの生成関数(global)
var map_create = function(game, map_name) {

    // 指定のMAP定義を取得
    var collection = mapCollection[map_name];
    if (!collection) { return []; }

    // MAP1 背景
    var groundMap = new Map(collection.imageSize, collection.imageSize);   // size区切りで画像を読み込む (マップサイズはデータから動的判定)
    groundMap.image = game.assets[collection.imageFile];                   // 使用する画像パターンファイル
    groundMap.loadData(collection.groundBitmap);                           // パターン指定

    // MAP2 オブジェクト 衝突判定込み
    var objectMap = new Map(collection.imageSize, collection.imageSize);   // size区切りで画像を読み込む
    objectMap.image = game.assets[collection.imageFile];                   // 使用する画像パターンファイル
    objectMap.loadData(collection.objectBitmap);                           // パターン指定
    objectMap.collisionData = collection.objectCollision;                  // 衝突判定

    return [groundMap, objectMap];
};

// MAP定義コレクション(global)
var mapCollection = {

    // バトルエリア01 横20 x 縦(背景18 オブジェクト15) ブロック
    'battle_area_01' : {
        imageSize : 16,
        imageFile : 'image/map1.gif',
        groundBitmap : [
                [342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,85,],
                [342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,322,],
                [342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,104,],
                [342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,124,],
                [342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,85,],
                [342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,322,],
                [342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,184,],
                [342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,204,],
                [342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,204,],
                [342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,224,],
                [342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,322,],
                [342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,322,],
                [342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,322,],
                [342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,322,],
                [342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,322,],
                [ 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,],
                [ 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,],
                [ 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,],
            ],
        objectBitmap : [
                [321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321, -1,],
                [321, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,321, -1,],
                [321, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,321, -1,],
                [321, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,321, -1,],
                [321, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,321, -1,],
                [321, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,321, -1,],
                [321, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,321, -1,],
                [321, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,321, -1,],
                [321, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,321, -1,],
                [321, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,321, -1,],
                [321, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,321, -1,],
                [321, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,321, 421,],
                [321, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,341, -1,321, -1,],
                [321, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,321, 461,],
                [321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321, 481,],
            ],
        objectCollision : [
                [  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,],
                [  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,],
                [  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  1,],
                [  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,],
                [  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  1,],
                [  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,],
                [  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  1,],
                [  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,],
                [  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  1,],
                [  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,],
                [  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  1,],
                [  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,],
                [  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  1,],
                [  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,],
                [  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,],
            ],
    },
};