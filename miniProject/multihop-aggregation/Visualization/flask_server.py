import time
import matplotlib.pyplot as plt
from flask import Flask, render_template

app = Flask(__name__)



@app.route('/')
def index():
    return render_template('index.html')

# Start the Flask app (must be after the endpoint functions) 
host_local_computer = "localhost"  # Listen for connections on the local computer
app.run(host= host_local_computer, port=9000)



