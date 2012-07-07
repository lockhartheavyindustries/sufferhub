(function($) {
  $(document).ready(function() {
    var summaryChart = new MiniBarChart(document.getElementById('summary-chart'));
    summaryChart.bars.color = '#9BC642';
    summaryChart.bars.spacing = 8;
    summaryChart.setBarData(Drupal.settings.summaryChartData);
  });
})(jQuery);