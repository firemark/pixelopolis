from flask import Flask, request, render_template, url_for
from subprocess import run, TimeoutExpired, PIPE
from base64 import b64encode

app = Flask(__name__)

MENU = [
    ('Basic shapes', [
        ('Cube', 'examples/basic/cube.css'),
        ('Cylinder', 'examples/basic/cylinder.css'),
        ('Cone', 'examples/basic/cone.css'),
        ('Dome', 'examples/basic/dome.css'),
        ('Pyramid', 'examples/basic/pyramid.css'),
        ('Triangle', 'examples/basic/triangle.css'),
    ]),
    ('Containers', [
        ('Union', 'examples/containers/union.scss'),
        ('Fillers', 'examples/containers/fillers.css'),
        ('Series', 'examples/containers/series.css'),
        ('Square-Fence', 'examples/containers/square-fence.css'),
        ('Bsp-tree', 'examples/containers/bsp-tree.css'),
    ]),
    ('Textures', [
        ('Textures', 'examples/textures/textures.css'),
        ('Floor-Align', 'examples/textures/floor-align.css'),
        ('Points', 'examples/textures/points.css'),
    ]),
    ('Attributes', [
        ('Random', 'examples/random.css'),
        ('Rotate', 'examples/rotate.css'),
    ]),
    ('Complex', [
        ('Mosque', 'examples/complex/mosque.scss'),
        ('Village', 'examples/complex/village.css'),
        ('Complex-old', 'examples/complex/complex.css'),
    ]),
]

HIDDEN_FILES = {
    'Tutorial/Hello-Cube': 'examples/tutorial/hello-cube.scss',
    'Tutorial/Textures': 'examples/tutorial/textures.scss',
    'Tutorial/Containers': 'examples/tutorial/containers.scss',
    'Tutorial/Nested-objects': 'examples/tutorial/nested-objects.scss',
}

FILES_TO_DICT = {
    f'{category.replace(" ", "-")}/{name}': filepath
    for category, files in MENU
    for name, filepath in files
}

FILES_TO_DICT.update(HIDDEN_FILES)


@app.route('/', methods=['GET'])
def get():
    data = get_data_from_file()
    return render_and_run(data)


@app.route('/examples/<category>/<filename>', methods=['GET'])
def get_example(category, filename):
    data = get_data_from_file(f'{category}/{filename}')
    return render_and_run(data)


@app.route('/', methods=['POST'])
def post():
    data = request.form['data']
    return render_and_run(data)


def get_data_from_file(filename=None) -> str:
    if not MENU:
        return ''

    if filename is None:
        filename = 'Complex/Village'

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
        menu=MENU,
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
