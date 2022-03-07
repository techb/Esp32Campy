import threading, time

# import Websocket and Flask servers
from classes.WSServer import WSServer
import classes.FlaskServer as fs

WSPORT = 5000
FSPORT =  4242

# Setup and run Websocket server
def runWS():
	ws = WSServer(host='0.0.0.0', port=WSPORT)
	print("[+] Starting WebSocket Server")
	ws.run()


# Setup and run Flask server
def runFS():
	print("[+] Starting Flask Server")
	fs.run(host='0.0.0.0', port=FSPORT)


def main():
	ws_thread = threading.Thread(target=runWS, daemon=True)
	fs_thread = threading.Thread(target=runFS, daemon=True)

	# Start threads and run forever
	ws_thread.start()
	fs_thread.start()
	while threading.active_count() > 0:
		time.sleep(0.1)

	# No active threads, exiting
	print("Both servers have exited")


if __name__ == "__main__":
	main()