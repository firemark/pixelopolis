from flask import Flask, request, render_template, url_for
from subprocess import run, TimeoutExpired, PIPE
from base64 import b64encode
from glob import iglob
from sass import compile as sass_compile, CompileError
app = Flask(__name__)

FILES = list(iglob('examples/*.css')) + list(iglob('examples/*.scss'));

@app.route('/', methods=['GET'])
def get():
    data, is_sass_support = get_data_from_file()
    return render_index(data=data, is_sass_support=is_sass_support)


@app.route('/', methods=['POST'])
def post():
    data = request.form['data']
    is_sass_support = request.form.get('type') == 'sass'
    errors, img = run_pixelopolis(data, is_sass_support)
    return render_index(
        data=data,
        errors=errors, 
        img=img, 
        is_sass_support=is_sass_support,
    )


def get_data_from_file() -> (str, bool):
    if not FILES:
        return '', False
    filename = request.args.get('file', '') or FILES[0]
    if filename not in FILES:
        return '', False
    with open(filename) as fp:
        return fp.read(), filename.endswith('.scss')


def render_index(errors=None, data='', img=None, is_sass_support=False):
    errors = errors or []
    return render_template(
        'index.html', 
        errors=errors, 
        data=data,
        files=FILES, 
        img=img,
        is_sass_support=is_sass_support,
    ) 


def run_pixelopolis(data, is_sass_support=False):
    if is_sass_support:
        try:
            data = sass_compile(string=data)
        except CompileError as exp:
            return [str(exp)], None
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
