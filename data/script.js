
window.onload = function(){
    // Slider Laber aktualisieren
    var wert = document.getElementById("personenzahl").value;
    document.getElementById("einstellungen_personenzahl_label").innerHTML = wert;
}

function scrollToEinstellungen(){
    document.getElementById('wrapper_einstellungen').scrollIntoView();
}

function button_reset_func(){
    alert("Zähler wurde erfolgreich resettet.");
}

function updateSlider(wert){
    console.log(wert);
    document.getElementById("einstellungen_personenzahl_label").innerHTML = wert;
}

function updateSliderRealeased(wert){
    console.log(wert);
    //var d = new Date();
    //console.log(d);
}
function scrollToTop() {
    //window.scrollTo({ top: 0, left: 0, behavior: "smooth" });
    document.getElementById('wrapper_anzeige').scrollIntoView();
    go();
}

function go(){
    document.getElementById("anzeige_nachricht").innerHTML = "Bitte eintreten und Abstand halten"
    document.getElementById("anzeige_icon").innerHTML = "🏃‍♀️↔️🏃‍♂️";
    document.getElementById("anzeige_hinweis").innerHTML = "32 / 120 Personen"
}

function stop(){
    document.getElementById("anzeige_nachricht").innerHTML = "Stopp! Bitte warten!"
    document.getElementById("anzeige_icon").innerHTML = "✋";
    document.getElementById("anzeige_hinweis").innerHTML = "120 / 120 Personen"
}


/*
function scrollToTop (duration) {
    // cancel if already on top
    if (document.scrollingElement.scrollTop === 0) return;

    const cosParameter = document.scrollingElement.scrollTop / 2;
    let scrollCount = 0, oldTimestamp = null;

    function step (newTimestamp) {
        if (oldTimestamp !== null) {
            // if duration is 0 scrollCount will be Infinity
            scrollCount += Math.PI * (newTimestamp - oldTimestamp) / duration;
            if (scrollCount >= Math.PI) return document.scrollingElement.scrollTop = 0;
            document.scrollingElement.scrollTop = cosParameter + cosParameter * Math.cos(scrollCount);
        }
        oldTimestamp = newTimestamp;
        window.requestAnimationFrame(step);
    }
    window.requestAnimationFrame(step);
}
/* 
  Explanation:
  - pi is the length/end point of the cosinus intervall (see below)
  - newTimestamp indicates the current time when callbacks queued by requestAnimationFrame begin to fire.
    (for more information see https://developer.mozilla.org/en-US/docs/Web/API/window/requestAnimationFrame)
  - newTimestamp - oldTimestamp equals the delta time

    a * cos (bx + c) + d                        | c translates along the x axis = 0
  = a * cos (bx) + d                            | d translates along the y axis = 1 -> only positive y values
  = a * cos (bx) + 1                            | a stretches along the y axis = cosParameter = window.scrollY / 2
  = cosParameter + cosParameter * (cos bx)  | b stretches along the x axis = scrollCount = Math.PI / (scrollDuration / (newTimestamp - oldTimestamp))
  = cosParameter + cosParameter * (cos scrollCount * x)
*/
/*
function scrollToTop (duration) {
    // cancel if already on top
    if (document.scrollingElement.scrollTop === 0) return;

    const totalScrollDistance = document.scrollingElement.scrollTop;
    let scrollY = totalScrollDistance, oldTimestamp = null;

    function step (newTimestamp) {
        if (oldTimestamp !== null) {
            // if duration is 0 scrollY will be -Infinity
            scrollY -= totalScrollDistance * (newTimestamp - oldTimestamp) / duration;
            if (scrollY <= 0) return document.scrollingElement.scrollTop = 0;
            document.scrollingElement.scrollTop = scrollY;
        }
        oldTimestamp = newTimestamp;
        window.requestAnimationFrame(step);
    }
    window.requestAnimationFrame(step);
}
*/

