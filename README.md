
# CLP Linear Programming solver ported to WebAssembly

This project brings linear programming library CLP to the browser and node.js via WebAssembly.
The interface at the moment only supports a single method `solve(problem: string): string`, where `problem` is a string specifyign the linear programming input in [LP format](https://www.ibm.com/support/knowledgecenter/SSSA5P_12.7.1/ilog.odms.cplex.help/CPLEX/FileFormats/topics/LP.html).

## Live demo

You can try the solver [here](https://dpar39.github.io/clp-wasm/)

## Building from source

You'll need docker
Simply run

```bash
npm install
npm run build
npm run test # Optionally run the test suite
```

## Usage

For a browser-based example, please look at the live demo.

To install from NPM

```bash
npm install clp-wasm
```

Then import in Javasript and use

```javascript
require("clp-wasm/clp-wasm").then(clp => {

   const lp = `Maximize
   obj: + 0.6 x1 + 0.5 x2
   Subject To
   cons1: + x1 + 2 x2 <= 1
   cons2: + 3 x1 + x2 <= 2
   End`;

   console.log(clp.solve(lp));
});
```
