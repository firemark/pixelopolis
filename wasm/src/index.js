import { basicSetup } from "codemirror";
import { EditorView } from "@codemirror/view";
import { EditorState } from "@codemirror/state";
import { vsCodeDark } from '@fsegurai/codemirror-theme-vscode-dark'
import { css } from "@codemirror/lang-css";

let $editorView = null;

Module.onRuntimeInitialized = _ => {
    Module['canvas'] = document.getElementById('canvas');
    Module['$output'] = document.getElementById('output');
    Module.func__init = Module.cwrap('init', 'number', []);
    Module.func__stop = Module.cwrap('stop', 'number', []);
    Module.func__build = Module.cwrap('build', 'number', ['string', 'number'], { async: true });
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

    Module.func__stop();
    Module.func__init();
    setTimeout(buildAndDraw, 0);
}

async function buildAndDraw() {
    const log = Module.addFunction(
        (severityCode, msgPtr, filenamePtr, col, row) => {
            const msg = Module.UTF8ToString(msgPtr);
            const filename = Module.UTF8ToString(filenamePtr);
            const severity = String.fromCharCode(severityCode);

            const severityCell = document.createElement("th");
            severityCell.innerText = severity;

            const filenameCell = document.createElement("td");
            if (filename) {
                filenameCell.innerText = `${filename}:${col}:${row}`;
            } else {
                filenameCell.innerHTML = "<i>GLOBAL</i>";
            }

            const msgCell = document.createElement("td");
            msgCell.innerText = msg;

            const tableRow = document.createElement("tr");
            tableRow.appendChild(severityCell);
            tableRow.appendChild(filenameCell);
            tableRow.appendChild(msgCell);

            if (filename) {
                tableRow.classList.add("highlight")
                tableRow.addEventListener("click", function () {
                    const line = $editorView.state.doc.line(col);
                    $editorView.dispatch({
                        selection: { head: line.from, anchor: line.to },
                        scrollIntoView: true
                    });
                })
            }

            Module.$output.appendChild(tableRow);
        }, "viiiii"
    );

    Module.$output.innerHTML = "";

    const code = $editorView.state.doc.toString();
    const build_has_error = await Module.func__build(code, log);
    if (build_has_error) {
        const canvas = Module['canvas'];
        const ctx = canvas.getContext('2d');
        ctx.fillStyle = "red";
        ctx.fillRect(0, 0, canvas.width, canvas.height);
        ctx.fill();
        return;
    }
    await Module.func__draw(0, 0, 0);
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