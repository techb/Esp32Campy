// stop long click context popup on android
window.oncontextmenu = function () { return false; }

// global websocket object
let ws;


// helper for showing/hiding form and error alert
function showConnectionForm() {
	document.getElementById("loading").classList.add("hidden");
	document.getElementById("controls-container").classList.add("hidden");
	document.getElementById("connection-container").classList.remove("hidden");
}
function hideConnectionForm() {
	document.getElementById("loading").classList.remove("hidden");
	document.getElementById("controls-container").classList.remove("hidden");
	document.getElementById("connection-container").classList.add("hidden");
}
function showError() {
	document.getElementById("error-container").classList.remove("hidden");
	document.getElementById("error-container").classList.add("show");
}


// pack websocket message json
function packJSON(msg) {
	return JSON.stringify({ "message": msg });
}


// handle websocket connection
function WSConnection(host, port) {
	// check if connection form fields were empty
	if (!host || !port) {
		showError();
		return;
	}
	hideConnectionForm();

	// setup websockets and start receiving images
	const img = document.getElementById('stream');
	const WS_URL = 'ws://' + host + ':' + port;
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
		if (!stream_started) {
			document.getElementById("stream").classList.remove("hidden");
			document.getElementById("loading").classList.add("hidden");
			stream_started = true;
			console.log("[+] Stream started");
		}
		const arrayBuffer = message.data;
		if (urlObject) {
			URL.revokeObjectURL(urlObject);
		}
		urlObject = URL.createObjectURL(new Blob([arrayBuffer]));
		img.src = urlObject;
	}

	// handle html button controls
	const buttons = document.querySelectorAll('.button');
	for (let i = 0; i < buttons.length; i++) {
		buttons[i].addEventListener('mousedown', function (e) {
			whichButton(e, ws, true);
			e.preventDefault();
		});
		buttons[i].addEventListener('mouseup', function (e) {
			whichButton(e, ws, false);
			e.preventDefault();
		});
	}

	// gamepad support
	if (gpLib.supportsGamepads()) {
		console.log('[+] Gamepad supported');
		// listen for new controller connected
		window.addEventListener("gamepadconnected", () => {
			console.log("[+] Gamepad connected");
		});

		function gamepadLoop() {
			requestAnimationFrame(gamepadLoop);
			let current_buttons = gpLib.getButtons();
			if (current_buttons) {
				Object.keys(current_buttons).forEach(key => {
					if (current_buttons[key]) {
						ws.send(packJSON(key));
					}
				});
			}
		}
		gamepadLoop();
	}
}


function whichButton(e, ws, onoff) {
	if (onoff) {
		if (e.target.classList.contains("forward")) {
			ws.send(packJSON("forward"));
		}
		if (e.target.classList.contains("reverse")) {
			ws.send(packJSON("reverse"));
		}
		if (e.target.classList.contains("left")) {
			ws.send(packJSON("left"));
		}
		if (e.target.classList.contains("right")) {
			ws.send(packJSON("right"));
		}
		if (e.target.classList.contains("A")) {
			ws.send(packJSON("AON"));
		}
		if (e.target.classList.contains("B")) {
			ws.send(packJSON("BON"));
		}

	} else {
		// get button map objects keys
		// turn classList into array
		const directions = Object.keys(gpLib.buttonMap);
		const classes = [].slice.apply(e.target.classList);
		// check if directions are in button map
		// A,B buttons won't match since they are AOFF, BOFF in map
		const hault = classes.some(r => directions.indexOf(r) >= 0);
		if (hault) {
			ws.send(packJSON("hault"));
		}
		if (e.target.classList.contains("A")) {
			ws.send(packJSON("AOFF"));
		}
		if (e.target.classList.contains("B")) {
			ws.send(packJSON("BOFF"));
		}
	}
}


// get values from host and ip fields and start WS connection
const connection_button = document.getElementById("connection-submit");
connection_button.addEventListener('click', function (e) {
	e.preventDefault();
	const host = document.getElementById("host-ip").value;
	const port = document.getElementById("host-port").value;
	ws = WSConnection(host, port);
});
