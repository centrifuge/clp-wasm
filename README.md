
# CLP Linear Programming solver ported to WebAssembly

This project brings linear programming library CLP to the browser and node.js via WebAssembly. Given the limited precision of standard double arithmetics when dealing with values over `2^53`, the library is compiled with custom floating point precision mapped to `boost::multiprecision::number<mp::cpp_dec_float<100>, mp::et_off>` (i.e. floating point with 100 decimal values).
The interface at the moment only supports a single method `solve(problem: string, precision?: number): string`, where `problem` is a string specifyign the linear programming input in [LP format](https://www.ibm.com/support/knowledgecenter/SSSA5P_12.7.1/ilog.odms.cplex.help/CPLEX/FileFormats/topics/LP.html) and `precision` is just the numeric precision of the values printed as strings when returning the result object.

## Live demo

You can try the solver [here](https://centrifuge.github.io/clp-wasm/)

## Building from source

You'll need docker installed in your system. Simply run:

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

If you don't want to deal with the `clp-wasm.wasm` asset contet and don't mind the extra size and Base64 conversion, `clp-wasm.all.js` includes the wasm blob as a Base64 string can be used without any extra dependencies. 
