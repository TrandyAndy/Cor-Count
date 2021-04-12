// Autor: Julian Schweizerhof

//////////////////////////////// Variablen /////////////////////////////////////////////////////////////////////////

var ws = null;                  // Variable für den Websocket
var flagAutoConnect = false;    // Varialbe für die AutoConnect Funktion false = deaktiviert, true = aktiviert
var flagFirstCall = true;       // Variable für den ersten Aufruf true = erster Aufruf, false = nicht erster Aufruf
var personenzahlMax = 12;       // globale Variable für die maximale Personenzahl
var personenzahlAktuell = 0;    // globale Variable für die aktuelle Personenzahl
var akkustand = 0;            // globale Variable für den Akkustand
var energiesparmodus = true;    // globale Variable für den Energiesparmodus
var flagGetTime = false;        // globale Variable ob die Zeit bei Personenerkennung geschickt werden soll.
var maxSliderValue = 100;       // maximal mögliche Personenzahl für die Schieberegler
// Objekt für die JSON Übertragung Webseite --> Server auf ESP32: 
    // personenzahlAktuell = eingstellte aktuelle Personenzahl über dem Schieberelger
    // personenzahlMax = eingestellte maximale Personenzahl über dem Schieberelger
    // dateTime = aktuelles Datum und Uhrzeit für die Statistik, Format: "Thu Dec 31 2020 12:22:02 GMT+0100 (CET)""
    // energiesparmodus: Status des Energiesparmodus: true = aktiv, false = deaktiviert
var jsonData = {personenzahlAktuell: 0, personenzahlMax: 0, dateTime: " ", energiesparmodus: false};

//////////////////////////////// Funktionen /////////////////////////////////////////////////////////////////////////

window.onload = function(){     // Wird nach dem Laden der Webseite aufgerufen
    // Slider auf maximale Werte einstellen
    document.getElementById("personenzahlMax").max = maxSliderValue;
    document.getElementById("personenzahlAktuell").max = maxSliderValue;
    // Labels der Slider aktualisieren
    var wert;                   // Temporäre Variable für den Slider Wert
    wert = document.getElementById("personenzahlMax").value;
    document.getElementById("einstellungen_personenzahlMax_label").innerHTML = wert;
    wert = document.getElementById("personenzahlAktuell").value;
    document.getElementById("einstellungen_personenzahlAktuell_label").innerHTML = wert;
    openWebsocket();            // Websocket starten
    aktualisiereAnzeige();
    sendDataToServer(true);
}

function openWebsocket(){
    ws = new WebSocket("ws://" + document.location.hostname + "/ws");   // Erzeugen eines neuen WebSocket unter der Adresse ws:// (ip-Adresse der Verbindung) /ws
    console.log("OpenWebsocket");

    ws.onopen = function() {        // Was soll passieren, wenn eine Verbindung hergestellt wird?
        console.log("ws.onopen");
        //clearInterval(flagAutoConnect); // Autoconnect-Funktion deaktivieren
        //flagAutoConnect = false;    // Autoconnect Flag reseten   
        document.getElementById("einstellungen_status_wlan").innerHTML = "WIFI-Status: Verbunden";
        document.getElementById("einstellungen_status_wlan").style.color = "#2ECC40";
        // Leere Nachricht schicken:
        jsonData.energiesparmodus = " ";
        var nachricht = JSON.stringify(jsonData);
        console.log(nachricht); 
        ws.send(nachricht);
    }

    ws.onclose = function(error) {       // Was soll passieren, wenn die Verbindung geschlossen wird?
        console.log('Socket is closed. Reconnect will be attempted in 5 second.', error.reason);
        setTimeout(function() {
        openWebsocket();
        }, 5000);
        /*
        if(flagAutoConnect == false)    // wenn die Autoconnect-Funktion noch nicht aktiviert ist
        {
            openWebsocket();    // autoConnect nicht mehr notwendig, reicht so aus. Mmh macht manchmal Endlosschleifen
            flagAutoConnect = setInterval(openWebsocket, 10000); // aktiviere die Autoconnect-Funktion, diese wird alle 60 Sekunden aufgerufen
            
            console.log(flagAutoConnect);                       
        }
        */
        document.getElementById("einstellungen_status_wlan").innerHTML = "WIFI-Status: Nicht verbunden";
        document.getElementById("einstellungen_status_wlan").style.color = "#FFDC00";
    }

    ws.onmessage = function(evt) {  // was soll passieren, wenn eine Nachricht kommt
        var message = JSON.parse(evt.data); // ["personenzahlMax"], ["personenzahlAktuell"], ["akkustand"], ["energiesparmodus"], ["flagGetTime"]
        console.log(message);   // debug
        // geschickte Werte laden
        personenzahlMax = message.personenzahlMax;
        personenzahlAktuell = message.personenzahlAktuell;
        akkustand = message.akkustand;
        if (akkustand > 100) { // wenn Akkustand größer 100 wird das begrenzt
            akkustand = 100;
        }
        energiesparmodus = message.energiesparmodus;
        flagGetTime = message.flagGetTime;
        if(flagFirstCall)   // nur beim ersten Aufruf aufrufen:
        {
            flagFirstCall = false;
        }
        else if(flagGetTime) {
            sendDataToServer(false);
            flagGetTime = false;
        }
        aktualisiereAnzeige();
    }         

    ws.onerror = function(error) {  // was soll passieren, wenn ein Fehler auftritt?
        console.log("Fehler: ", error, " ", error.message, " Websocket wird geschlossen");
        ws.close();
    }
} 

function aktualisiereAnzeige() {
    updateSliderPersonenzahlMax(personenzahlMax);
    updateSliderPersonenzahlAktuell(personenzahlAktuell);
    // Anzeige aktualisieren
    document.getElementById("anzeige_hinweis").innerHTML = personenzahlAktuell + " / " + personenzahlMax +" Personen";
    // Akkustand aktualisieren
    document.getElementById("einstellungen_status_batterie_text").innerHTML = "Akkustand: " + akkustand +" %";
    document.getElementById("batteryLevel").style.width = akkustand + "%";
    if (akkustand > 20) {
        document.getElementById("batteryLevel").style.backgroundColor = "#328C46";
    } else {
        document.getElementById("batteryLevel").style.backgroundColor = "#C75151";
    }
    
    // Energiesparmodus 
    document.getElementById("energiesparmodus_checkbox").checked = energiesparmodus;
    // Maximal Personenzahl erreicht
    if(personenzahlAktuell >= personenzahlMax) {
        stop();
    }
    else {
        go();
    }
}

function scrollToTop() {            // Zur 1. Seite "Anzeige" scrollen 
    document.getElementById('wrapper_anzeige').scrollIntoView();
}

function scrollToEinstellungen(){   // Zur 2. Seite "den Einstellungen" scrollen
    document.getElementById('wrapper_einstellungen').scrollIntoView();
}

function scrollToWlanEinstellungen(){   // Zur 3. Seite "den WLAN Einstellungen" scrollen
    document.getElementById('wrapper').scrollIntoView();
}

function button_reset_func(){       // Button Personenzahl resetten gedrückt
    alert("Zähler wurde erfolgreich resettet.");
    document.getElementById("personenzahlAktuell").value = 0;
    document.getElementById("einstellungen_personenzahlAktuell_label").innerHTML = document.getElementById("personenzahlAktuell").value;
    sendDataToServer(true);
}

function updateSliderPersonenzahlMax(wert){    // wird beim Bewegen des Sliders aufgerufen
    //console.log(wert);
    document.getElementById("personenzahlMax").value = wert;
    document.getElementById("einstellungen_personenzahlMax_label").innerHTML = wert;
}
function updateSliderPersonenzahlAktuell(wert){    // wird beim Bewegen des Sliders aufgerufen
    //console.log(wert);
    document.getElementById("personenzahlAktuell").value = wert;
    document.getElementById("einstellungen_personenzahlAktuell_label").innerHTML = wert;
}

function updateSliderRealeasedPersoenzahlMax(wert){   // wird beim Loslassen aufgerufen
    console.log(wert);
    sendDataToServer(true);
}

function updateSliderRealeasedPersonenzahlAktuell(wert){   // wird beim Loslassen aufgerufen
    console.log(wert);
    sendDataToServer(true);
}

function go(){                      // Anzeige auf go ändern
    document.getElementById("anzeige_nachricht").innerHTML = "Bitte eintreten und Abstand halten"
    document.getElementById("anzeige_icon").innerHTML = "🏃‍♀️↔️🏃‍♂️";
    //document.getElementById("anzeige_hinweis").innerHTML = "32 / 120 Personen"
    // Auf die in CSS definierte Farbe zugreifen.
    document.getElementById("wrapper_anzeige").style.backgroundColor = getComputedStyle(document.documentElement).getPropertyValue('--green');  // Grün
}

function stop(){                    // Anzeige auf stop ändern
    document.getElementById("anzeige_nachricht").innerHTML = "Stopp! Bitte warten!"
    document.getElementById("anzeige_icon").innerHTML = "✋";
    //document.getElementById("anzeige_hinweis").innerHTML = "120 / 120 Personen"
    document.getElementById("wrapper_anzeige").style.backgroundColor = getComputedStyle(document.documentElement).getPropertyValue('--red');    // Rot
}

function buttonPersonenzahlMax_min(){   // + beim PersonenzahlMax geklickt
    document.getElementById("personenzahlMax").stepDown(1);
    var wert = document.getElementById("personenzahlMax").value;
    document.getElementById("einstellungen_personenzahlMax_label").innerHTML = wert;
    sendDataToServer(true);
}

function buttonPersonenzahlMax_max(){   // - beim PersonenzahlMax geklickt
    document.getElementById("personenzahlMax").stepUp(1);
    var wert = document.getElementById("personenzahlMax").value;
    document.getElementById("einstellungen_personenzahlMax_label").innerHTML = wert;
    sendDataToServer(true);
}

function buttonPersonenzahlAktuell_min(){   // + beim PersonenzahlAktuell geklickt
    document.getElementById("personenzahlAktuell").stepDown(1);
    var wert = document.getElementById("personenzahlAktuell").value;
    document.getElementById("einstellungen_personenzahlAktuell_label").innerHTML = wert;
    sendDataToServer(true);
}

function buttonPersonenzahlAktuell_max(){   // - beim PersonenzahlAktuell geklickt
    console.log("bye");
    document.getElementById("personenzahlAktuell").stepUp(1);
    var wert = document.getElementById("personenzahlAktuell").value;
    document.getElementById("einstellungen_personenzahlAktuell_label").innerHTML = wert;
    sendDataToServer(true);
}

function energiesparmodus_clicked() // div mit der Checkbox geklickt
{
    //console.log("Energieeeeee");
    if(document.getElementById("energiesparmodus_checkbox").checked){
        document.getElementById("energiesparmodus_checkbox").checked = false;
    }
    else{
        document.getElementById("energiesparmodus_checkbox").checked = true;
    }
    sendDataToServer(true);
}

function sendDataToServer(changedEvent) {
    if(changedEvent) {
        jsonData.personenzahlMax = document.getElementById("personenzahlMax").value;
        jsonData.personenzahlAktuell = document.getElementById("personenzahlAktuell").value;
        jsonData.energiesparmodus = document.getElementById("energiesparmodus_checkbox").checked;
        jsonData.dateTime = " ";
    }
    else {
        var now = new Date();
        jsonData.dateTime = now;    // Achtung Weltzeituhr!!!!
        
    }
    var nachricht = JSON.stringify(jsonData);
    console.log(nachricht); 
    ws.send(nachricht);
}
function buttonSaveAP() {
    document.getElementById("AP_IP").href = "http://www.google.de"
    document.getElementById("AP_IP").innerHTML = "http://www.google.de"
    console.log("tada!");
    //var url = 'https://mdn.github.io/learning-area/javascript/oojs/json/superheroes.jso'
    var url = "data";
    console.log("Die Url ist: ", url);
    fetch(url)
    .then(function (response) {
        //console.log(response);
        //console.log(response.json);
        return response.json();
    })
    .then(function (data) {
        console.log(data);
        console.log(data.flagSTA);
        if (data.flagSTA) 
        {
            document.getElementById("AP_ssid_textbox").value = "Verbunden über STA"
        } 
        else 
        {
            document.getElementById("AP_ssid_textbox").value = "Verbunden über AP"
        }
        
    })
    .catch((error) => {
        console.error('Error:', error);
    });
}
function buttonSaveSTA(){
    document.getElementById("AP_hinweise").innerHTML = "";
    document.getElementById("AP_hinweise").style.height = "0%";
    document.getElementById("STA_hinweise").innerHTML = "";
    document.getElementById("STA_hinweise").style.height = "0%";
}