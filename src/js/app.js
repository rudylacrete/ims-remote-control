var WebServiceConnector = function() {
  this.baseUri = "http://scooterlabs.com/echo.json";
};
  
WebServiceConnector.prototype._makeRequest = function(path, method, parameters) {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    console.log(':::::::', request.readyState);
  	if (request.readyState == 4 && (request.status == 200 || request.status === 0)) {
  		console.log("OK ..." + request.responseText);
  	}
  };
  // Send the request
  request.open(method, this.baseUri + path);
  request.setRequestHeader('Content-Type', 'application/json');
  request.send();
};

WebServiceConnector.prototype.openValve = function(guid) {
    this._makeRequest("", "GET", {});
};

var webServiceConnector = new WebServiceConnector();

Pebble.addEventListener('ready', function(){
  console.log("Pebble JS ready !!!");

  // Update s_js_ready on watch
  Pebble.sendAppMessage({'AppKeyJSReady': 1});
  var valves = [{name: 'test', guid: 1}, {name: 'test2', guid: 2}];
  
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
});

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