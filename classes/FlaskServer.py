from flask import Flask, render_template
import os

# Set template directory
template_dir = os.path.abspath('templates')
static_dir = os.path.abspath('static')
app = Flask(__name__, template_folder=template_dir, static_folder=static_dir)

# homepage route
@app.route('/')
def index():
	return render_template('index.html')


# run forever
def run(host, port):
	app.run(host=host, port=port)


# can be stand-alone
if __name__ == "__main__":
	run(host='0.0.0.0', port=4242)