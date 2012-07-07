(function($) {
  /**
   * A jQuery plugin for table selection.
   * @param options
   *   An array of options for the table.
   *   - multiSelect: Boolean indicating whether multiselection with CTRL and SHIFT is allowed.
   *   - allowEmptySelection: Boolean indicating whether selection can be entirely cleared.
   */
  $.fn.table = function(options) {
    var opts = $.extend({}, $.fn.table.defaults, options);
    var me = this;
    var tableBody = $('tbody', this);
    var rangeStartIndex = -1;
    
    return this.each(function() {
      $(this).disableSelection();
      onSelectionChanged();
      $('tr', tableBody).click(onClick);
    });

    /**
     * Click event handler.
     */     
    function onClick(event) {
      // If multi-select is allowed, SHIFT is pressed, and a prior anchor point is selected, select range or rows.
      if (opts.multiSelect && event.shiftKey && rangeStartIndex != -1) {
        // Clear prior selection.
        $('tr', tableBody).each(function(index) {
            jQuery(this).toggleClass('selected', false);
        });
        // Get a slice of the selection range.
        var start = rangeStartIndex;
        var end = $(this).index();
        if (end < start) {
          var swap = start;
          start = end;
          end = swap;
        }
        // Select the rows in the slice.
        $('tr', tableBody).slice(start, end + 1).each(function(index) {
          jQuery(this).toggleClass('selected', true);
        });
      } else {
        // If multi-select is not allowed or CTRL is not pressed, clear all other selected rows.
        if (!opts.multiSelect || !event.ctrlKey) {
          $('tr', tableBody).each(function(index) {
              jQuery(this).toggleClass('selected', false);
          });
        }
        // Either set or toggle the selected row based on whether empty is allowed.
        if (opts.allowEmptySelection || $('tr.selected', tableBody).length > 1) {
          $(this).toggleClass('selected');
        } else {
          $(this).toggleClass('selected', true);
        }
        rangeStartIndex = $(this).index();
      }
      // Trigger the selectionChanged event.
      onSelectionChanged();
    }
    
    /**
     * Handle a selection change by triggering a selectionChanged event.
     */
    function onSelectionChanged() {
      $(me).trigger('selectionChanged');
    }
    
    /**
     * Construction option defaults
     */
    $.fn.table.defaults = {
      multiSelect: false,
      allowEmptySelection: false,
    }
  }
  
  /**
   * Return a list of jQuery elements for the selected rows.
   */
  $.fn.table.getSelectedElements = function() {
    return $('tbody tr.selected');
  };
    
})(jQuery);

