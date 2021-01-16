/* $Id: CoinPresolveTripleton.hpp 2083 2019-01-06 19:38:09Z unxusr $ */
// Copyright (C) 2003, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

#ifndef CoinPresolveTripleton_H
#define CoinPresolveTripleton_H
#define TRIPLETON 11
/** We are only going to do this if it does not increase number of elements?.
    It could be generalized to more than three but it seems unlikely it would
    help.

    As it is adapted from FloatTton icoly is one dropped.
 */
class tripleton_action : public CoinPresolveAction {
public:
  struct action {
    int icolx;
    int icolz;
    int row;

    int icoly;
    FloatT cloy;
    FloatT cupy;
    FloatT costy;
    FloatT clox;
    FloatT cupx;
    FloatT costx;

    FloatT rlo;
    FloatT rup;

    FloatT coeffx;
    FloatT coeffy;
    FloatT coeffz;

    FloatT *colel;

    int ncolx;
    int ncoly;
  };

  const int nactions_;
  const action *const actions_;

private:
  tripleton_action(int nactions,
    const action *actions,
    const CoinPresolveAction *next)
    : CoinPresolveAction(next)
    , nactions_(nactions)
    , actions_(actions)
  {
  }

public:
  const char *name() const { return ("tripleton_action"); }

  static const CoinPresolveAction *presolve(CoinPresolveMatrix *,
    const CoinPresolveAction *next);

  void postsolve(CoinPostsolveMatrix *prob) const;

  virtual ~tripleton_action();
};
#endif

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/
