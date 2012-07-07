(function($) {
  $(document).ready(function() {
    var thisWeekChart = new MiniBarChart(document.getElementById('this-week-chart'));
    thisWeekChart.bars.color = '#9BC642';
    thisWeekChart.setBarData(Drupal.settings.weekChartData);
    
    var thisMonthChart = new MiniBarChart(document.getElementById('this-month-chart'));
    thisMonthChart.bars.color = '#9BC642';
    thisMonthChart.setBarData(Drupal.settings.monthChartData);
  });
})(jQuery);