/* $Id: CoinPresolveDoubleton.hpp 2083 2019-01-06 19:38:09Z unxusr $ */
// Copyright (C) 2002, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

#ifndef CoinPresolveDoubleton_H
#define CoinPresolveDoubleton_H

#define DOUBLETON 5

/*! \class FloatTton_action
    \brief Solve ax+by=c for y and substitute y out of the problem.

  This moves the bounds information for y onto x, making y free and allowing
  us to substitute it away.
  \verbatim
	   a x + b y = c
	   l1 <= x <= u1
	   l2 <= y <= u2	==>
	  
	   l2 <= (c - a x) / b <= u2
	   b/-a > 0 ==> (b l2 - c) / -a <= x <= (b u2 - c) / -a
	   b/-a < 0 ==> (b u2 - c) / -a <= x <= (b l2 - c) / -a
  \endverbatim
*/
class FloatTton_action : public CoinPresolveAction {
public:
  struct action {

    FloatT clox;
    FloatT cupx;
    FloatT costx;

    FloatT costy;

    FloatT rlo;

    FloatT coeffx;
    FloatT coeffy;

    FloatT *colel;

    int icolx;
    int icoly;
    int row;
    int ncolx;
    int ncoly;
  };

  const int nactions_;
  const action *const actions_;

private:
  FloatTton_action(int nactions,
    const action *actions,
    const CoinPresolveAction *next)
    : CoinPresolveAction(next)
    , nactions_(nactions)
    , actions_(actions)
  {
  }

public:
  const char *name() const { return ("FloatTton_action"); }

  static const CoinPresolveAction *presolve(CoinPresolveMatrix *,
    const CoinPresolveAction *next);

  void postsolve(CoinPostsolveMatrix *prob) const;

  virtual ~FloatTton_action();
};
#endif

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/
