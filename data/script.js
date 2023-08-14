/*
Written by spoonie (Rick Spooner) for RobotsGo 

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

A Huge thankyou to Rui Santos https://randomnerdtutorials.com for his
esp32 tutorials which this code is heavly based on.

Thanks Rui :)
    
*/

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onload);

//Counters for onmousedown
var count = 0;
var counter;

function onload(event) {
    initWebSocket();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    getValues();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

//Claw open commands
function clawOpen(event){
    counter = setInterval(function(){websocket.send("cOpen");}, 20);
}

function clawClose(event){
    counter = setInterval(function(){websocket.send("cClose");}, 20);
}

//Claw Rotate commands
function rotateLeft(event){
    counter = setInterval(function(){websocket.send("rLeft");}, 20);
}

function rotateRight(event){
    counter = setInterval(function(){websocket.send("rRight");}, 20);
}

//Stick commands
function stickUp(event){
    counter = setInterval(function(){websocket.send("sUp");}, 20);
}

function stickDown(event){
    counter = setInterval(function(){websocket.send("sDown");}, 20);
}

//Boom commands
function boomForwards(event){
    counter = setInterval(function(){websocket.send("bForwards");}, 20);
}

function boomBackwards(event){
    counter = setInterval(function(){websocket.send("bBackwards");}, 20);
}

//Slew commands
function slewLeft(event) {
     websocket.send("sLeft");
}

function slewRight(event) {
     websocket.send("sRight");
}

//Stop command
function stop(event) {
    websocket.send("sStop");
    clearInterval(counter)
}

function home(event) {
    websocket.send("home");
}


