from flask import Flask, request, render_template, url_for
from subprocess import run, TimeoutExpired, PIPE
from base64 import b64encode

app = Flask(__name__)

FILES = (
    ('Cube', 'examples/cube.css'),
    ('Pyramid', 'examples/pyramid.css'),
    ('Triangle', 'examples/triangle.css'),
    ('Textures', 'examples/textures.css'),
    ('Floor-Align', 'examples/floor-align.css'),
    ('Random', 'examples/random.css'),
    ('Fillers', 'examples/fillers.css'),
    ('Series', 'examples/series.css'),
    ('Complex', 'examples/complex.css'),
)

FILES_TO_DICT = dict(FILES)


@app.route('/', methods=['GET'])
def get():
    data = get_data_from_file()
    return render_and_run(data)


@app.route('/examples/<filename>', methods=['GET'])
def get_example(filename):
    data = get_data_from_file(filename)
    return render_and_run(data)


@app.route('/', methods=['POST'])
def post():
    data = request.form['data']
    return render_and_run(data)


def get_data_from_file(filename=None) -> str:
    if not FILES:
        return ''

    if filename is None:
        filename = 'Complex'

    path = FILES_TO_DICT.get(filename)
    if path is None:
        return ''
    with open(path) as fp:
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


def render_and_run(data):
    errors, img = run_pixelopolis(data)
    return render_index(
        data=data,
        errors=errors, 
        img=img, 
    )
