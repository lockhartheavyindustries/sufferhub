(function($) {
  $(document).ready(function() {
    var lastValidName =  $("#edit-fullname").val();
    
    $("#edit-fullname").change(function() {
      if ($(this).val().replace(/\s/g, "") == "") {
        $(this).val(lastValidName);
      } else {
        lastValidName = $(this).val();
      }
    });
    
    $("#upload-file").live('change', function() {
      $('#ok-button').attr('disabled','').removeClass('ui-state-disabled');
      $('#openfit-user-edit-picture-form').ajaxForm($.extend({}, {target:'#preview'}, {
        url: ('user/set-avatar/' + $('#profile-uid').val() + '/preview'),
      })).submit();
    });
    
    $('#openfit-user-edit-picture-form').dialog({
      autoOpen: false,
      resizable: false,
      width: 500,
      height: 450,
      modal: true,
			buttons: [
        {
          id: 'ok-button',
          text: Drupal.t('Ok'),
          click: function() {
            $(this).dialog('close'); 
            $('#openfit-user-edit-picture-form').ajaxForm($.extend({}, {target:'#preview'}, {
              url: ('user/set-avatar/' + $('#profile-uid').val() + '/submit'),
              success: function() {
                window.location = Drupal.settings.basePath + 'user';
              },
            })).submit();            
          }
        },
        {
          id: 'cancel-button',
          text: Drupal.t('Cancel'),
          click: function() { $(this).dialog('close'); }
        }
      ],      
    });
    
    $('#photo div').click(function() {
      // Reset the image URL, clear the file input and disable the ok button.
      $("#preview div img").attr('src', $("#photo div img").attr('src'));
      $("#upload-file").replaceWith('<input id="upload-file" class="form-file" type="file" size="60" name="files[file]">'); 
      $('#ok-button').attr('disabled','disabled').addClass('ui-state-disabled');
      $('#openfit-user-edit-picture-form').dialog('open');
      return false;
    });
    $('#photo div').hover(
      function() {
        $('#avatar-link-hover').show();
      },
      function() {
        $('#avatar-link-hover').hide();
      }
    );
  });
}) (jQuery);
