(function ($) {
  Drupal.behaviors.search_exclude_nid = {
    attach: function (context, settings) {
      // Handler for .ready() called.
      var filtered_nids_list = Array();
      jQuery('#edit-submit').click(function() {
        var nid = parseInt(jQuery('#edit-search-exclude-nid-search-exclusion-auto').val());
        if (!(isNaN(nid)) && (nid != 0)) {
          filtered_nids_list = getFilteredNidList();
          if (filtered_nids_list.containsNot(nid)) {
            filtered_nids_list.push(nid);
          }
          jQuery('#edit-search-exclude-nid-search-exclusion-nids').val(filtered_nids_list.join(','));
          jQuery('#edit-search-exclude-nid-search-exclusion-auto').val('').focus();
        }
        filtered_nids_list = getFilteredNidList();
        jQuery('#edit-search-exclude-nid-search-exclusion-nids').val(filtered_nids_list.join(','));
      });
    }
  };
})(jQuery);

Array.prototype.containsNot = function(obj) {
  var i = this.length;
  while (i--) {
    if (this[i] == obj) {
      return false;
    }
  }
  return true;
}

function getFilteredNidList() {
  var nidsList = jQuery('#edit-search-exclude-nid-search-exclusion-nids').val().split(',');
  var filtered_nids_list = new Array();
  var intRegex = /^\d+$/;
  for (var i in nidsList) {
    if (intRegex.test(nidsList[i]) && (nidsList[i] > 0) && (filtered_nids_list.containsNot(nidsList[i]))) {
      filtered_nids_list.push(nidsList[i]);
    }
  }
  return filtered_nids_list;
}
