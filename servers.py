import threading

# import Websocket and Flask servers
from classes.WSServer import WSServer
import classes.FlaskServer as fs


# Setup and run Websocket server
def runWS():
	ws = WSServer(host='0.0.0.0', port=5000)
	print("[+] Starting WebSocket Server")
	ws.run()


# Setup and run Flask server
def runFS():
	print("[+] Starting Flask Server")
	fs.run(host='0.0.0.0', port=4242)


def main():
	ws_thread = threading.Thread(target=runWS)
	fs_thread = threading.Thread(target=runFS)

	# Start threads and run forever
	ws_thread.start()
	fs_thread.start()

	ws_thread.join()
	fs_thread.join()
	print("Both servers have exited")


if __name__ == "__main__":
	main()