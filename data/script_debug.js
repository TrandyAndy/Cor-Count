// Autor: Julian Schweizerhof

//////////////////////////////// Variablen /////////////////////////////////////////////////////////////////////////

var ws = null;                  // Variable für den Websocket
var flagAutoConnect = false;    // Varialbe für die AutoConnect Funktion false = deaktiviert, true = aktiviert
var flagFirstCall = true;       // Variable für den ersten Aufruf true = erster Aufruf, false = nicht erster Aufruf
var dataPlot;
var maxDataPoints = 100;         // maximal angezeigte Daten im Graph

//////////////////////////////// Funktionen /////////////////////////////////////////////////////////////////////////

window.onload = function(){     // Wird nach dem Laden der Webseite aufgerufen
    openWebsocket();            // Websocket starten
    dataPlot = new Chart(document.getElementById("line-chart"), {
        type: 'line',
        data: {
          labels: [],
          datasets: [{
            data: [],
            label: "TOF 1",
            borderColor: "#3e95cd",
            fill: false
          }]
        }
    });
    dataPlot2 = new Chart(document.getElementById("line-chart2"), {
        type: 'line',
        data: {
          labels: [],
          datasets: [{
            data: [],
            label: "TOF 2",
            borderColor: "#3e95cd",
            fill: false
          }]
        }
    });
}

function openWebsocket(){
    ws = new WebSocket("ws://" + document.location.hostname + "/ws");   // Erzeugen eines neuen WebSocket unter der Adresse ws:// (ip-Adresse der Verbindung) /ws
    console.log("OpenWebsocket");

    ws.onopen = function() {        // Was soll passieren, wenn eine Verbindung hergestellt wird?
        console.log("ws.onopen");
        //clearInterval(flagAutoConnect); // Autoconnect-Funktion deaktivieren
        //flagAutoConnect = false;    // Autoconnect Flag reseten   
        // Leere Nachricht schicken:
    }

    ws.onclose = function(error) {       // Was soll passieren, wenn die Verbindung geschlossen wird?
        console.log('Socket is closed. Reconnect will be attempted in 5 second.', error.reason);
        setTimeout(function() {
            openWebsocket();
        }, 5000);
    }

    ws.onmessage = function(evt) {  // was soll passieren, wenn eine Nachricht kommt
        var message = JSON.parse(evt.data); // ["personenzahlMax"], ["personenzahlAktuell"], ["akkustand"], ["energiesparmodus"], ["flagGetTime"]
        console.log(message);   // debug
        if (message.hasOwnProperty("debug_msg")) {
            printMessage(message.debug_msg);
        }
        if (message.hasOwnProperty("tof1")) {
            addData(dataPlot, getTime(), message.tof1);
        }
        if (message.hasOwnProperty("tof2")) {
            addData(dataPlot2, getTime(), message.tof2);
        }
    }         

    ws.onerror = function(error) {  // was soll passieren, wenn ein Fehler auftritt?
        console.log("Fehler: ", error, " ", error.message, " Websocket wird geschlossen");
        ws.close();
    }
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

function checkTime(i) {
    if (i < 10) {
        i = "0" + i;
    }
    return i;
}

function printMessage(msg) {
    var textarea = document.getElementById("debug_messages_text");
    textarea.scrollTop = textarea.scrollHeight;
    textarea.value +=  getTime() + msg + "\n";
}

function addData(chart, label, data) {
    if(chart.data.labels.length > maxDataPoints) removeData(chart);
    chart.data.labels.push(label);
    chart.data.datasets[0].data.push(data);
    chart.update();
}
function removeData(chart){
    chart.data.labels.shift();
    chart.data.datasets[0].data.shift();
  }

function button() {
    printMessage("Hallo");
    addData(dataPlot ,getTime(),77);
    addData(dataPlot2, getTime(),77);
}