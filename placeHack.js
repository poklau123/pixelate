var matrix = []; //timg.json
var startX = 0,
    startY = 0;

function drawPixel() {
    var stack = [];
    for (j = 0; j < matrix.length; j++) {
        for (i = 0; i < matrix[j].length; i++) {
            var pointX = startX + i,
                pointY = startY + j;
            var pixelData = place.canvasController.ctx.getImageData(pointX, pointY, 1, 1);
            var hexColor = rgbToHex(pixelData.data[0], pixelData.data[1], pixelData.data[2]).toUpperCase();
            var index = place.DEFAULT_COLOURS.indexOf(hexColor);
            if (matrix[j][i] != index) {
                stack.push({i:i,j:j});
            }
        }
    }
    if(stack.length > 0){
        var select = stack[Math.floor(Math.random()*stack.length)];
        var i = select.i,
            j = select.j;
        var pointX = startX + i,
            pointY = startY + j;
        var color = matrix[j][i];
        place.selectedColour = null;
        place.canvasClicked(pointX, pointY);
        place.selectedColour = color;
        place.canvasClicked(pointX, pointY);
        console.log('pixel point('+pointX+','+pointY+') with color index:'+color);
        return true;
    }else{
        return false;
    }
}

/*RGB颜色转换为16进制*/
function rgbToHex(r, g, b) {
    var hex = "#";
    hex += ("0" + Number(r).toString(16)).slice(-2);
    hex += ("0" + Number(g).toString(16)).slice(-2);
    hex += ("0" + Number(b).toString(16)).slice(-2);
    return hex;
}
//监听unlockTime的变化，如果为null,则可以画像素
Object.defineProperty(place, "unlockTime", {
    set: function (value) {
        unlockTime = value;
        if(value == null){
            try{
                if(!drawPixel()){
                    setTimeout(function(){
                        place.unlockTime = null;
                    },5000);
                }
            }catch(e){
                console.warn(e);
                setTimeout(function(){
                    place.unlockTime = null;
                },5000);
            }
        }
    },
    get: function () {
        return unlockTime;
    }
});
//防止alert中断
window.alert = function(v){
console.warn(v);
}
// 错误时比如429后隔一段时间重试
$.ajaxSetup({
    error:function(){
        setTimeout(function(){
            place.unlockTime = null;
        },5000);
    }
});

place.canvasClicked(startX,startY);
place.unlockTime = null;
