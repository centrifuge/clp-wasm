interface Result {
  objectiveValue: string;
  variables: string[];
  solution: string[];
  unboundedRay: string[];
  infeasibilityRay: string[];
  integerSolution: boolean;
}

type NumericType = number | string;

interface LP {
    name: string;
    objective: {
        direction: number,
        name: string,
        vars: { name: string, coef: NumericType }[]
    },
    subjectTo: {
        name: string,
        vars: { name: string, coef: number }[],
        bnds: { ub?: NumericType, lb?: NumericType, type?: number }
    }[],
    bounds?: {
        name: string,
        ub?: NumericType,
        lb?: NumericType,
        type?: number
    }[],
    binaries?: string[],
    generals?: string[]
}

interface CLP {

    /* direction */
    readonly GLP_MIN: number;  /* minimization */
    readonly GLP_MAX: number;  /* maximization */

    /* type of auxiliary/structural variable: */
    readonly GLP_FR: number;  /* free (unbounded) variable */
    readonly GLP_LO: number;  /* variable with lower bound */
    readonly GLP_UP: number;  /* variable with upper bound */
    readonly GLP_DB: number;  /* double-bounded variable */
    readonly GLP_FX: number;  /* fixed variable */

    /* message level: */
    // readonly GLP_MSG_OFF: number;  /* no output */
    // readonly GLP_MSG_ERR: number;  /* warning and error messages only */
    // readonly GLP_MSG_ON: number;   /* normal output */
    // readonly GLP_MSG_ALL: number;  /* full output */
    // readonly GLP_MSG_DBG: number;  /* debug output */

    /* solution status: */
    readonly GLP_UNDEF: number;   /* solution is undefined */
    readonly GLP_FEAS: number;    /* solution is feasible */
    readonly GLP_INFEAS: number;  /* solution is infeasible */
    readonly GLP_NOFEAS: number;  /* no feasible solution exists */
    readonly GLP_OPT: number;     /* solution is optimal */
    readonly GLP_UNBND: number;   /* solution is unbounded */

    version(): string;

    createLp(lp: LP): string;

    solve(lp: string, precision: number): Result

    bnRound(bn: string): string; /* rounds a big number from a string */
    bnCeil(bn: string): string; /* ceil of a big number from a string */
    bnFloor(bn: string): string; /* floor of a big number from a string */
}

export {
    LP,
    Result,
    CLP
}
