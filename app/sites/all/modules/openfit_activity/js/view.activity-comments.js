(function($) {
  $(document).ready(function() {
    var commentDeleteUrl = '';
    $('#delete-comment-confirm').dialog({
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
            window.location=commentDeleteUrl;
          }
        },
        {
          text: Drupal.t('No'),
          click: function() { $(this).dialog('close'); }
        }
      ],
		});
    
    $('.comment-actions .comment-delete a').click(function(event) {
      commentDeleteUrl = $(this).attr('href');
      $('#delete-comment-confirm').dialog('open');
      event.preventDefault();
    });
  });
})(jQuery);