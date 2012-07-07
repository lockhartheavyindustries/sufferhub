var OpenFitPopupMenuCloser = {
  create: function() {
    (function($) {
      $(document).click(function(event) {      
        var target = $(event.target).closest('.btn').parent();
        var mymenu = null;
        if (target.parent('.commandbar')) mymenu = target.children('.menu')[0];
        
        $('.buttonbar .menu').each(function(index) {
          if (this != mymenu) {
            var othermenu = $(this);
            othermenu.hide();
            othermenu.parent().removeClass('menu-open');
          }
        });
      });
    })(jQuery);
  }
};

(function($) {
  var PopupSelectMenu = {
    _init: function() {
      var me = this
      $('.btn a', this.element).click(function(event) {
        var menu = $('.menu', me.element);
        menu.css('minWidth', me.element.width()-2);
        menu.toggle();
        me.element.addClass('menu-open');
        event.preventDefault();
      });
      this.setItems(this.options.items);
      this.setSelected(this.options.selected);
    },
    setItems: function(items) {
      this.options.items = items;
      $('.menu', this.element).remove();
      var menuHtml = '<div class="menu" style="display:none"><ul>';
      if (this.options.items.length == 0) {
        menuHtml += '<li>&nbsp</li>';
      } else {
        $.each(this.options.items, function(index, value) {
          var itemHtml = value.html != null ? value.html : '<span class="name">' + value.name + '</span>';
          menuHtml += '<li><a href="#' + value.id + '" data-menuid="' + value.id + '">' + itemHtml + '</a></li>';
        });
      }
      menuHtml += '</ul></div>';
      this.element.append(menuHtml);
      var me = this;
      var menu = $('.menu', this.element);
      menu.each(function(index, value) {
        $('a', this).click(function(event) {
          var selectedId = $(this).attr('data-menuid');
          if (me.options.selected != selectedId) {
            me.setSelected(selectedId);
            menu.toggle();
            me.element.trigger('selectedChanged', selectedId);
          }
          event.preventDefault();
        });
      });
      this.setSelected(this.options.selected);
    },    
    setSelected: function(selected) {
      var me = this;
      $.each(this.options.items, function(index, value) {
        if(value.id==selected) { 
          me.options.selected = selected;
          var itemText = value.html != null ? value.html : value.name;
          $('.btn a .name', me.element).html(itemText);
          return false; 
        }
      });
    },    
    options: {
      items: [],
      selected: ''
    }
  };
  $.widget("ui.openfit_popupselectmenu", PopupSelectMenu);
})(jQuery);
