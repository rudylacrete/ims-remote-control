// base 64 encode
var b64array = "ABCDEFGHIJKLMNOP" +
           "QRSTUVWXYZabcdef" +
           "ghijklmnopqrstuv" +
           "wxyz0123456789+/" +
           "=";
function encode64(input) {
    var base64 = "";
    var chr1, chr2, chr3 = "";
    var enc1, enc2, enc3, enc4 = "";
    var i = 0;
    do {
        chr1 = input.charCodeAt(i++);
        chr2 = input.charCodeAt(i++);
        chr3 = input.charCodeAt(i++);
        enc1 = chr1 >> 2;
        enc2 = ((chr1 & 3) << 4) | (chr2 >> 4);
        enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
        enc4 = chr3 & 63;
        if (isNaN(chr2)) {
            enc3 = enc4 = 64;
        } else if (isNaN(chr3)) {
            enc4 = 64;
        }
        base64  = base64  +
            b64array.charAt(enc1) +
            b64array.charAt(enc2) +
            b64array.charAt(enc3) +
            b64array.charAt(enc4);
        chr1 = chr2 = chr3 = "";
        enc1 = enc2 = enc3 = enc4 = "";
    } while (i < input.length);
    //console.log(base64 );
    return base64;
}
// end



var CLIENT_ID = "test";
var CLIENT_SECRET = "test";
var TOKEN_URL = "http://scooterlabs.com/echo.json";

function _generateBasicAuthHeaderValue(username, password) {
  return "Basic " + encode64(username + ":" + password);
}



var WebServiceConnector = function() {
  this.baseUri = "http://scooterlabs.com/echo.json";
  this._token = null;
};
  
WebServiceConnector.prototype._makeRequest = function(method, options, callback) {
  options = options || {};
  var path = options.path || "";
  var headers = options.headers || {};
  var body = options.body || "";
  var request = new XMLHttpRequest();
  var baseUri = options.baseUri || this.baseUri;
  request.onreadystatechange = function() {
    if (request.readyState == 4) {
      if (request.status == 200 || request.status === 0) {
    		console.log("OK ..." + request.responseText);
        console.log(request.status, request.statusText);
        try{
          var json = JSON.parse(request.responseText);
          callback && callback(null, json);
        }
        catch(e) {
          callback && callback(null, request.responseText);
        }
      }
      else {
        console.error(request.status, request.statusText);
        callback(new Error(request.statusText));
      }
  	}
  };
  // Send the request
  request.open(method, baseUri + path);
  Object.keys(headers).forEach(function(key) {
    request.setRequestHeader(key, headers[key]);
  });
  if(!(options.headers && options.headers['Content-Type']))
    request.setRequestHeader('Content-Type', 'application/json');
  request.send(body);
};

WebServiceConnector.prototype._getToken = function(callback) {
  var options = {
    baseUri: TOKEN_URL,
    headers: {
      "Authorization": _generateBasicAuthHeaderValue(CLIENT_ID, CLIENT_SECRET),
      "Content-Type": "application/x-www-form-urlencoded"
    },
    body: "grant_type=client_credentials"
  };
  this._makeRequest("POST", options, function(error, response) {
    if(error) return callback(error);
    this._token = response.access_token;
    callback(null);
  });
};

WebServiceConnector.prototype.callApiEndpoint = function(path, method, body, callback) {
  var self = this;
  var options = {
    path: path,
    body: body,
    headers: {}
  };
  function _sendRequest() {
    options.headers.Authorization = this._token;
    self._makeRequest(method, options, callback);
  }
  if(this._token) {
    _sendRequest();
  }
  else {
    this._getToken(function(error) {
      if(error) return callback(error);
      _sendRequest();
    });
  }
};

WebServiceConnector.prototype.openValve = function(guid) {
    this.callApiEndpoint("", "GET", "", console.log);
};





var webServiceConnector = new WebServiceConnector();

Pebble.addEventListener('ready', function(){
  console.log("Pebble JS ready !!!");

  // Update s_js_ready on watch
  Pebble.sendAppMessage({'AppKeyJSReady': 1});
});

function configureValves(valves) {

  function sendValve(index) {
    if(typeof index == 'undefined') index = 0;
    if(index == valves.length) {
      return console.log("Done sending valves ....");
    }
    var valve = valves[index];
    var dico = {ValveName: valve.name, ValveGuid: valve.guid};
    Pebble.sendAppMessage(dico, function() {
      sendValve(++index);
    }, function() {
      console.error("Failed to send valve", valve);
    });
  }
  
  Pebble.sendAppMessage({'ValveNumber': valves.length}, function() {
    console.log("Valve number sent, begin to push valve informations ....");
    sendValve();
  }, function() {
    console.error("Failed to send valve count");
  });
  
}

Pebble.addEventListener('appmessage', function(e) {
  console.log("Incoming message ....");
  var dict = e.payload;
  if(typeof dict.ValveCmdRequest != 'undefined')
    handleValveCmdRequest(dict.ValveGuid, dict.ValveCmdRequest);
});

function handleValveCmdRequest(guid, cmd) {
  console.log("....." + guid + " " + cmd);
  webServiceConnector.openValve(guid);
}


Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://adccdb24.ngrok.io/';
  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode the user's preferences
  var configData = JSON.parse(decodeURIComponent(e.response));
  console.log(JSON.stringify(configData));
  configureValves(configData);
});
