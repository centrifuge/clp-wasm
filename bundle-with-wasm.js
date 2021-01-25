const fs = require('fs');
const contents = fs.readFileSync('clp-wasm.wasm', { encoding: 'base64' });
let clpjs = fs.readFileSync('clp-wasm.js', 'utf8')
const newClp = clpjs.replace('var wasmBlobStr=null;', 'var wasmBlobStr="' + contents + '";');
fs.writeFileSync('clp-wasm.all.js', newClp, 'utf8');