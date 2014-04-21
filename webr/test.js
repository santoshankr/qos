var fs = require('fs');

var $ = require('jquery')(window);
function getXML(data){
    console.log("Get XML!");
    console.log(data);

    console.log(typeof data)
    chop = data.search('\n');
    xml = data.substr(chop+1);

    xml = xml.replace(/>\s*</g,"><");
    console.log(xml);

    xmldoc  = $.parseXML(xml),
    $xml = $( xmldoc );
    
    childs = $xml.find("nccp:responseheader").each(function(){console.log(this)});
    
    m = $("nccp:response").find("nccp:responseheader");
    console.log(m);

    response = $xml.children()[0];
    rheader  = $xml.find('nccp:responseheader')[0];

    console.log($(response).children());
    console.log($(rheader).children());
}

/*fs.readFile('check.xml', {encoding:'utf8'}, function (err, data) {
  if (err) throw err;
  getXML(data);
});*/

(function () {
  'use strict';

  var env = require('jsdom').env, html = '<html><body><h1>Hello World!</h1><p class="hello">Heya Big World!</body></html>';

  env(html, function (errors, window) {
    console.log(errors);
    var $ = require('jquery')(window);
    console.log($('.hello').text());
  });
}());

