var rawdata;
chrome.webRequest.onBeforeRequest.addListener(function(details){
    //console.log("Before Request: ");
    data = details.requestBody.raw[0];
    rawdata = String.fromCharCode.apply(null, new Uint8Array(data["bytes"]));
    //console.log(rawdata);
},
{urls: [ "https://cbp-us.nccp.netflix.com/nccp/controller/2.16/authorization" ]},["requestBody"]);

var headrs = {};
var originalRequest = 1;
chrome.webRequest.onSendHeaders.addListener(function(details){
    //console.log("Request:");
    var headers = details.requestHeaders,
    blockingResponse = {};

    for( var i = 0, l = headers.length; i < l; ++i ) {
	if (headers[i].name == "Accept"){
		originalRequest = 0
	}
	headrs[headers[i].name] = headers[i].value;
	//console.log(headers[i].name);
        //console.log(headers[i].value);
    }

    blockingResponse.requestHeaders = headers;
    return blockingResponse;
},
{urls: [ "https://cbp-us.nccp.netflix.com/nccp/controller/2.16/authorization" ]},['requestHeaders']);

/*function getXML(data, textStatus, jqXHR){
    console.log("Get XML!");
    console.log(data);
    console.log(textStatus);
    console.log(jqXHR);
}*/

chrome.webRequest.onCompleted.addListener(function(details){
    //console.log("Response:");
    //console.log(JSON.stringify(details));

    if (originalRequest == 0){
        console.log("Quitting, can't do this twice!");
        originalRequest = 1;
    	return;
    }

    console.log("Making my own Request:");
    //console.log(headrs);
    $.ajax({
  	type: "POST",
        headers: headrs,
        data: rawdata,
  	url: "https://cbp-us.nccp.netflix.com/nccp/controller/2.16/authorization",
        dataType: 'text',
  	success: getXML,
	error: getXML
    });
},
{urls: [ "https://cbp-us.nccp.netflix.com/nccp/controller/2.16/authorization" ]},['responseHeaders']);
