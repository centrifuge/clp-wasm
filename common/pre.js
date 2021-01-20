var clpPromise = new Promise(function (resolve) {
  Module["preInit"] = [
    function () {
      FS.mkdir("/out");
      FS.mount(MEMFS, { root: "." }, "/out");
    },
  ];
  Module['print'] = () => { };

  var constants = {
    /* direction: */
    GLP_MIN: 1 /* minimization */,
    GLP_MAX: 2 /* maximization */,

    /* type of auxiliary/structural variable: */
    GLP_FR: 1 /* free (unbounded) variable */,
    GLP_LO: 2 /* variable with lower bound */,
    GLP_UP: 3 /* variable with upper bound */,
    GLP_DB: 4 /* double-bounded variable */,
    GLP_FX: 5 /* fixed variable */,

    /* message level: */
    GLP_MSG_OFF: 0 /* no output */,
    GLP_MSG_ERR: 1 /* warning and error messages only */,
    GLP_MSG_ON: 2 /* normal output */,
    GLP_MSG_ALL: 3 /* full output */,
    GLP_MSG_DBG: 4 /* debug output */,

    /* solution status: */
    GLP_UNDEF: 1 /* solution is undefined */,
    GLP_FEAS: 2 /* solution is feasible */,
    GLP_INFEAS: 3 /* solution is infeasible */,
    GLP_NOFEAS: 4 /* no feasible solution exists */,
    GLP_OPT: 5 /* solution is optimal */,
    GLP_UNBND: 6 /* solution is unbounded */,

    DBL_MAX: Number.MAX_VALUE,
    /* kind of structural variable: */
    GLP_CV: 1, /* continuous variable */
    GLP_IV: 2, /* integer variable */
    GLP_BV: 3, /* binary variable */
  };

  function nameValToStr(name, coef, first) {
    var str = "";
    coef = coef.toString();
    var coefnum = Number(coef);
    if (first && coefnum == 1)
      return name;
    if (coefnum === 1) {
      str += "+";
    } else if (coefnum == -1) {
      str += "-";
    } else if (!(coef.startsWith("-") || coef.startsWith("+") || first)) {
      str += "+" + coef;
    } else {
      str += coef;
    }
    str += " " + name;
    return str;
  }

  function isdefined(x) {
    return typeof x !== "undefined";
  }

  function varCoefExpression(name, vars) {
    var str = "";
    if (name) {
      str += " " + name + ": ";
    }

    var first = true;
    for (var nc of vars) {
      str += nameValToStr(nc.name, nc.coef, first) + " ";
      first = false;
    }
    return str;
  }

  function objectiveToStr(obj) {
    var str = "";
    if (obj.direction == constants.GLP_MAX) {
      str += "Maximize\n";
    } else if (obj.direction == constants.GLP_MIN) {
      str += "Minimize\n";
    }
    str += varCoefExpression(obj.name, obj.vars);
    return str + "\n";
  }

  function boundedExpression(expression, lbVal, ubVal, type) {
    var prefix = "";
    var posfix = "";
    var lb = isdefined(lbVal) ? lbVal.toString() : "";
    var ub = isdefined(ubVal) ? ubVal.toString() : "";
    if (!isdefined(type)) {
      if (lb && ub) {
        if (lb == ub) {
          type == constants.GLP_FX;
        } else {
          type = constants.GLP_DB;
        }
      } else if (lb) {
        type = constants.GLP_LO;
      } else if (ub) {
        type = constants.GLP_UP;
      } else {
        throw new Error("No bounds were specified");
      }
    }

    if (type == constants.GLP_LO) {
      posfix = " >= " + lb;
    } else if (type == constants.GLP_UP) {
      posfix = " <= " + ub;
    } else if (type == constants.GLP_FX) {
      posfix = " = " + ub ? ub : lb;
    } else if (type == constants.GLP_DB) {
      prefix = lb + " <= ";
      posfix = " <= " + ub;
    }
    return prefix + expression + posfix + "\n";
  }

  function constraintToStr(row) {
    var expression = varCoefExpression(row.name, row.vars);
    return boundedExpression(
      expression,
      row.bnds.lb,
      row.bnds.ub,
      row.bnds.type
    );
  }

  function toLpFormat(lp) {
    var str = objectiveToStr(lp.objective);

    if (lp.subjectTo) {
      var constraints_str = "";
      for (var row of lp.subjectTo) {
        constraints_str += constraintToStr(row);
      }
      str += "Subject To\n" + constraints_str + "\n";
    }
    if (lp.bounds) {
      var var_bounds_str = "";
      for (var col of lp.bounds) {
        var_bounds_str += boundedExpression(
          col.name,
          col.bnds.lb,
          col.bnds.ub,
          cols.bnds.type
        );
      }
      str += "Bounds\n" + var_bounds_str + "\n";
    }
    str += 'End\n'
    return str;
  }

  Module["onRuntimeInitialized"] = function () {
    var m = this;
    this["clp"] = (function () {
      var methods = {
        version: function () {
          return "TODO";
        },
        solve: function (lpProblem) {
          try {
            var solutionStr = m.solveLinearProblem(lpProblem);
            var solObj = JSON.parse(solutionStr);
            return solObj
          } catch {
            return null;
          }
        },
        toLpFormat: toLpFormat,
      };

      var pub = {};
      Object.assign(pub, constants, methods);
      return Object.freeze(pub);
    })();
    resolve(this["clp"]);
  };
//});
