// Get a handle to the button's HTML element
var submitButton = document.getElementById('submit_button');

// Add a 'click' listener
submitButton.addEventListener('click', function() {
  // Get the config data from the UI elements
  var valveName = document.getElementById('valve_name').value;
  var valveGuid = document.getElementById('valve_guid').value;

  // Make a data object to be sent, coercing value types to integers
  var options = [{
    name: valveName,
    guid: parseInt(valveGuid)
  }];

  console.log(options);
  // Determine the correct return URL (emulator vs real watch)
  function getQueryParam(variable, defaultValue) {
    var query = location.search.substring(1);
    var vars = query.split('&');
    for (var i = 0; i < vars.length; i++) {
      var pair = vars[i].split('=');
      if (pair[0] === variable) {
        return decodeURIComponent(pair[1]);
      }
    }
    return defaultValue || false;
  }
  var return_to = getQueryParam('return_to', 'pebblejs://close#');

  // Encode and send the data when the page closes
  document.location = return_to + encodeURIComponent(JSON.stringify(options));
});