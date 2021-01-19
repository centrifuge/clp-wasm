
# CLP Linear Programming solver ported to WebAssembly

This project brings linear programming library CLP to the browser and node.js via WebAssembly.
The interface at the moment only supports a single method `solveLinearProblem(problem: string): string`, where `problem` is a string specifyign the linear programming input in [LP format](https://www.ibm.com/support/knowledgecenter/SSSA5P_12.7.1/ilog.odms.cplex.help/CPLEX/FileFormats/topics/LP.html).

## Live demo

You can try the solver [here](https://dpar39.github.io/clp-wasm/)

## Building from source

Simply run

   npm run build
   
## Usage

See `solver.test.ts` for a simple example.
