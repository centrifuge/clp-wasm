/* $Id: ClpDualRowDantzig.hpp 2385 2019-01-06 19:43:06Z unxusr $ */
// Copyright (C) 2002, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

#ifndef ClpDualRowDantzig_H
#define ClpDualRowDantzig_H

#include "ClpDualRowPivot.hpp"

//#############################################################################

/** Dual Row Pivot Dantzig Algorithm Class

This is simplest choice - choose largest infeasibility

*/

class ClpDualRowDantzig : public ClpDualRowPivot {

public:
  ///@name Algorithmic methods
  //@{

  /// Returns pivot row, -1 if none
  virtual int pivotRow();

  /** Updates weights and returns pivot alpha.
         Also does FT update */
  virtual FloatT updateWeights(CoinIndexedVector *input,
    CoinIndexedVector *spare,
    CoinIndexedVector *spare2,
    CoinIndexedVector *updatedColumn);
  /** Updates primal solution (and maybe list of candidates)
         Uses input vector which it deletes
         Computes change in objective function
     */
  virtual void updatePrimalSolution(CoinIndexedVector *input,
    FloatT theta,
    FloatT &changeInObjective);
  //@}

  ///@name Constructors and destructors
  //@{
  /// Default Constructor
  ClpDualRowDantzig();

  /// Copy constructor
  ClpDualRowDantzig(const ClpDualRowDantzig &);

  /// Assignment operator
  ClpDualRowDantzig &operator=(const ClpDualRowDantzig &rhs);

  /// Destructor
  virtual ~ClpDualRowDantzig();

  /// Clone
  virtual ClpDualRowPivot *clone(bool copyData = true) const;

  //@}

  //---------------------------------------------------------------------------

private:
  ///@name Private member data
  //@}
};

#endif

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/
