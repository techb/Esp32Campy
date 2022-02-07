window.oncontextmenu = function() { return false; }
let ws;

function showConnectionForm(){
	document.getElementById("loading").classList.add("hidden");
	document.getElementById("controls-container").classList.add("hidden");
	document.getElementById("connection-container").classList.remove("hidden");
}
function hideConnectionForm(){
	document.getElementById("loading").classList.remove("hidden");
	document.getElementById("controls-container").classList.remove("hidden");
	document.getElementById("connection-container").classList.add("hidden");
}
function showError(){
	document.getElementById("error-container").classList.remove("hidden");
	document.getElementById("error-container").classList.add("show");
}


function WSConnection(host, port){
	// check if connection form fields were empty
	if(!host || !port){
		showError();
		return;
	}
	hideConnectionForm();

	// setup websockets and start receiving images
	const img = document.getElementById('stream');
	const WS_URL = 'ws://'+host+':'+port;
	const ws = new WebSocket(WS_URL);
	let stream_started = false;
	let urlObject;
	ws.onopen = () => {
		console.log(`[+] Connected to ${WS_URL}`);
	}

	ws.onerror = () => {
		console.log("[-] Server not found");
		showConnectionForm();
		showError();
		return;
	}

	ws.onmessage = (message) => {
		// remove loading gif and show stream
		if(!stream_started){
			document.getElementById("stream").classList.remove("hidden");
			document.getElementById("loading").classList.add("hidden");
			stream_started = true;
			console.log("[+] Stream started");
		}
		const arrayBuffer = message.data;
		if(urlObject){
			URL.revokeObjectURL(urlObject);
		}
		urlObject = URL.createObjectURL(new Blob([arrayBuffer]));
		img.src = urlObject;
	}

	// handle html button controls
	const buttons = document.querySelectorAll('.button');
	for(let i = 0; i < buttons.length; i++){
		console.log(buttons[i]);
		buttons[i].addEventListener('mousedown', function(e){
			whichButtonOn(e, ws);
			e.preventDefault();
		});
		buttons[i].addEventListener('mouseup', function(e){
			whichButtonOff(e, ws);
			e.preventDefault();
		});
	}
}

function whichButtonOn(e, ws){
	if(e.target.classList.contains("forward")){
		ws.send(packJSON("forward"));
	}
	if(e.target.classList.contains("reverse")){
		ws.send(packJSON("reverse"));
	}
	if(e.target.classList.contains("left")){
		ws.send(packJSON("left"));
	}
	if(e.target.classList.contains("right")){
		ws.send(packJSON("right"));
	}
	if(e.target.classList.contains("A")){
		ws.send(packJSON("A"));
	}
	if(e.target.classList.contains("B")){
		ws.send(packJSON("B"));
	}
	if(e.target.classList.contains("X")){
		ws.send(packJSON("X"));
	}
	if(e.target.classList.contains("Y")){
		ws.send(packJSON("Y"));
	}
}

function whichButtonOff(e, ws){
	if(e.target.classList.contains("forward")){
		console.log("Off", "forward");
	}
	if(e.target.classList.contains("reverse")){
		console.log("Off", "reverse");
	}
	if(e.target.classList.contains("left")){
		console.log("Off", "left");
	}
	if(e.target.classList.contains("right")){
		console.log("Off", "right");
	}
	if(e.target.classList.contains("A")){
		console.log("Off", "A");
	}
	if(e.target.classList.contains("B")){
		console.log("Off", "B");
	}
	if(e.target.classList.contains("X")){
		console.log("Off", "X");
	}
	if(e.target.classList.contains("Y")){
		console.log("Off", "Y");
	}
}


function packJSON(msg){
	return JSON.stringify({"message": msg});
}

// get values from host and ip fields and start WS connection
const connection_button = document.getElementById("connection-submit");
connection_button.addEventListener('click', function(e){
	e.preventDefault();
	const host = document.getElementById("host-ip").value;
	const port = document.getElementById("host-port").value;
	ws = WSConnection(host, port);
});


// // gamepad support
// if(gpLib.supportsGamepads()){
// 	// listen for new controller connected
// 	window.addEventListener("gamepadconnected", () => {
// 		console.log("Gamepad connected");
// 	});

// 	function gamepadLoop() {
// 		requestAnimationFrame(gamepadLoop);
// 		let current_buttons = gpLib.getButtons();
// 		// console.log( current_buttons );
// 	}
// 	// gamepadLoop();
// }