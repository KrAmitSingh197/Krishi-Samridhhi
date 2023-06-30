// Create the charts when the web page loads
window.addEventListener('load', onload);

function onload(event){
  chartT1 = createTemp1Chart();
  chartT2 = createTemp2Chart();
  chartT3 = createTemp3Chart();
  chartH2 = createHum2Chart();
  chartT4 = createTemp4Chart();
  chartH4 = createHum4Chart();
  
}

// Create Ambient Temp
function createTemp1Chart() {
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-temp1',
      type: 'spline' 
    },
    series: [
      {
        name: 'BME280'
      }
    ],
    title: { 
      text: undefined
    },
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' }
    },
    yAxis: {
      title: { 
        text: 'Ambient Temperature' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}
// Create Temperature Chart
function createTemp2Chart() {
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-temp2',
      type: 'spline' 
    },
    series: [
      {
        name: 'BME280'
      }
    ],
    title: { 
      text: undefined
    },
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' }
    },
    yAxis: {
      title: { 
        text: 'Ambient Humidity' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}
// Create Humidity Chart
function createTemp3Chart(){
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-temp3',
      type: 'spline'  
    },
    series: [{
      name: 'BME280'
    }],
    title: { 
      text: undefined
    },    
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      },
      series: { 
        color: '#50b8b4' 
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' }
    },
    yAxis: {
      title: { 
        text: 'Humidity (%)' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}
// Create Temperature Chart
function createTemp4Chart() {
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-temp4',
      type: 'spline' 
    },
    series: [
      {
        name: 'BME280'
      }
    ],
    title: { 
      text: undefined
    },
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' }
    },
    yAxis: {
      title: { 
        text: 'Soil Temperature' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}
// Create Humidity Chart
function createHum2Chart(){
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-hum2',
      type: 'spline'  
    },
    series: [{
      name: 'BME280'
    }],
    title: { 
      text: undefined
    },    
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      },
      series: { 
        color: '#50b8b4' 
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' }
    },
    yAxis: {
      title: { 
        text: 'Humidity_N_1 (%)' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}
// Create Temperature Chart
function createHum4Chart() {
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-hum4',
      type: 'spline' 
    },
    series: [
      {
        name: 'BME280'
      }
    ],
    title: { 
      text: undefined
    },
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' }
    },
    yAxis: {
      title: { 
        text: 'Humidity_N_2' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}
