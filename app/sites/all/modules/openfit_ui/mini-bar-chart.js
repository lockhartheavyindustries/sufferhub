function MiniBarChart(div) {
  this.bars = {data:[],color:'#3376EA',radius:4,spacing:4};
  this.xAxis = {labels:{show:true,color:'#B0B0B0',fontSize:13,fontFace:'Arial'}};
  this.canvas = document.createElement("canvas");
  this.canvas.style.width = "100%";
  this.canvas.style.height = "100%";
  div.appendChild(this.canvas);
  this.ctx = this.canvas.getContext ? this.canvas.getContext('2d') : null;
  var me = this;
  window.addEventListener('resize', function() {me.resizeRefresh();}, false);
  
  this.refresh();
}

MiniBarChart.prototype.resizeRefresh = function() {
  if (this.canvas.width != this.canvas.offsetWidth || this.canvas.height != this.canvas.offsetHeight) this.refresh();
}
 
MiniBarChart.prototype.refresh = function() {
    if (!this.ctx) return;
    var width,height;
    width = this.canvas.width = this.canvas.offsetWidth;
    height = this.canvas.height = this.canvas.offsetHeight;
    this.ctx.save();
    this.ctx.clearRect(0,0,width,height);
    
    var barCount = this.bars.data.length;
    if (barCount == 0) return;
    
    var barWidth = parseInt(width/barCount);
    if (barWidth <= 1) return;
    
    var maxValue = null;
    var minValue = null;
    var absMax = null;
    var hasPositive = false;
    var hasNegative = false;
    var hasXAxisLabels = false;
    
    // Collect value range
    for (var i = 0; i < barCount; i++) {
      hasXAxisLabels = hasXAxisLabels || (this.bars.data[i].label != null && this.bars.data[i].label.length > 0);
      var value = this.bars.data[i].value;
      if (value == null) continue;
      if (maxValue == null || value > maxValue) maxValue = value;
      if (minValue == null || value < minValue) minValue = value;
      var absValue = Math.abs(value);
      if (absMax == null || absValue > absMax) absMax = absValue;
      if (value > 0) hasPositive = true;
      if (value < 0) hasNegative = true;
    }
    hasXAxisLabels = hasXAxisLabels && this.xAxis.labels.show;
    
    var barAreaHeight = height;
    if (hasXAxisLabels) barAreaHeight -= (this.xAxis.labels.fontSize+4);

    // Calculate the origin, either at the top, bottom, or middle
    var originPos = 0;
    if (hasPositive) {
      if (hasNegative) {
        originPos = Math.round(barAreaHeight/2) + 0.5;
      } else {
        originPos = barAreaHeight + 0.5;
      }
    } else if (!hasNegative) originPos = barAreaHeight + 0.5;
    
    // Plot the x-axis labels
    if (hasXAxisLabels) {
      this.ctx.font = this.xAxis.labels.fontSize + "px " + this.xAxis.labels.fontFace;
      this.ctx.textAlign = 'center';
      this.ctx.textBaseline = 'bottom';
      this.ctx.fillStyle = this.xAxis.labels.color;
      for (var i = 0; i < barCount; i++) {
        var barLabel = this.bars.data[i].label;
        if (barLabel == null || barLabel.length == 0) continue;
        var x = (i+0.5)*barWidth-(this.bars.spacing/2);
        this.ctx.fillText(barLabel,x,height);
      }
    }
    
    // If there are values, plot them
    var rectWidth = barWidth-this.bars.spacing;
    if (rectWidth > 1 && (hasPositive || hasNegative)) {
      var barValueUnits = (barAreaHeight-1) / absMax;
      if (hasPositive && hasNegative) barValueUnits /= 2;
      
      this.ctx.lineWidth = .5;
      var prevBarColor = '';
      var lighter = '';
      for (var i = 0; i < barCount; i++) {
        var barValue = this.bars.data[i].value;
        if (barValue == null || barValue == 0) continue;
        var pixelHeight = Math.abs(barValue * barValueUnits);
        var x = i*barWidth;
        var y = originPos;
        if (barValue > 0) y -= pixelHeight;
        var barColor = this.bars.color;
        if (this.bars.data[i].color != null) barColor= this.bars.data[i].color;
        if (barColor != prevBarColor) {
          prevBarColor = barColor;
          lighter = this.changeColor(barColor, 0.3, false);
          this.ctx.strokeStyle = this.changeColor(barColor, 0.2, true);
        }
        var gradient = this.ctx.createLinearGradient(x,y,x,y+pixelHeight);
        gradient.addColorStop(0, lighter);
        gradient.addColorStop(1, barColor);
        this.ctx.fillStyle = gradient;
        if (this.bars.radius <= 1) {
          this.ctx.fillRect(x,y,rectWidth,pixelHeight);
          this.ctx.strokeRect(x,y,rectWidth,pixelHeight);
        } else {
          this.ctx.beginPath(); 
          this.ctx.roundRect(x,y,rectWidth,pixelHeight,this.bars.radius);
          this.ctx.closePath();
          this.ctx.fill();
          this.ctx.stroke();
        }
      }
    } else {
      hasPositive = hasNegative = true;
    }
    
    // Draw the origin line
    this.ctx.lineWidth = 1;
    this.ctx.strokeStyle = 'rgba(200,200,200,0.6)';
    this.ctx.beginPath(); 
    this.ctx.moveTo(0,originPos);
    this.ctx.lineTo(width,originPos);
    this.ctx.closePath();
    this.ctx.stroke();

    this.ctx.restore();
};
     
MiniBarChart.prototype.setBarData = function(barData) {
  this.bars.data = [];
  if (barData instanceof Array) {
    var length = barData.length;
    this.bars.data = new Array(length);
    for(var i = 0; i < length; i++) {
      var value = {value:null};
      if (barData[i] instanceof Object) {
        value = barData[i];
      } else {
        value = {value:parseFloat(barData[i])};
      }
      this.bars.data[i] = value;
    }
  } else {
    if (!(typeof barData === "string")) barData = barData.toString();
    barData = barData.split(',');
    var length = barData.length;
    this.bars.data = new Array(length);
    for(var i = 0; i < length; i++) {
      var barValue = barData[i].length > 0 ? parseFloat(barData[i]) : null;
      this.bars.data[i] = {value:barValue};
    }
  }
  this.refresh();
}

MiniBarChart.prototype.pad = function(num, totalChars) { 
  var pad = '0'; 
  num = num + ''; 
  while (num.length < totalChars) num = pad + num;
  return num; 
}; 
 
// Ratio is between 0 and 1 
MiniBarChart.prototype.changeColor = function(color, ratio, darker) { 
  var difference = Math.round(ratio * 255) * (darker ? -1 : 1), 
      minmax = darker ? 0 : 255,
      minmaxfunc = darker ? Math.max : Math.min, 
      decimal = color.replace( 
          /^#?([a-z0-9][a-z0-9])([a-z0-9][a-z0-9])([a-z0-9][a-z0-9])/i, 
          function() { 
              return parseInt(arguments[1], 16) + ',' + 
                  parseInt(arguments[2], 16) + ',' + 
                  parseInt(arguments[3], 16); 
          } 
      ).split(/,/); 
  return [ 
      '#', 
      this.pad(minmaxfunc(parseInt(decimal[0], 10) + difference, minmax).toString(16), 2), 
      this.pad(minmaxfunc(parseInt(decimal[1], 10) + difference, minmax).toString(16), 2), 
      this.pad(minmaxfunc(parseInt(decimal[2], 10) + difference, minmax).toString(16), 2) 
  ].join(''); 
}; 

CanvasRenderingContext2D.prototype.roundRect = function (x, y, w, h, r) {
  // lineTo should not be necessary, but fixes an IE 9 bug      
  if (w < 2 * r) r = w / 2; 
  if (h < 2 * r) r = h / 2; 
  this.moveTo(x + r, y);
  this.arcTo(x + w, y, x + w, y + r, r);
  this.lineTo(x + w, y + r);
  this.arcTo(x + w, y + h, x + w - r, y + h, r);
  this.lineTo(x + w - r, y + h);
  this.arcTo(x, y + h, x, y + h - r, r);
  this.lineTo(x, y + h - r);
  this.arcTo(x, y, x + r, y, r);
  return this; 
} 
