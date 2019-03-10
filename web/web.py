from flask import Flask, request, render_template, url_for
from subprocess import run, CalledProcessError, TimeoutExpired, PIPE
from base64 import b64encode
app = Flask(__name__)

@app.route('/', methods=['GET'])
def get():
    with open('test.css') as fp:
        data = fp.read()
    return render_template('index.html', errors=[], data=data, img=None)

@app.route('/', methods=['POST'])
def post():
    data = request.form['data']
    errors, img = run_pixelopolis(data)
    return render_template('index.html', errors=errors, data=data, img=img) 

def run_pixelopolis(data):
    try:
        proc = run(
            args=['../pixelopolis', '-', '-'],
            input=data.encode('utf-8'),
            capture_output=True,
            timeout=10,
        )
    except TimeoutExpired:
        return ['timeout ;_;'], None

    if proc.stdout:
        img = b64encode(proc.stdout).decode('utf-8')
    else:
        img = None

    errors = proc.stderr.decode('utf-8').strip().splitlines()
    if not errors and proc.returncode != 0:
        errors = ['sth is wrong']
    return errors, img
