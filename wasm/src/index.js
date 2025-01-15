Module.onRuntimeInitialized = _ => {
    Module['canvas'] = document.getElementById('canvas');
    Module.func__init = Module.cwrap('init', 'number', []);
    Module.func__stop = Module.cwrap('stop', 'number', []);
    Module.func__build = Module.cwrap('build', 'number', ['string'], { async: true });
    Module.func__draw = Module.cwrap('draw', 'number', ['number', 'number', 'number'], { async: true });
    Module.func__init();
    start();
};

async function fetchCode(codeUrl) {
    const response = await fetch(codeUrl);
    if (!response.ok) {
        return;
    }
    document.getElementById('code').innerText = await response.text();

    if (buildAndDrawTimer) {
        clearTimeout(buildAndDrawTimer);
    }
    Module.func__stop();
    Module.func__init();
    buildAndDrawTimer = setTimeout(buildAndDraw, 0);
}

let buildAndDrawTimer = null;
async function buildAndDraw() {
    const code = document.getElementById('code').innerText;
    await Module.func__build(code);
    await Module.func__draw(0, 0, 0);
    buildAndDrawTimer = setTimeout(buildAndDraw, 3000);
};


function start() {
    const selectEl = document.querySelector("#select");
    selectEl.addEventListener('change', e => {
        const first = selectEl.value;
        setTimeout(async _ => await fetchCode(first), 0);
    });
    const first = selectEl.value;
    setTimeout(async _ => await fetchCode(first), 0);
}

// document.addEventListener('DOMContentLoaded', start, false);