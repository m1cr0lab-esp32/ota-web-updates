var gateway = 'ws://' + window.location.hostname + ':1234/';
var timeout = 2000; // milliseconds
var websocket;
var led;
var button;

window.addEventListener("load", init, false);

function init() {
    led    = document.getElementById('led');
    button = document.getElementById('button');
    button.addEventListener('click', onPress);
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
    doSend('getLEDState');
}

function onClose(event) {
    setTimeout(function () { wsConnect(gateway); }, timeout);
}

function onMessage(event) {
    switch (event.data) {
        case '0':
            led.classList.remove('on');
            button.classList.remove('off');
            break;
        case '1':
            led.classList.add('on');
            button.classList.add('off');
            break;
    }
}

function onError(event) {

}

function doSend(message) {
    websocket.send(message);
}

function onPress(event) {
    event.preventDefault();
    event.stopPropagation();
    doSend("toggleLED");
    doSend("getLEDState");
}