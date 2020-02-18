var gateway = 'ws://' + window.location.hostname + ':1234/';
var period  = 3000; // milliseconds
var timeout = 2000; // milliseconds
var websocket;
var temperature;
var pressure;
var humidity;

window.addEventListener("load", init, false);

function init() {
    temperature = document.getElementById('temperature');
    pressure    = document.getElementById('pressure');
    humidity    = document.getElementById('humidity');
    wsConnect(gateway);
}

function wsConnect(gateway) {
    websocket = new WebSocket(gateway);
    websocket.onopen    = function(event) { onOpen(event);    };
    websocket.onclose   = function(event) { onClose(event);   };
    websocket.onmessage = function(event) { onMessage(event); };
    websocket.onerror   = function(event) { onError(event);   };
}

function onOpen(event) {
    doSend('getData');
}

function onClose(event) {
    setTimeout(function () { wsConnect(gateway); }, timeout);
}

function onMessage(event) {
    let data = event.data.split(',');
    temperature.innerHTML = data[0];
    pressure.innerHTML    = data[1];
    humidity.innerHTML    = data[2];
    setTimeout(function () { doSend('getData'); }, period);
}

function onError(event) {

}

function doSend(message) {
    websocket.send(message);
}