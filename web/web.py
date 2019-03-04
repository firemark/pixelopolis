from flask import Flask, request, render_template, url_for
from subprocess import run, CalledProcessError
app = Flask(__name__)

@app.route('/', methods=['GET'])
def get():
    with open('test.css') as fp:
        data = fp.read()
    return render_template('index.html', is_ok=None, data=data)

@app.route('/', methods=['POST'])
def post():
    data = request.form['data']
    is_ok = run_pixelopolis(data)
    return render_template('index.html', is_ok=is_ok, data=data) 

def run_pixelopolis(data):
    try:
        proc = run(['../pixelopolis'], input=data.encode('utf-8'))
    except CalledProcessError:
        return False
    return True
