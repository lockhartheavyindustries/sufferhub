(function ($) {
  var OPENFIT_DATA_URL = Drupal.settings.basePath + "openfit/api/";
  var OPENFIT_TICKER_URL = Drupal.settings.basePath + "openfit/activityticker";
  $(document).ready(function() {
    $.getJSON(OPENFIT_DATA_URL, { op: 'get_homepage_data' }, function(response) {
      $(".digit-list").children().remove();
      
      var sepRgx = /\D/g,
          value = String(response.value),
          sepCount = (value.match(sepRgx) == null) ? 0 : value.match(sepRgx).length,
          digitCounter = 0,
          classes = [];
      
      //I found it easier to go right to left 
      //through the string as though you were reading it
      for (var i = value.length - 1; i >= 0 ; i--) {
        if (value[i].match(/\s/) || isNaN(value[i])) {
          classes.unshift([value[i] , "seperator"]);
        } else {
          var digitNum = (digitCounter%3 + 1);
          if (digitCounter == value.length - sepCount - 1) digitNum = digitNum +" d3"; //ensure rounded corners on the end
          classes.unshift([value[i], "digit d"+digitNum]);
          digitCounter++;
        }
      }
      
      //For scaling.  For n as the next nearest multiple of 3 from digitCounter, Classes are digit-num-n
      var scaleNum = ((digitCounter % 3) != 0) ? 3 * Math.ceil(digitCounter/3) : digitCounter;
      $(".digit-list").addClass("digit-num-"+scaleNum);
      
      for (var i = 0; i < classes.length; i++) {
        $(".digit-list").append($("<li>", { "class": classes[i][1] }).text(classes[i][0]));
      }
      
      $("p.label").html(response.label);
      $("p.equivalent").html(response.equivalent);
    });
    var loadCallback = function() {
      $("#ticker").load(OPENFIT_TICKER_URL, function() {
        //quickfix for node_view($node, 'teaser') returning clearfixed lines of teasers
        $("#ticker article").each(function() {
          $(this).removeClass("clearfix");
          $(this).attr("style", "float: left; margin: 0;");
        });
        //setTimeout(loadCallback, 10000);
      });
    };
    loadCallback();
  });
}) (jQuery);