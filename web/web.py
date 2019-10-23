from flask import Flask, request, render_template, url_for
from subprocess import run, TimeoutExpired, PIPE
from base64 import b64encode
from glob import iglob
from sass import compile as sass_compile, CompileError
app = Flask(__name__)

FILES = list(iglob('examples/*.css'))

@app.route('/', methods=['GET'])
def get():
    data = get_data_from_file()
    return render_index(data=data)


@app.route('/', methods=['POST'])
def post():
    data = request.form['data']
    errors, img = run_pixelopolis(data)
    return render_index(
        data=data,
        errors=errors, 
        img=img, 
    )


def get_data_from_file() -> str:
    if not FILES:
        return ''
    filename = request.args.get('file', '') or FILES[0]
    if filename not in FILES:
        return ''
    with open(filename) as fp:
        return fp.read()


def render_index(errors=None, data='', img=None):
    errors = errors or []
    return render_template(
        'index.html', 
        errors=errors, 
        data=data,
        files=FILES, 
        img=img,
    ) 


def run_pixelopolis(data):
    try:
        proc = run(
            args=['../pixelopolis', '-', '-'],
            input=data.encode('utf-8'),
            stdout=PIPE,
            stderr=PIPE,
            timeout=10,
        )
    except TimeoutExpired:
        return ['timeout ;_;'], None

    if proc.stdout:
        img = b64encode(proc.stdout).decode('utf-8')
    else:
        img = None

    errors = proc.stderr.decode('utf-8').strip().splitlines()
    if proc.returncode != 0:
        errors = [f'sth is wrong (errocode {proc.returncode})'] + errors
    return errors, img
