var dictIn = msg.payload;
var dictOut = [];

for (var key in dictIn) {
    var item = {
        BSSID: key,
        RSSI: dictIn[key].rssiVal,
        mean: dictIn[key].mean,
        stdDev: dictIn[key].stdDev,
        
    };
    dictOut.push(item);
}

msg.payload = dictOut;
return msg;