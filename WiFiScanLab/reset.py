var dictOut = [];

if (msg.payload==1){
    delete context.global.dictOut;
    msg.payload = "Reset"
}
return msg;




