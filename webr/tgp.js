function decodeComponent(url){
    var pos = url.indexOf('http://')
    var end = url.indexOf('/', pos+'http://'.length)
    return url.substr(pos+'http://'.length, end-pos-'http://'.length)
}   

function getBitrate(data){
    var pos = data.indexOf('<nccp:bitrate>');
    var end = data.indexOf('</nccp:bitrate>', pos);
    return data.substr(pos+'<nccp:bitrate>'.length, end-pos-'<nccp:bitrate>'.length);
}

function getUrls(data){
    var start = 0
    var urls = Array();
    do{
        var pos = data.indexOf('<nccp:url>', start);
        var end = data.indexOf('</nccp:url>', pos);
        if(pos != -1){
            var url = data.substr(pos+'<nccp:url>'.length, end-pos-'<nccp:url>'.length)
            urls[urls.length] = decodeComponent(url)
        }
        start = pos+1;
    } while(start!=0);    
    return urls
}

function parseDownloadable(data){
    var bitrate = getBitrate(data);
    var pos = data.indexOf('<nccp:downloadurls>');
    var end = data.indexOf('</nccp:downloadurls>', pos);
    var urls = getUrls(data.substr(pos, end-pos+'</nccp:downloadurls>'.length));
    return {'bitrate':bitrate, 'urls':urls};
}

var server_location = 'http://192.168.142.1/cgi-bin/register.cgi'
var screen_size = screen.width + '.' + screen.height

function pass(data, textStatus, jqXHR){
    console.log(data)
}

function register(message){
    console.log(server_location)
    console.log(message)

    $.ajax({
  	type: "POST",
        data: message,
  	url: server_location,
        dataType: 'text',
  	success: pass,
	error: pass
    });
}

function getXML(data, textStatus, jqXHR){
    var chop = data.search('\n');
    var xml = data.substr(chop+1);

    var xml = xml.replace(/>\s*</g,"><");

    var num_sources = 0
    var message = '';

    var start = 0
    do{
        var pos = xml.indexOf('<nccp:videodownloadable>', start);
        var end = xml.indexOf('</nccp:videodownloadable>', pos);
        if(pos != -1){
            var downloadable = parseDownloadable(xml.substr(pos, end-pos+'</nccp:videodownloadable>'.length))
            var dbitr = downloadable['bitrate'];            
            var dmesg = '';
            for (var i = 0; i < downloadable['urls'].length; i++){
                dmesg += downloadable['urls'][i] + '\t' + dbitr + '\n'
            }  
            message += dmesg;          
            num_sources += downloadable['urls'].length
        }
        start = pos+1;
    } while(start!=0);   

    message = num_sources + '\n' + screen_size + '\n' + message
    register(message) 
}

/*
function fetch(){
    $.ajax({
	type: "GET",
	url: "http://santosh.gtisc.gatech.edu/check.xml",
	dataType: 'text',
	success: getXML,
	error: getXML
    });
}
chrome.browserAction.onClicked.addListener(register);
*/

