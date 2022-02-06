window.oncontextmenu = function() { return false; }

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

	// handle button controls
	const buttons = document.querySelectorAll('.button');
	for(let i = 0; i < buttons.length; i++){
		buttons[i].addEventListener('click', function(e){
			e.preventDefault();
			console.log(e.target.classList.contains("up"));
		});
	}
}


const connection_button = document.getElementById("connection-submit");
connection_button.addEventListener('click', function(e){
	e.preventDefault();
	const host = document.getElementById("host-ip").value;
	const port = document.getElementById("host-port").value;
	WSConnection(host, port);
});