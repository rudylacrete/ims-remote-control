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