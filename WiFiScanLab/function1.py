var dictIn = msg.payload;  //  Assuming msg.payload contains the JSON data
var keys = [[],[]];
if (dictIn.length > 0) {
    for (var i = 0; i < dictIn.length ; i++) {
        var bssid = dictIn[i].bssid;
        var rssi = parseInt(dictIn[i].rssi);
        keys[0].push(bssid);

        if (!context.global.dictOut) {
            context.global.dictOut = {};
        }

        if (!context.global.dictOut[bssid]) {
            context.global.dictOut[bssid] = {
                rssiList: [],
                rssiVal: rssi,
                mean: 0,
                stdDev: 0
            };
        }

        var rssiList = context.global.dictOut[bssid].rssiList;

        rssiList.push(rssi);
        context.global.dictOut[bssid].rssiVal = rssi;

        if (rssiList.length > 25) {
            rssiList.shift(); // Remove the first element if the list is longer than 25
        }
    }
    
    
    // Iterate over existing BSSIDs in dictOut
    for (var key in context.global.dictOut) {
        var rssiList2 = context.global.dictOut[key].rssiList;
        if (!keys[0].includes(key)) {
            // If a BSSID is not in the new data (dictIn)

            rssiList2.push(-120);// Append a -120 to the RSSI list
            context.global.dictOut[key].rssiVal = -120;
            if (rssiList2.length > 25) {

                if (rssiList2.every(function (value) { return value === -120; })) {
                    // If all elements are -120 for 25 consecutive observations, remove the BSSID
                    delete context.global.dictOut[key];
                    continue;
                } else {
                    // Remove the first element
                    rssiList2.shift();
                }
            }
        }

        // Calculate mean
        context.global.dictOut[key].mean = rssiList2.reduce(function (a, b) { return a + b; }, 0) / rssiList2.length;

        // Calculate standard deviation
        var squaredDiffs = rssiList2.map(function (value) {
            return Math.pow(value - context.global.dictOut[key].mean, 2);
        });
        var variance = squaredDiffs.reduce(function (a, b) { return a + b; }, 0) / rssiList2.length;
        context.global.dictOut[key].stdDev = Math.sqrt(variance);
    }
}

if (context.global.dictOut) {
    // Get an array of keys from context.global.dictOut
    var sortedKeys = Object.keys(context.global.dictOut);

    // Sort the keys in ascending order (alphabetical order)
    sortedKeys.sort();

    // Create a new object to hold the sorted data
    var sortedDictOut = {};

    // Populate the new object with the sorted data
    for (var i = 0; i < sortedKeys.length; i++) {
        var key = sortedKeys[i];
        sortedDictOut[key] = context.global.dictOut[key];
    }

    // Update context.global.dictOut with the sorted data
    context.global.dictOut = sortedDictOut;
}

msg.payload = context.global.dictOut;


return msg;