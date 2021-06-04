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
var passwordVisibleAP = false;
var passwordVisibleSTA = false;
var isActiveSTA = "";
var AP_hinweise = "Personenzähler erreichbar unter <a href=\"http://counting-cube.local\">counting-cube.local</a> (Windows, macOS und iOS) und unter <a id=\"AP_IP\" href=\"#\">192.168.4.1</a> (Android)."
var STA_hinweise = "Personenzähler erreichbar unter <a href=\"http://counting-cube.local\">counting-cube.local</a> (Windows, macOS und iOS) und unter <a id=\"STA_IP\" href=\"#\">192.168.4.1</a> (Android)."
var ipAP = "", ipSTA = "";

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
    onchangeComboBoxSSID(true);
    openWebsocket();            // Websocket starten
    aktualisiereAnzeige();
    sendDataToServer(true);
}

function openWebsocket(){
    ws = new WebSocket("ws://" + document.location.hostname + "/ws");   // Erzeugen eines neuen WebSocket unter der Adresse ws:// (ip-Adresse der Verbindung) /ws
    console.log("OpenWebsocket");

    ws.onopen = function() {        // Was soll passieren, wenn eine Verbindung hergestellt wird?
        console.log("ws.onopen");
        checkConnection();
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
        if (message.hasOwnProperty("personenzahlMax")) {
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
        else if (message.hasOwnProperty("isActiveSTA")) {
            if (message.isActiveSTA) {
                isActiveSTA = "Ist aktiv";
            }
            else {
                isActiveSTA = "Inaktiv";
            }
            checkConnection();
        }
        else if (message.hasOwnProperty("ssidAP")) {
            document.getElementById("AP_ssid_textbox").value = message.ssidAP;
            document.getElementById("AP_password_textbox").value = message.passwordAP;
            //document.getElementById("STA_ssid_textbox").value = message.ssidSTA;
            addElementToComboBox(message.ssidSTA, "saved");
            document.getElementById("comboBoxSSID").value = message.ssidSTA;
            message.scannedNetworks.forEach(element => {
                addElementToComboBox(element, "scanned");
            });
            document.getElementById("STA_password_textbox").value = message.passwordSTA;
            ipAP = message.ipAP;
            ipSTA = message.ipSTA;
            // document.getElementById("AP_IP").innerHTML = message.ipAP;
            // document.getElementById("AP_IP").href = "http://" + message.ipAP;
            // document.getElementById("STA_IP").innerHTML = message.ipSTA;
            // document.getElementById("STA_IP").href = "http://" + message.ipSTA;
            onchangeComboBoxSSID(true);
        }
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


//////////// WLAN Settings /////////////////
function onclickPasswordAP() {
    var element = document.getElementById("eyeIconAP");
    var textBox = document.getElementById("AP_password_textbox");
    if (passwordVisibleAP) {
        element.classList.remove("fa-eye-slash");
        element.classList.add("fa-eye");
        passwordVisibleAP = false;
        textBox.type = "password";
    }
    else {
        element.classList.remove("fa-eye");
        element.classList.add("fa-eye-slash");
        passwordVisibleAP = true;
        textBox.type = "text";
    }
}

function onclickPasswordSTA() {
    var element = document.getElementById("eyeIconSTA");
    var textBox = document.getElementById("STA_password_textbox");
    if (passwordVisibleSTA) {
        element.classList.remove("fa-eye-slash");
        element.classList.add("fa-eye");
        passwordVisibleSTA = false;
        textBox.type = "password";
    }
    else {
        element.classList.remove("fa-eye");
        element.classList.add("fa-eye-slash");
        passwordVisibleSTA = true;
        textBox.type = "text";
    }


}

function buttonSaveAP() {
    //checkConnection();

    var config = {ssidAP: "", passwordAP: ""}
    config.ssidAP = document.getElementById("AP_ssid_textbox").value;
    config.passwordAP = document.getElementById("AP_password_textbox").value;
    if (config.ssidAP == "") {
        alert("Sie haben keine SSID eingeben.");
        setFocus("AP_ssid_textbox");
    }
    else if (config.passwordAP == "") {
        alert("Sie haben kein Passwort eingeben.");
        setFocus("AP_password_textbox");
    }
    else if (config.passwordAP.length < 8) {     // Zu kurzen Passowert
        alert("Das Passwort ist zu kurz. Es werden mind. 8 Zeichen benötigt.");
        setFocus("AP_password_textbox");
    }
    else if (config.ssidAP.includes("?") || config.ssidAP.includes("\"") || config.ssidAP.includes("$") || config.ssidAP.includes("[") || config.ssidAP.includes("]") || config.ssidAP.includes("+") ) {
        alert("Diese Zeichen sind in der SSID verboten: ? \" $ [ ] +");
        setFocus("AP_ssid_textbox");
    } 
    else {
        var nachricht = JSON.stringify(config);
        console.log(nachricht);
        ws.send(nachricht);
    }
    
    
    
}


function onchangeComboBoxSSID(flag=false) {
    
    console.log("test"); // debug
    var element = document.getElementById("comboBoxSSID")
    var savedNetworks = document.getElementById("groupSavedNetworks");
    var foundNetworks = document.getElementById("groupFoundNetworks");
    /*
    console.log(savedNetworks.label);

    var opt = document.createElement('option');
    opt.innerHTML = 23;
    element.appendChild(opt);
    savedNetworks.appendChild(opt);
    */

    console.log(element.value); // debug
    if(element.value == "Anderes Netzwerk verwenden") {
        document.getElementById("STA_ssid_own").classList.remove("hide")
        document.getElementById("STA_ssid_own").classList.add("show")
        if (flag == false) {
            setTimeout(setFocus.bind(null, "STA_ssid_textbox"), 500); // Textbox aktivieren
        }
        //setFocus("STA_ssid_textbox");   
    }
    else {
        document.getElementById("STA_ssid_own").classList.remove("show")
        document.getElementById("STA_ssid_own").classList.add("hide")
    }
}

function addElementToComboBox(text, type) {
    var savedNetworks = document.getElementById("groupSavedNetworks");
    var foundNetworks = document.getElementById("groupFoundNetworks");
    if (type == "saved") {
        var opt = document.createElement('option');
        opt.innerHTML = text;
        savedNetworks.appendChild(opt)
    } else {
        var opt = document.createElement('option');
        opt.innerHTML = text;
        foundNetworks.appendChild(opt)
    }
}

function setFocus(id) {
    document.getElementById(id).focus();
}

function buttonSaveSTA(){
    var config = {ssidSTA: "", passwordSTA: ""}
    var element = document.getElementById("comboBoxSSID")
    if(element.value == "Anderes Netzwerk verwenden") {
        config.ssidSTA = document.getElementById("STA_ssid_textbox").value;
    }
    else {
        config.ssidSTA = element.value;
    }
    config.passwordSTA = document.getElementById("STA_password_textbox").value;
    var nachricht = JSON.stringify(config);
    console.log(nachricht);
    ws.send(nachricht);
    /*
    document.getElementById("AP_hinweise").innerHTML = "";
    document.getElementById("AP_hinweise").style.height = "0%";
    document.getElementById("STA_hinweise").innerHTML = "";
    document.getElementById("STA_hinweise").style.height = "0%";
    */
}

function checkConnection() {

    //document.getElementById("AP_IP").href = "http://www.google.de"
    //document.getElementById("AP_IP").innerHTML = "http://www.google.de"
    //console.log("tada!");
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
        console.log(data.flagConnection);
        if (data.flagConnection == "STA") 
        {
            document.getElementById("STA_status").innerHTML =  isActiveSTA + " und Sie sind damit verbunden";
            document.getElementById("AP_status").innerHTML  = "Ist aktiv";
            document.getElementById("STA_status").style.color = "#2ECC40";
            document.getElementById("AP_status").style.color = "#2ECC40";
            showAP_hinweise(false);
            showSTA_hinweise(true);
        } 
        else if (data.flagConnection == "AP")
        {
            document.getElementById("STA_status").innerHTML =  isActiveSTA;
            document.getElementById("AP_status").innerHTML  = "Ist aktiv und Sie sind damit verbunden";
            document.getElementById("AP_status").style.color = "#2ECC40";
            if (isActiveSTA == "Ist aktiv") {
                document.getElementById("STA_status").style.color = "#2ECC40";
                showSTA_hinweise(true);
            }
            else {
                document.getElementById("STA_status").style.color = "#FFDC00";
                showSTA_hinweise(false);
            }
            showAP_hinweise(true);
            
            
        }
        else {
            document.getElementById("AP_status").innerHTML  = "nicht verbunden";
            document.getElementById("STA_status").innerHTML = "nicht verbunden";
            document.getElementById("AP_status").style.color = "#FFDC00";
            document.getElementById("STA_status").style.color = "#FFDC00";
            showAP_hinweise(false);
            showSTA_hinweise(false);
        }
        
    })
    .catch((error) => {
        console.error('Error:', error);
    });
    
}

function showAP_hinweise(flag) {
    if (flag) {
        document.getElementById("AP_hinweise").innerHTML = AP_hinweise;
        document.getElementById("AP_hinweise").style.height = "13%";
        document.getElementById("AP_IP").innerHTML = ipAP;
        document.getElementById("AP_IP").href = "http://" + ipAP;
    }
    else {
        document.getElementById("AP_hinweise").innerHTML = "";
        document.getElementById("AP_hinweise").style.height = "0%";
    }
}

function showSTA_hinweise(flag) {
    if (flag) {
        document.getElementById("STA_hinweise").innerHTML = STA_hinweise;
        document.getElementById("STA_hinweise").style.height = "13%";
        document.getElementById("STA_IP").innerHTML = ipSTA;
        document.getElementById("STA_IP").href = "http://" + ipSTA;
    }
    else {
        document.getElementById("STA_hinweise").innerHTML = "";
        document.getElementById("STA_hinweise").style.height = "0%";
    }
}