(function($) {
  $(document).ready(function() {
    // Attach selection listeners to the activity table.
    var activityTable = $('#activity-table');
    activityTable.bind('selectionChanged', function(event) {
      var rowsSelected = activityTable.table.getSelectedElements();
      $('tbody tr input:checkbox').each(function() {
        if (this.checked) $(this).attr('checked', false);
      });
      rowsSelected.each(function() {
        $('input:checkbox', this).attr('checked', true); 
      });
      $('#edit-button').toggle(rowsSelected.length == 1);
      $('#delete-button').toggle(rowsSelected.length != 0);
      if (rowsSelected.length == 1) {
        var editUrl = $('a.view-activity', rowsSelected).attr('href') + '/edit';
        $('#edit-button a').attr('href', editUrl);
      }
    });
    activityTable.table({multiSelect:true});
    
    $('#delete-confirm').dialog({
      autoOpen: false,
			resizable: false,
      width: 520,
			height: 140,
			modal: true,
			buttons: [
        {
          text: Drupal.t('Yes'),
          click: function() {
            $(this).dialog('close'); 
            $('#edit-action-type').attr('value', 'delete');
            $('#openfit-activity-activities-form').submit();
          }
        },
        {
          text: Drupal.t('No'),
          click: function() { $(this).dialog('close'); }
        }
      ],
		});
    
    $('#delete-button a').click(function(event) {
      $('#delete-confirm').dialog('open');
      return false;
    });
  });
})(jQuery);