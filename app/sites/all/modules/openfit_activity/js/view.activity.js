(function($) {
  $(document).ready(function() {
  
    OpenFitPopupMenuCloser.create();
    
    // Add click handler to each buttonbar btn which has a sibling menu. Toggle the menu visibility on click.
    $('.buttonbar .menu').each(function(index) {
      var menu = $(this);
      var parent = menu.parent();
      var x = $('.btn > a', parent);
      $('.btn > a', parent).click(function(event) {
        var btnParent = $(event.target).closest('.btn').parent();
        btnParent.addClass('menu-open');
        var menu = btnParent.children('.menu');
        menu.css('minWidth', btnParent.width()-2);
        menu.toggle();
        event.preventDefault();
      });
    });
    
    // Add click handlers for ajax check lists
    $('.buttonbar .check-item.ajax').each(function(index) {
      $('a', this).click(function(event) {
        // Invoke the ajax
        var ajaxurl = $('[name="ajaxurl"]', this).attr('value');
        if (ajaxurl) $.ajax({url: ajaxurl});
        // Uncheck all other items and check this item
        var checklist = $(this).closest('ul');
        $('.check-img.checked', checklist).removeClass('checked');
        $('.check-img', this).addClass('checked');
        event.preventDefault();
      });
    });

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
            window.location=$('#delete-button a').attr('href');
          }
        },
        {
          text: Drupal.t('No'),
          click: function() { $(this).dialog('close'); }
        }
      ],
		});
    
    $('#delete-button .btn > a').click(function(event) {
      $('#delete-confirm').dialog('open');
      event.preventDefault();
    });
  });
})(jQuery);