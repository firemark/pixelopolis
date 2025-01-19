import { basicSetup } from "codemirror";
import { EditorView } from "@codemirror/view";
import { EditorState } from "@codemirror/state";
import { vsCodeDark } from '@fsegurai/codemirror-theme-vscode-dark'
import { css } from "@codemirror/lang-css";

let $editorView = null;

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
    const code = await response.text();
    let state = EditorState.create(
        {
            doc: code,
            extensions: [basicSetup, css(), vsCodeDark],
        }
    );
    $editorView.setState(state);

    if (buildAndDrawTimer) {
        clearTimeout(buildAndDrawTimer);
    }
    Module.func__stop();
    Module.func__init();
    buildAndDrawTimer = setTimeout(buildAndDraw, 0);
}

let buildAndDrawTimer = null;
async function buildAndDraw() {
    const code = $editorView.state.doc.toString();
    await Module.func__build(code);
    await Module.func__draw(0, 0, 0);
    // buildAndDrawTimer = setTimeout(buildAndDraw, 3000);
};



function start() {
    $editorView = new EditorView({
        doc: "world { width: 2000; height: 2000; }\n",
        parent: document.getElementById('code'),
    });

    const selectEl = document.querySelector("#select");
    selectEl.addEventListener('change', e => {
        const first = selectEl.value;
        setTimeout(async _ => await fetchCode(first), 0);
    });
    const first = selectEl.value;
    document.getElementById('btn-run').addEventListener('click', _ => {
        buildAndDraw();
    });
    setTimeout(async _ => await fetchCode(first), 0);
}

// document.addEventListener('DOMContentLoaded', start, false);