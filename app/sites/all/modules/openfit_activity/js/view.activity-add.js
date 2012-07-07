(function($) {
  $(document).ready(function() {
    var onSportChange = function(option) {
      var selected_value = option.attr('value');
      var selected = $("option[value='" + selected_value + "']");
      $('#edit-activity-info-name').attr('value', selected.text());
    }
    $('#edit-activity-info-sport').change(function() {
      onSportChange($(this));
    });
    onSportChange($('#edit-activity-info-sport'));
  });
})(jQuery);