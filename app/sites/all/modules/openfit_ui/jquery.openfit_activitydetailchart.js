(function($) {
  var ActivityDetailChart = {
    _init: function() {
      var me = this;
      var opts = this.options;
      
      this._zoomOutButton = $('#zoom-out-button');
      this._zoomOutButton.click(function(event) { me._onZoomOutClicked(); event.preventDefault(); });
    
      this._metricWidget = $('.buttonbar.metrics #activity-chart-metric', this.element);
      this._metricWidget.openfit_popupselectmenu();
      this._metricWidget.bind('selectedChanged', function(event,selected) { me._onMetricChanged(selected); event.preventDefault(); });
      
      this._xAxisWidget = $('.buttonbar.xaxis .xaxis-selector', this.element);
      this._xAxisWidget.openfit_popupselectmenu({selected:opts.xAxis});
      this._xAxisWidget.bind('selectedChanged', function(event,selected) { me._onXAxisChanged(selected); event.preventDefault(); });
      
      this.setDataTracks(opts.dataTracks);
    
      this._chartContainer = $('.chart-canvas-container:first');
      this._chartContainer.bind('plotselected', function (event, ranges) { me._onChartSelectionChanged(ranges.xaxis.from.toFixed(1), ranges.xaxis.to.toFixed(1)); });

      this._refreshChartData();
    },
    setDataTracks: function(dataTracks) {
      if (dataTracks == null) dataTracks = {};
      var opts = this.options;
      opts.dataTracks = dataTracks;
      this._setZoomRange(null);
      
      var metricOptions = new Array();
      var firstMetricId = null;
      for (var trackId in dataTracks) {
        var track = dataTracks[trackId];
        track.id = track.type;
        var units = '';
        var m = track.measurement;
        if (m != null) {
          units = m.unit_symbol;
          m.f = parseFloat(m.conversion_factor);
          m.o = parseFloat(m.conversion_offset);
        }
        if (track.required != null) {
          var exists = false;
          for (var t in dataTracks) if (t == track.required) exists = true;
          if (!exists) continue;
        }
        if (track.hidden != null) continue;
        metricOptions.push({id:track.id,name:track.title,html:this._metricItemHtml(track.color,track.title,units)});
        if (firstMetricId == null) firstMetricId = trackId;
      }
      
      if (opts.metric.length == 0 && firstMetricId != null) opts.metric = firstMetricId;
      
      var xAxisHtml = '<span class="xaxis-name">%name%</span><span class="xaxis-units">%units%</span>';
      var xAxisOptions = new Array();
      if (opts.dataTracks['distance'] != null) {
        var m = opts.dataTracks['distance'].measurement;
        var units = m != null ? m.unit_symbol : '';
        var html = xAxisHtml.replace('%name%', Drupal.t('Distance')).replace('%units%',units);
        xAxisOptions.push({id:'distance',name:Drupal.t('Distance'),html:html});
      }
      var html = xAxisHtml.replace('%name%', Drupal.t('Time')).replace('%units%','');
      xAxisOptions.push({id:'time',name:Drupal.t('Time'),html:html});
      if(opts.xAxis.length == 0 && firstMetricId != null && xAxisOptions.length > 0) opts.xAxis = xAxisOptions[0].id;

      $('.buttonbar.metrics', this.element).toggle(firstMetricId != null);
      $('.buttonbar.settings', this.element).toggle(firstMetricId != null);
      $('.buttonbar.xaxis', this.element).toggle(firstMetricId != null);
      
      this._metricWidget.openfit_popupselectmenu('setItems',metricOptions);
      this._metricWidget.openfit_popupselectmenu('setSelected',opts.metric);
      this._xAxisWidget.openfit_popupselectmenu('setItems', xAxisOptions);
      this._xAxisWidget.openfit_popupselectmenu('setSelected',opts.xAxis);      
      this._refreshChartData();
    },
    setMetric: function(metric) {
      this.options.metric = metric;
      this._refreshChartData();
    },
    setXAxis: function(xAxis) {
      var opts =  this.options;
      var z= this._zoomRange;
      if (z != null && opts.xAxis != xAxis && opts.dataTracks['distance'] != null) {
        var distanceTrack = opts.dataTracks['distance'];
        switch (opts.xAxis) {
          case 'time':
            if (xAxis == 'distance') {
              this._setZoomRange([
                this._getValueAtTime(distanceTrack,z[0]),
                this._getValueAtTime(distanceTrack,z[1])
              ]);
            }
            break;
          case 'distance':
            if (xAxis == 'time') {
              this._setZoomRange([
                this._getTimeAtDistance(z[0]),
                this._getTimeAtDistance(z[1])
              ]);
            }
            break;
        }
      }
      opts.xAxis = xAxis;
      this._refreshChartData();
    },
    options: {
      dataTracks: {},
      xAxis:'',
      metric:''
    },
    _zoomRange:null,
    _metricItemHtml: function(color,name,units) {
      return '<div class="metric-color" style="color:' + color + '"><span>&bull;</span></div><span class="metric-name">' + 
        name + '</span><span class="metric-units">' + units + '</span>';
    },
    _refreshChartData: function() {
      if (this._chartContainer == null) return;
    
      var me = this;
      var opts = this.options;
      var dataTrack = opts.dataTracks[opts.metric];
      
      try {    
        var chartData = this._getMetricData();
      } catch(err) {
        var chartData = {data:null,min:0,max:2,avg:1};
      }
      // Spread the yaxis range out by 5% on each side
      var ymin = chartData.min;
      var ymax = chartData.max;
      var yavg = chartData.avg;
      var diff = Math.abs(ymax-ymin);
      
      if (diff < 0.01) {
        diff = 1;
        if (opts.metric == 'pace') diff = 30;
      } else {
        diff *= 0.05;
      }
      ymin = ymin >= 0 ? Math.max(0,ymin-diff) : ymin-diff;
      ymax = ymax <= 0 ? Math.min(0,ymax+diff) : ymax+diff;
      if (ymin == ymax && ymax == 0) ymax += 1;
      if (dataTrack != null && dataTrack.zoommaxavg != null) {
        ymax = Math.min(ymax,yavg*dataTrack.zoommaxavg);
      }
      
      var chartOptions = {
        legend:{show:false},
        shadowSize:0,
        grid:{borderWidth:0},
        xaxis:{color:'#AAA',tickColor:'rgba(0,0,0,0)'},
        yaxis:{color:'#666',tickColor:'#EEE',ticks:4,min:ymin,max:ymax},
        selection:{mode:'x'}
      }
      
      var zoomRange = this._zoomRange;
      if (chartData.data != null && chartData.data.length == 0) {
        zoomRange = [0,1];
        chartOptions.xaxis.show = false;
        chartOptions.yaxis.labelWidth = 0;
      }
      if (zoomRange != null) {
        chartOptions.xaxis.min = zoomRange[0];
        chartOptions.xaxis.max = zoomRange[1];
      }
      switch (opts.xAxis) {
        case 'time':
          chartOptions.xaxis.tickFormatter = function(value,axis) {return me._tickFormatterTime(value,axis);};
          // TODO: Special tick generator for time
          break;
        case 'distance':
          chartOptions.xaxis.tickFormatter = this._getTickFormatter(opts.dataTracks['distance']);
          chartOptions.xaxis.ticks = this._getTickGenerator(opts.dataTracks['distance']);
          break;
      }
      chartOptions.yaxis.tickFormatter = this._getTickFormatter(opts.dataTracks[opts.metric]);
      chartOptions.yaxis.ticks = this._getTickGenerator(opts.dataTracks[opts.metric]);
      
      var data = [];
      if (chartData.data != null) data = [chartData.data];
      $.plot(this._chartContainer, data, chartOptions);
    },
    _getTickFormatter: function(track) {
      if (track == null || track.measurement == null) return null;
      var m = track.measurement;
      var me = this;
      if (m.time != null) return function(value,axis) { return me._tickFormatterTime(value,axis); };
      if (m.f == 1 && m.o == 0) return null;
      return function(value,axis) {
        value = me._convertValue(m,value);
        var decimals = axis.tickDecimals;
        if (m.unit_decimals != null) decimals = Math.max(decimals,m.unit_decimals);
        return value.toFixed(decimals);
      };
    },
    _getTickGenerator: function(track) {
      if (track == null || track.measurement == null) return null;
      var m = track.measurement;
      if (m.time != null) return null; // TODO: Time tick generator
      if (m.f == 1 && m.o == 0) return null;
      var me = this;
      return function(axis) { return me._tickGenerator(m,axis); };
    },
    _tickGenerator: function(measurement,axis) {
      var min = this._convertValue(measurement,axis.min);
      var max = this._convertValue(measurement,axis.max);
      
      var opts = axis.options;
              
      // estimate number of ticks
      var canvasWidth = this._chartContainer.width();
      var canvasHeight = this._chartContainer.height();
      var noTicks;
      if (typeof opts.ticks == 'number' && opts.ticks > 0) {
          noTicks = opts.ticks;
      } else {
        // heuristic based on the model a*sqrt(x) fitted to some data points that seemed reasonable
        noTicks = 0.3 * Math.sqrt(axis.direction == 'x' ? canvasWidth : canvasHeight);
      }

      var delta = (max - min) / noTicks, size, generator, unit, formatter, i, magn, norm;
      // pretty rounding of base-10 numbers
      var maxDec = opts.tickDecimals;
      var dec = -Math.floor(Math.log(delta) / Math.LN10);
      if (maxDec != null && dec > maxDec) dec = maxDec;

      magn = Math.pow(10, -dec);
      norm = delta / magn; // norm is between 1.0 and 10.0
      
      if (norm < 1.5) {
        size = 1;
      } else if (norm < 3) {
        size = 2;
        // special case for 2.5, requires an extra decimal
        if (norm > 2.25 && (maxDec == null || dec + 1 <= maxDec)) {
          size = 2.5;
          ++dec;
        }
      } else if (norm < 7.5) {
        size = 5;
      } else {
        size = 10;
      }
      
      size *= magn;
      
      if (opts.minTickSize != null && size < opts.minTickSize) size = opts.minTickSize;
        
      axis.tickDecimals = Math.max(0, maxDec != null ? maxDec : dec);
      axis.tickSize = opts.tickSize || size;

      var ticks = [];

      // spew out all possible ticks        
      var start = axis.tickSize * Math.floor(min / axis.tickSize),i = 0, v = Number.NaN, prev;
      do {
        prev = v;
        v = start + i * axis.tickSize;
        ticks.push(this._convertValueInvert(measurement,v));
        ++i;
      } while (v < max && v != prev);
      return ticks;
    },
    _tickFormatterTime: function (value, axis) {
			sign = value < 0 ? '-' : '';
      // TODO: Handle scenario where axis is zoomed in sufficiently to show
      // fractions of seconds. For now, just handle whole seconds
			value = Math.abs(value);
			var hours = Math.floor(value/3600);
			value -= hours*3600;
			var minutes = Math.floor(value/60);
			value -= minutes*60;
      var mult = 1;
      if (axis.tickDecimals > 0) mult = Math.pow(10,axis.tickDecimals);
			var seconds_fraction = Math.round(value*mult)/mult;
      var seconds = Math.floor(seconds_fraction);
      seconds_fraction -= seconds;
			value -= seconds;
      if (seconds >= 60) {
        seconds -= 60;
        minutes++;
        if (minutes >= 60) {
          minutes -= 60;
          hours++;
        }
      }
			
			var hour_digits = 1;
		  var minute_digits = 2;
			var second_digits = 2;
		  hours = (hours > 0) ? this._zeroPad(hours,hour_digits)+':' : '';
			minutes = (minutes >= 0 || hours >= 0) ? this._zeroPad(minutes,minute_digits)+':' : '';
			seconds = (seconds >= 0 || minutes >= 0 || hours >= 0) ? this._zeroPad(seconds,second_digits) : '';
      seconds_fraction = (axis.tickDecimals > 0) ? seconds_fraction.toFixed(axis.tickDecimals).substr(1) : '';
			return sign + hours + minutes + seconds + seconds_fraction;
		},
    _zeroPad: function(n,l){n+='';while(n.length<l)n='0'+n;return n},
    _convertValue: function(measurement,value) {
      if (measurement == null) return value;
      if (measurement.f != 0 && measurement.f != 1) value /= measurement.f;
      value += measurement.o;
      return value;
    },
    _convertValueInvert: function(measurement,value) {
      if (measurement == null) return value;
      if (measurement.f != 0 && measurement.f != 1) value *= measurement.f;
      value -= measurement.o;
      return value;
    },
    _getMetricData: function() {
      var opts = this.options;
      var dataTrack = opts.dataTracks[opts.metric];
      if (dataTrack == null) return {data:null,min:0,max:2,avg:1};
      
      var dataTrackColor = dataTrack.color;
        // TODO: Cache data array for each track
      if (dataTrack['function'] != null) {
        var derived = this._getDerivedMetricData(dataTrack);
        var chartPoints = derived.chartPoints;
        var dataTrack = derived.dataTrack;
        if (chartPoints == null || dataTrack == null) return {data:null,min:0,max:2,avg:1};

      } else {
        var dataPoints = dataTrack.data;
        var numDataPoints = dataPoints != null ? dataPoints.length : 0;
        var numChartPoints = 2+numDataPoints;
        var lastChartPoint = numChartPoints-1;
        
        // Calculate y values from selected tack
        var chartPoints = new Array(numChartPoints);        
        chartPoints[0] = [null,dataTrack.start[1]];
        chartPoints[lastChartPoint] = [null,dataTrack.end[1]];
        for (var i = 0; i < numDataPoints; i++) chartPoints[i+1] = [null,dataPoints[i]];
      }
      var numDataPoints = dataTrack.data != null ? dataTrack.data.length : 0;
      var numChartPoints = chartPoints.length;
      var lastChartPoint = numChartPoints-1;

      // Calculate x values from time or distance track
      var interval = dataTrack.interval;
      var startInterval = Math.floor(dataTrack.start[0]/interval)+1;
      if (opts.xAxis == 'distance' && opts.dataTracks['distance'] != null) {
        var distanceTrack = opts.dataTracks['distance'];
        var numDistanceTrackPoints = distanceTrack.data.length;
        chartPoints[0][0] = this._getValueAtTime(distanceTrack,dataTrack.start[0]);
        chartPoints[lastChartPoint][0] = this._getValueAtTime(distanceTrack,dataTrack.end[0]);
        // TODO: If we are really paranoid we should assert 
        // distanceTrack.interval==dataTrack.interval and fall back to point interpolation
        // when intervals don't match. For now, the backend always returns identical intervals
        var distanceIntervalOffset = startInterval-Math.floor(distanceTrack.start[0]/distanceTrack.interval)+1;
        for (var i = 0; i < numDataPoints; i++) {
          var distanceIndex = distanceIntervalOffset+i;
          var distance;
          if (distanceIndex < 0) {
            distance = chartPoints[0][0];
          } else if (distanceIndex < numDistanceTrackPoints) {
            distance = distanceTrack.data[distanceIndex];
          } else {
            distance = chartPoints[lastChartPoint][0];
          }
          chartPoints[i+1][0] = distance;
        }
      } else {
        chartPoints[0][0] = dataTrack.start[0];
        chartPoints[lastChartPoint][0] = dataTrack.end[0];
        var time = startInterval*interval;
        for (var i = 0; i < numDataPoints; i++) {
          chartPoints[i+1][0] = time;
          time += interval;
        }
      }
      
      // Calculate min,max y values
      var zoomRange = this._zoomRange;
      var min = +Infinity;
      var max = -Infinity;
      var avg = 0;
      var avgTime = 0;
      var x,y;
      var addedFirst = false, addedLast = false;
      for (var i = 0; i < numChartPoints; i++) {        
        y = chartPoints[i][1];
        if (i > 0) {
          var t = chartPoints[i][0]-chartPoints[i-1][0];
          avgTime += t;
          avg += t*y;
        }
        if (zoomRange != null) {
          x = chartPoints[i][0];
          if (x < zoomRange[0]) continue;
          if (!addedFirst) {
            // Add the point immediately before zoomRange start.
            addedFirst = true;
            if (i > 0) {
              y = chartPoints[i-1][1];
              if (y < min) min = y;
              if (y > max) max = y;
            }
          }
          if (x > zoomRange[1]) {
            if (!addedLast) {
              // Add the point immediately after zoomRange end.
              addedLast = true;
              if (i < numChartPoints-1) {
                y = chartPoints[i+1][1];
                if (y < min) min = y;
                if (y > max) max = y;
              }
            }
            continue;
          }
        }
        if (y < min) min = y;
        if (y > max) max = y;
      }
      if (min == +Infinity) min = 0;
      if (max == -Infinity) max = 0;
      if (avgTime > 0) avg /= avgTime;
            
      var trackRGB = this._parseColor(dataTrackColor);
      var lineColor = 'rgba(' + trackRGB.r + ',' + trackRGB.g + ',' + trackRGB.b + ',0.7)';
      var fillColor = 'rgba(' + trackRGB.r + ',' + trackRGB.g + ',' + trackRGB.b + ',0.2)';
      
      return {
        data:{color:lineColor,lines:{fill:true,fillColor:fillColor},data:chartPoints},
        min:min,max:max,avg:avg
      };
    },
    _getDerivedMetricData: function(dataTrack) {
      if (dataTrack['function'] == null) return {};
      return this[dataTrack['function']](dataTrack); 
    },
    _getSpeedTrack: function(dataTrack) {
      var track = this.options.dataTracks['distance'];
      var numDataPoints = track.data != null ? track.data.length : 0;
      var interval = track.interval;
      var startInterval = Math.floor(track.start[0]/interval)+1;
      var time = startInterval*interval;
      
      var points = new Array(numDataPoints+2);
      var empty = true;
      var prevPt = track.start;
      var p;
      var lookback = Math.ceil(10/interval); // 10 smoothing seconds
      for (var i = 0; i < numDataPoints; i++) {
        p = [time,track.data[i]];
        var speed = (p[0] > prevPt[0]) ? Math.max(0,(p[1]-prevPt[1])/(p[0]-prevPt[0])) : null;
        if (speed != null) speed = Math.round(speed*100)/100;
        var speedPt = [time,speed];
        if (empty) { points[0] = [track.start[0],speed]; empty = false; }
        points[i+1] = speedPt;
        var prevIndex = Math.max(0,i-lookback);
        prevPt = [time-(interval*(i-prevIndex)),track.data[prevIndex]];
        time += interval;
      }
      p = track.end;
      var speed = p[0] > prevPt[0] ? Math.max(0,(p[1]-prevPt[1])/(p[0]-prevPt[0])) : null;
      if (speed != null) speed = Math.round(speed*100)/100;
      var speedPt = [p[0],speed];
      if (empty) { points[0] = [track.start[0],speed]; empty = false; }
      points[numDataPoints+1] = speedPt;

      return {dataTrack:track,chartPoints:points};
    },
    _getPaceTrack: function(dataTrack) {
      var speedTrack = this._getSpeedTrack();
      var measurement = dataTrack.measurement;
      var points = speedTrack.chartPoints;
      var numPoints = points.length;
      for (var i = 0; i < numPoints; i++) {
        var speed = points[i][1];
        if (measurement != null) {
          if (measurement.f != 0 && measurement.f != 1) speed /= measurement.f;
          speed += measurement.o;
        }
        points[i][1] = speed == 0 ? null : Math.round(1/speed);
      }
      
      return {dataTrack:speedTrack.dataTrack,chartPoints:points};
    },
    _getValueAtTime: function(dataTrack, time) {
      if (time <= dataTrack.start[0]) return dataTrack.start[1];
      if (time >= dataTrack.end[0]) return dataTrack.end[1];
      
      var numDataPoints = dataTrack.data != null ? dataTrack.data.length : 0;
      var p1,p2;
      if (numDataPoints == 0) {
        p1 = dataTrack.start;
        p2 = dataTrack.end;
      } else {
        var interval = dataTrack.interval;
        var firstPointInterval = Math.floor(dataTrack.start[0]/interval)+1;
        var firstPointTime = firstPointInterval*interval;
        var lastPointTime = firstPointTime+((numDataPoints-1)*interval);
        if (time <= firstPointTime) { // Between start and first
          p1 = dataTrack.start;
          p2 = [firstPointTime,dataTrack.data[0]];
        } else if (time >= lastPointTime) { // Between last and end
          p1 = [lastPointTime,dataTrack.data[numDataPoints-1]];
          p2 = dataTrack.end;
        } else { // Between two points
          var intervalTime = time-firstPointTime;
          var i1 = Math.floor(intervalTime/interval);
          var i2 = Math.ceil(intervalTime/interval);
          p1 = [firstPointTime+i1*interval,dataTrack.data[i1]];
          p2 = [firstPointTime+i2*interval,dataTrack.data[i2]];
        }
      }
      return this._getInterpolatedValue(p1,p2,time);
    },
    _getTimeAtDistance: function(distance) {
      var dataTrack = this.options.dataTracks['distance'];
      if (dataTrack == null) return 0;
      
      if (distance <= dataTrack.start[1]) return dataTrack.start[0];
      if (distance >= dataTrack.end[1]) return dataTrack.end[0];
      
      var numDataPoints = dataTrack.data != null ? dataTrack.data.length : null;
      var p1,p2;
      if (numDataPoints == 0) {
        p1 = dataTrack.start;
        p2 = dataTrack.end;
      } else {
        var interval = dataTrack.interval;
        var firstPointInterval = Math.floor(dataTrack.start[0]/interval)+1;
        var firstPointTime = firstPointInterval*interval;
        var lastPointTime = firstPointTime+((numDataPoints-1)*interval);
        if (distance < dataTrack.data[0]) { // Between start and first
          p1 = dataTrack.start;
          p2 = [firstPointTime,dataTrack.data[0]];
        } else if (distance >= dataTrack.data[numDataPoints-1]) { // Between last and end
          p1 = [lastPointTime,dataTrack.data[numDataPoints-1]];
          p2 = dataTrack.end;
        } else { // Between two points
          var pos = this._binarySearchY(dataTrack.data,distance);
          if (pos == -1) return null;
          if (dataTrack.data[pos] == distance) {
            return firstPointTime+pos*interval;
          } else {
            var i1,i2;
            if (dataTrack.data[pos] > distance) {
              i1 = pos-1;
              i2 = pos;
            } else {
              i1 = pos;
              i2 = pos+1;
            }
            p1 = [firstPointTime+i1*interval,dataTrack.data[i1]];
            p2 = [firstPointTime+i2*interval,dataTrack.data[i2]];
          }
        }
      }
      p1 = [p1[1],p1[0]];
      p2 = [p2[1],p2[0]];
      return this._getInterpolatedValue(p1,p2,distance);
    },
    _binarySearchY: function(array,needle) {  
      if (!array.length) return -1;  

      var high = array.length - 1,low = 0;
      var mid, element;

      while (low <= high) {  
        mid = parseInt((low + high) / 2)  
        element = array[mid];  
        if (element > needle) {  
          high = mid - 1;  
        } else if (element < needle) {  
          low = mid + 1;  
        } else {  
          return mid;  
        }  
      }  

      return mid;  
    },
    _getInterpolatedValue: function(p1,p2,x) {
      if (p1[0] == p2[0] || p1[1] == p2[1]) return p1[1];
      if (x == p1[0]) return p1[1];
      if (x == p2[0]) return p2[1];
      var d = p2[0]-p1[0];
      var s = (p2[1]-p1[1])/d;
      return p1[1] + (x-p1[0])*s;
    },
    _setZoomRange: function(range) {
      this._zoomRange = range;
      if (range != null) {
        this._zoomOutButton.removeClass('disabled');
      } else {
        this._zoomOutButton.addClass('disabled');
      }
    },
    _onXAxisChanged: function(selected) {
      this.setXAxis(selected);
    },
    _onMetricChanged: function(selected) {
      this.setMetric(selected);
    },
    _onZoomOutClicked: function() {
      this._setZoomRange(null);
      this._refreshChartData();
    },
    _onChartSelectionChanged: function(from,to) {
      var opts = this.options;
      var z = this._zoomRange;
      if (z == null || z[0] != from || z[1] != to) {
        this._setZoomRange([parseFloat(from),parseFloat(to)]);
        this._refreshChartData();
      }
    },
    _parseColor: function(hex) {
      var r, g, b;
      if (hex.substring(0, 1) == "#") hex = hex.substring(1);
      if (hex.length == 3) {
        r = hex.substring(0, 1); r = r + r;
        g = hex.substring(1, 2); g = g + g;
        b = hex.substring(2, 3); b = b + b;
      } else if (hex.length == 6) {
        r = hex.substring(0, 2);
        g = hex.substring(2, 4);
        b = hex.substring(4, 6);
      }
      r = parseInt(r, 16);
      g = parseInt(g, 16);
      b = parseInt(b, 16);
      return {r:r,g:g,b:b};
    }
  };
  $.widget("ui.openfit_activitydetailchart", ActivityDetailChart);
})(jQuery);
