const importSolver = () => {
  return new Promise((resolve) => {
    const m = require("./dist/solver.js");
    m['onRuntimeInitialized'] = () => resolve(m);
    // let x = m();
    // x.then((lib) => {
    //   resolve(lib);
    // });
  });
};

let solver = null;
describe("CLP tests", () => {
  beforeAll(() => {
    return new Promise((resolve) => {
      importSolver().then((l) => {
        solver = l; 
        resolve(l);
      });
    });
  });

  test("Sweden Calendar", async () => {
    const t = `[METADATA]
    name Tinlake Solver (WIP)
    vars 4
    [VARIABLES]
    0  tinInvest     124
    0  dropInvest    400
    0  tinRedeem     100
    0  dropRedeem    300
    [CONSTRAINTS]
    1 1 -1 -1 > -200
    1 1 -1 -1 < 9800
    0.85 -0.15 -0.85 0.15 > -50
    -0.80 0.20 0.80 -0.20 > 0
    [OBJECTIVE]
    maximize 10000 1000 100000 1000000`;
  
    const expected =
      '{"variables":["tinInvest","dropInvest","tinRedeem","dropRedeem"],"solution":["124.00","400.00","100.00","300.00"],"unboundedRay":[],"infeasibilityRay":[],"objectiveValue":"311640000.00"}';
  
  
    expect(solver.solveLinearProblem(t)).toBe(expected);
  });
});


