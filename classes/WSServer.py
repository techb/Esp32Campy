import asyncio
import websockets


class WSServer:
	# Set connection settings
	def __init__(self, host='0.0.0.0', port='5000'):
		self.clients = set()
		self.host = host
		self.port = port


	# Takes in a message from any client then sends it to all
	# other connected clients, excluding the sender
	async def echo(self, websocket):
		#  keep list of connected clients
		self.clients.add(websocket)
		print("Added Client: ", websocket)
		try:
			async for message in websocket:
				for conn in self.clients:
					if conn != websocket:
						await conn.send(message)

		except websockets.exceptions.ConnectionClosed as e:
			print("Error: ", e)

		finally:
			# remove disconnected client from list
			print("Client Disconnected: ", websocket)
			self.clients.remove(websocket)


	async def main(self):
		async with websockets.serve(self.echo, self.host, self.port, ping_interval=None):
			await asyncio.Future()


	def run(self):
		asyncio.run(self.main())


# can be stand-alone
if __name__ == "__main__":
	ws = WSServer()
	ws.run()