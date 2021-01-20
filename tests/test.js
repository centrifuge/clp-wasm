const fs = require('fs');

function almostEq(a, b, tol) {
  tol = tol || 0.01;
  return Math.abs(a - b) < tol;
}

const rebench = false;
let glpk = null;
try {
  glpk = require('glpk');
} catch (e) {
  console.log("Not validating against GLPK");
}

let solver = null;
describe("clp-wasm test suite", () => {

  beforeAll(async () => {
    solver = await require("../clp-wasm.js");
  });

  test("test CPLEX LP format", () => {
    const lpProblemFiles = ['diet_large', 'lp', 'tinlake'];
    for (const fileName of lpProblemFiles) {

      const lpFile = `${__dirname}/data/${fileName}.lp`;
      const lpBench = `${__dirname}/bench/${fileName}.json`;
      const lpContent = fs.readFileSync(lpFile, "utf8");

      const result = solver.solve(lpContent);

      if (fs.existsSync(lpBench) && !rebench) {
        const expected = JSON.parse(fs.readFileSync(lpBench, "utf8"));
        expect(expected).toMatchObject(result);
      }
      else {
        console.log(`Rebencing problem ${lpProblemFiles}`)
        const benchcontent = JSON.stringify(result);
        fs.writeFileSync(lpBench, benchcontent, "utf8");
      }

      if (glpk) {
        // Solve problem using node-glpk for cross validation
        let prob = new glpk.Problem();
        prob.readLpSync(lpFile);
        prob.scaleSync(glpk.SF_AUTO);
        prob.simplexSync({
          presolve: glpk.ON,
          msgLev: glpk.MSG_ERR
        });
        let z1 = 0;
        if (prob.getNumInt() > 0) {
          prob.intoptSync();
          z1 = prob.mipObjVal();
        } else {
          z1 = prob.getObjVal()
        }
        prob.delete();

        // Solve using this library
        const result = solver.solve(lpContent);
        const z2 = Number(result.objectiveValue);
        expect(almostEq(z1, z2)).toBeTruthy();
      }
    }
  });
});

