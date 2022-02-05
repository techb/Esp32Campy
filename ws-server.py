import asyncio
import websockets

clients = set()


async def echo(websocket, path):
	clients.add(websocket)
	print("Added Client: ", websocket)
	try:
		async for message in websocket:
			for conn in clients:
				if conn != websocket:
					await conn.send(message)

	except websockets.exceptions.ConnectionClosed as e:
		print("Error: ", e)
	finally:
		print("Client Disconnected: ", websocket)
		clients.remove(websocket)


async def main():
	async with websockets.serve(echo, "0.0.0.0", 5000, ping_interval=None):
		await asyncio.Future()


asyncio.run(main())