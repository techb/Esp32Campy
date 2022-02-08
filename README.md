# Esp32Campy
Python implementation for a WebSocket server handling ESP32 Cam module streams. Flask server to handle front-end and house the JS used that does most of the WebSocket work. Since this is using a server to handle everything, the limit for concurrent connections, all receiving the live stream and control access, is high.

This is a fun hobby project and not meant for use in production environments.

- **WebSocket Server**: Handles the websocket connections. Tracks currently connected clients and removes them when their connections drop. When the WS server receives  messages/data it will echo/resend it to all other connected clients.
- **Flask**: Simple flask server handling front-end connections. Serves a single html page containing an img tag for the video data and buttons to control the ESP32 Cam module.

![Screenshot 1](READMEFILES/img/splash-page.png)
![Screenshot 2](READMEFILES/img/waiting-on-espcam.png)
![Screenshot 3](READMEFILES/img/working-stream-low-rez.png)


## Software Requirements
- websockets: https://github.com/aaugustin/websockets
- Flask: https://flask.palletsprojects.com/


## Firmware Requirements
- ArduinoJson: https://arduinojson.org/
- Arduino Websockets: https://github.com/gilmaimon/ArduinoWebsockets
- ESP32 Libs & Board manager: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/


## BOM
- ESP32 Cam: https://www.amazon.com/dp/B07S5PVZKV
- USB to FTDI: https://www.sparkfun.com/products/9716
- N20 Motors X2: https://www.amazon.com/gp/product/B07SQ4TV2T
- H-Bridge: https://www.amazon.com/Aideepen-Driver-H-Bridge-Replace-Stepper/dp/B075S368Y2/
- 18640 Batteries X2: https://www.amazon.com/LCLEBM-3400mAh-Battery-Spotlight-Flashlight/dp/B08H1NZLY7


## 3D Print
**--- WARNING! ---**

The .gcode file provided uses a BLTouch.
If you don't use a BLTouch you will need to remove `G29 ; Level bed` in the .gcode file.

**----------------**

![stl preview 10 hours](/3DPrint/Slice-Screenshot.png)

#### Remixed
- cam case: https://www.thingiverse.com/thing:4191077
- 18640 battery holder: https://www.thingiverse.com/thing:2339441/files
- base for battery holder: https://www.thingiverse.com/thing:2756968/files
- SMARS robot: https://www.thingiverse.com/thing:2662828


## Run
- `$ git clone git@github.com:techb/Esp32Campy.git`
- Ensure you have the required libraries installed for your Arduino IDE
- Upload the `firmware/` code to the ESP32 Cam module via FTDI
  - You will probably need to make a new Arduino project and save or copy the files
- `$ python -m venv venv`
- `$ venv\Scripts\activate`
  - activating env is dependent on [your OS](https://www.infoworld.com/article/3239675/virtualenv-and-venv-python-virtual-environments-explained.html)
- `$(venv) pip install -r requirements.txt`
- `$(venv) python servers.py`
- Visit your server's IP, or localhost if on the same machine
  - default front-end port: `4242`
  - http://127.0.0.1:4242


## To Do
- Wire diagram and schematics
- Build/Project blog write up


## References
- websockets: https://github.com/aaugustin/websockets
  - docs: https://websockets.readthedocs.io/en/stable/
- great vid, he has more too: https://www.youtube.com/watch?v=SfQd1FdcTlI
- arduino ws broadcast example: https://shorturl.at/akAFG
- loading gif: https://boingboing.net/2015/10/18/loadingicon-trippy-looping-gi.html
- d-pad buttons: https://saruwakakun.com/en/css3-buttons
- AB/XY buttons: https://codepen.io/tswone/pen/GLzZLd
- Bootstrap CSS: https://getbootstrap.com/
- Gamepad support: https://w3c.github.io/gamepad/#remapping
- Gamepad tutorial: https://beej.us/blog/data/javascript-gamepad/
- RX GPIO 3 is always and only an input: https://electronics.stackexchange.com/a/445636