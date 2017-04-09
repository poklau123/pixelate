var matrix = [];
var startX = 0,
    startY = 0;

function drawPixel() {
    for (j = 0; j < matrix.length; j++) {
        for (i = 0; i < matrix[j].length; i++) {
            var pointX = startX + i,
                pointY = startY + j;
            var pixelData = place.canvasController.ctx.getImageData(pointX, pointY, 1, 1);
            var hexColor = rgbToHex(pixelData.data[0], pixelData.data[1], pixelData.data[2]).toUpperCase();
            var index = place.DEFAULT_COLOURS.indexOf(hexColor);
            if (matrix[i][j] != index) {
                var color = matrix[i][j];
                place.selectedColour = color;
                place.canvasClicked(pointX, pointY);
                console.log('pixel point('+pointX+','+pointY+') with color index:'+color);
                return true;
            }
        }
    }
    return false;
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
            }
        }
    },
    get: function () {
        return unlockTime;
    }
});
place.canvasClicked(startX,startY);