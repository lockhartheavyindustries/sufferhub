(function($) {
  $(document).ready(function() {
    var ajaxUrl = $('#route').attr('data-dataurl');
    $.ajax({
      url: ajaxUrl,
      success: function(data, textStatus, jqXHR) {
        loadRoute(data);
      },
      error: function(jqXHR, textStatus, errorThrown) {
        // TODO: Handle this by setting text in map div and error icon
        alert('error code: ' + jqXHR.status + ': ' + jqXHR.responseText);
      },
    });
  });
})(jQuery);

function loadRoute(data) {
  if (data.route.interval == null) return;
  // TODO: Dummy options values for now.
  // We at least want to remember the map type as a user preference and pass it back to the client from the server.
  // We also may want to set the center to the user's neighborhood, or location center, etc.
  var myOptions = { 
    center: new google.maps.LatLng(0,0),
    zoom: 2,
    mapTypeId: google.maps.MapTypeId.ROADMAP
  };
  var map = new google.maps.Map(document.getElementById("gmap"), myOptions); 
  var routeCoordinates = [new google.maps.LatLng(data.route.start[1], data.route.start[2])];
  var i = 1;
  var pt = 0;
  var count = data.route.data.length;
  while (pt < count) {
    routeCoordinates[i] = new google.maps.LatLng(data.route.data[pt], data.route.data[pt + 1]);
    i++;
    pt += 2;
  }
  routeCoordinates[i] = new google.maps.LatLng(data.route.end[1], data.route.end[2]);
  var finishCoordinate = routeCoordinates[i];
  
  var bounds = new google.maps.LatLngBounds();
  for (var i = 0; i < routeCoordinates.length; i++) bounds.extend(routeCoordinates[i]);
  // TODO: More route line options should come from user preferences
  var route = new google.maps.Polyline(
    {
      path: routeCoordinates,
      strokeColor: data.route.color,
      strokeOpacity: 0.6,
      strokeWeight: 4
    }
  );
  var startMarker = new google.maps.Marker(
    {
      flat: true,
      icon: {url:"/openfit/images/route-markers.png",anchor:{x:10,y:10},size:{width:20,height:20},origin:{x:0,y:0}},
      map: map,
      position: routeCoordinates[0]
    }
  );
  var finishMarker = new google.maps.Marker(
    {
      flat: true,
      icon: {url:"/openfit/images/route-markers.png",anchor:{x:10,y:10},size:{width:20,height:20},origin:{x:20,y:0}},
      map: map,
      position: finishCoordinate
    }
  );
  route.setMap(map);    
  map.fitBounds(bounds);
}
