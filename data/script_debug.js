// Autor: Julian Schweizerhof

//////////////////////////////// Variablen /////////////////////////////////////////////////////////////////////////

var ws = null;                  // Variable für den Websocket
var flagAutoConnect = false;    // Varialbe für die AutoConnect Funktion false = deaktiviert, true = aktiviert
var flagFirstCall = true;       // Variable für den ersten Aufruf true = erster Aufruf, false = nicht erster Aufruf
var maxDataPoints = 100;        // maximal angezeigte Daten im Graph
var csvFile = "Messwert; Datum; Uhrzeit; TOF1; TOF2\n";
var indexMesswert = 1;
var pause = false;

var dataPlotTOF = new Chart(document.getElementById('myChartTOF').getContext('2d'), {
    type: 'line', // bar, horizontalBar, pie, line, doughnut, radar, polarArea
    data: {
        labels: [],
        datasets: [{
            label: 'Abstand von TOF 1',
            data: [

            ],
            borderColor: "#3e95cd",
            fill: false
        },
        {
            label: 'Abstand von TOF 2',
            data: [
            ],
            borderColor: "#ffab42",
            fill: false
        }]
    },
    options: {
        scales: {
            xAxes: [{
                type: 'time',
                time: {
                    displayFormats: {
                        millisecond: 'mm:ss:SSS'
                    }
                }
            }],
            yAxes: [{
                ticks: {
                    beginAtZero: true
                },
                scaleLabel: {
                    display: true,
                    labelString: "Abstand in mm"
                }
            }],
        },
        title: {
            display: true,
            text: 'Rohwerte der TOF-Sensoren',
            fontSize: 20
        },
        legend: {
            display: true
        },
        tooltips: {
            enabled: true,
            mode: 'index',
            intersect: false, // Tooltip immer anzeigen
            position: 'nearest'
        }
    }
});

var dataPlot1 = new Chart(document.getElementById("myChart1").getContext('2d'), {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            data: [],
            label: "Graph 1",
            borderColor: "#3e95cd",
            fill: false
        }]
    },
    options: {
        title: {
            display: true,
            text: "Graph 1",
            fontSize: 20
        }
    }
});

var dataPlot2 = new Chart(document.getElementById("myChart2").getContext('2d'), {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            data: [],
            label: "Graph 2",
            borderColor: "#3e95cd",
            fill: false
        }]
    },
    options: {
        title: {
            display: true,
            text: "Graph 2",
            fontSize: 20
        }
    }
});

//////////////////////////////// Funktionen /////////////////////////////////////////////////////////////////////////

window.onload = function () {     // Wird nach dem Laden der Webseite aufgerufen
    openWebsocket();            // Websocket starten
    //window.setInterval(updateGraph,1000);
}

function openWebsocket() {
    ws = new WebSocket("ws://" + document.location.hostname + "/ws");   // Erzeugen eines neuen WebSocket unter der Adresse ws:// (ip-Adresse der Verbindung) /ws
    console.log("OpenWebsocket");

    ws.onopen = function () {        // Was soll passieren, wenn eine Verbindung hergestellt wird?
        console.log("ws.onopen");
        //clearInterval(flagAutoConnect); // Autoconnect-Funktion deaktivieren
        //flagAutoConnect = false;    // Autoconnect Flag reseten   
        // Leere Nachricht schicken:
    }

    ws.onclose = function (error) {       // Was soll passieren, wenn die Verbindung geschlossen wird?
        console.log('Socket is closed. Reconnect will be attempted in 5 second.', error.reason);
        setTimeout(function () {
            openWebsocket();
        }, 5000);
    }

    ws.onmessage = function (evt) {  // was soll passieren, wenn eine Nachricht kommt
        var message = JSON.parse(evt.data); // ["personenzahlMax"], ["personenzahlAktuell"], ["akkustand"], ["energiesparmodus"], ["flagGetTime"]
        console.log(message);   // debug
        if (message.hasOwnProperty("debug_msg")) {
            printMessage(message.debug_msg);
        }
        if (message.hasOwnProperty("tof1")) {
            if (!pause) {
                //addData(dataPlotTOF, getTime(), [message.tof1, message.tof2]);
                addData(dataPlotTOF, new Date(), [message.tof1, message.tof2]);
            }
            saveToFile(message.tof1, message.tof2);
        }
    }

    ws.onerror = function (error) {  // was soll passieren, wenn ein Fehler auftritt?
        console.log("Fehler: ", error, " ", error.message, " Websocket wird geschlossen");
        ws.close();
    }
}

function saveToFile(messwert1, messwert2) {
    var now = new Date();
    csvFile += String(indexMesswert) + ";" +  getTimeStamp() + ";" + String(messwert1) + ";" + String(messwert2) + "\n";
    indexMesswert++;
    // csvFile = "Messwert; Datum; Uhrzeit; TOF1; TOF2\n";
}

function getTime() {
    var now = new Date();
    var h = now.getHours();
    var m = now.getMinutes();
    var s = now.getSeconds();
    m = checkTime(m);
    s = checkTime(s);
    return h + ":" + m + ":" + s + "\t";
}

function getDateTimeWithline() {
    var now = new Date();
    var d = now.toISOString().substring(0,10);
    var h = now.getHours();
    var m = now.getMinutes();
    var s = now.getSeconds();
    m = checkTime(m);
    s = checkTime(s);
    return d + "_" + h + "-" + m + "-" + s;
}

function getTimeStamp() {
    var now = new Date();
    var d = now.toLocaleDateString();
    var t = now.toLocaleTimeString();
    var ms = now.getMilliseconds();
    if (ms < 10) {
        ms = "00" + ms;
    } else if (ms < 100) {
        ms = "0" + ms;
    }
    return d + ";" + t + "," + ms;
}

function checkTime(i) {
    if (i < 10) {
        i = "0" + i;
    }
    return i;
}

function printMessage(msg) {
    var textarea = document.getElementById("debug_messages_text");
    textarea.scrollTop = textarea.scrollHeight;
    textarea.value += getTime() + msg + "\n";
}

function addData(chart, label, data) {
    if (chart.data.labels.length > maxDataPoints) removeData(chart);
    chart.data.labels.push(label);
    chart.data.datasets.forEach((dataset, i) => {
        //console.log(dataset, i); // debug
        if (Array.isArray(data)) {
            dataset.data.push(data[i]);
        } else {
            dataset.data.push(data)
        }
    });
    chart.update();
}

function removeData(chart) {
    chart.data.labels.shift();
    chart.data.datasets.forEach((dataset) => {
        dataset.data.shift();
    });
    //chart.update();
}
var x = 10;
var y
function button() {
    printMessage("Hallo");
    addData(dataPlot1, getTime(), 77);
    addData(dataPlot2, getTime(), 77);
    if (x == 10) {
        y = window.setInterval(addElement, 200);
        x = 0;
    }
    else {
        window.clearInterval(y);
        x = 10;
    }


    var now = new Date();
    /*console.log("toISOString\t" + now.toISOString());
    console.log("toLocaleTimeString\t" + now.toLocaleTimeString());
    console.log("toLocaleDateString\t" + now.toLocaleDateString());
    console.log("toLocaleString\t" + now.toLocaleString());
    console.log("valueOf\t" + now.valueOf())
    console.log("toString\t" + now.toString());
    console.log("get Millis" + now.getMilliseconds());*/
    saveToFile(Math.floor(Math.random() * 20), Math.floor(Math.random() * 20))

    //csvFile += String(now.toISOString()) + ";" + String(Math.floor(Math.random() * 20)) + ";" + String(Math.floor(Math.random() * 20)) + "\n";
}
function addElement() {
    if (dataPlotTOF.data.labels.length > maxDataPoints) removeData(dataPlotTOF);
    addData(dataPlotTOF, new Date(), [Math.floor(Math.random() * 20) + 10, Math.floor(Math.random() * 10) + 10]);
}

const downloadToFile = (content, filename, contentType) => {
    const a = document.createElement('a');
    const file = new Blob([content], { type: contentType });

    a.href = URL.createObjectURL(file);
    a.download = filename;
    a.click();

    URL.revokeObjectURL(a.href);
};

document.querySelector('#downloadCSV').addEventListener('click', () => {
    var now = new Date();
    downloadToFile(csvFile,  getDateTimeWithline() + '_sensor_measurement.csv', 'text/comma-separated-values');
});

function buttonLoeschen() {
    deleteData(dataPlotTOF);
    deleteData(dataPlot1);
    deleteData(dataPlot2);
}

function buttonPause() {
    pause = !pause;
    if(pause) {
        document.getElementById("pause").innerText = "Diagramm pausiert";
    } else {
        document.getElementById("pause").innerText = "Diagramm pausieren";
    }
}

function deleteData(chart) {
    chart.data.labels = [];
    chart.data.datasets.forEach((dataset) => {
        dataset.data = [];
    });
    chart.update();
}
/*function updateGraph() {
    dataPlotTOF.update();
    console.log("update");
}*/
/*

var newDataset = {
        label: "Party",
        backgroundColor: 'rgba(99, 255, 132, 0.2)',
        borderColor: 'rgba(99, 255, 132, 1)',
        borderWidth: 1,
        data: [10, 20, 30, 40, 50, 60, 70],
    }
    massPopChart.data.datasets.push(newDataset);
    massPopChart.update();

*/