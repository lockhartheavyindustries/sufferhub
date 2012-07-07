(function($) {

  $(document).ready(function() {
    var chart = $('#activity-chart');
    chart.openfit_activitydetailchart();
    
    var ajaxUrl = chart.attr('data-dataurl');
    $.ajax({
      url: ajaxUrl,
      success: function(data, textStatus, jqXHR) {
        var dataTracks = {};
        for (var trackId in data) dataTracks[trackId] = data[trackId];
        chart.openfit_activitydetailchart('setDataTracks', dataTracks);
      },
      error: function(jqXHR, textStatus, errorThrown) {
        // TODO: Handle this by setting text in chart div and error icon
        alert('error code: ' + jqXHR.status + ': ' + jqXHR.responseText);
      },
    });
  });
})(jQuery);
