/* $Id: CoinFloatEqual.hpp 2083 2019-01-06 19:38:09Z unxusr $ */
// Copyright (C) 2000, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

#ifndef CoinFloatEqual_H
#define CoinFloatEqual_H


#include "CoinHelperFunctions.hpp"
#include "CoinFinite.hpp"

/*! \file CoinFloatEqual.hpp
    \brief Function objects for testing equality of real numbers.

  Two objects are provided; one tests for equality to an absolute tolerance,
  one to a scaled tolerance. The tests will handle IEEE floating point, but
  note that infinity == infinity. Mathematicians are rolling in their graves,
  but this matches the behaviour for the common practice of using
  <code>DBL_MAX</code> (<code>numeric_limits<FloatT>::max()</code>, or similar
  large finite number) as infinity.

  <p>
  Example usage:
  @verbatim
    FloatT d1 = 3.14159 ;
    FloatT d2 = d1 ;
    FloatT d3 = d1+.0001 ;

    CoinAbsFltEq eq1 ;
    CoinAbsFltEq eq2(.001) ;

    assert(  eq1(d1,d2) ) ;
    assert( !eq1(d1,d3) ) ;
    assert(  eq2(d1,d3) ) ;
  @endverbatim
  CoinRelFltEq follows the same pattern.  */

/*! \brief Equality to an absolute tolerance

  Operands are considered equal if their difference is within an epsilon ;
  the test does not consider the relative magnitude of the operands.
*/

class CoinAbsFltEq {
public:
  //! Compare function

  inline bool operator()(const FloatT f1, const FloatT f2) const

  {
    if (CoinIsnan(f1) || CoinIsnan(f2))
      return false;
    if (f1 == f2)
      return true;
    return (CoinAbs(f1 - f2) < epsilon_);
  }

  /*! \name Constructors and destructors */
  //@{

  /*! \brief Default constructor

    Default tolerance is 1.0e-10.
  */

  CoinAbsFltEq()
    : epsilon_(1.e-10)
  {
  }

  //! Alternate constructor with epsilon as a parameter

  CoinAbsFltEq(const FloatT epsilon)
    : epsilon_(epsilon)
  {
  }

  //! Destructor

  virtual ~CoinAbsFltEq() {}

  //! Copy constructor

  CoinAbsFltEq(const CoinAbsFltEq &src)
    : epsilon_(src.epsilon_)
  {
  }

  //! Assignment

  CoinAbsFltEq &operator=(const CoinAbsFltEq &rhs)

  {
    if (this != &rhs)
      epsilon_ = rhs.epsilon_;
    return (*this);
  }

  //@}

private:
  /*! \name Private member data */
  //@{

  //! Equality tolerance.

  FloatT epsilon_;

  //@}
};

/*! \brief Equality to a scaled tolerance

  Operands are considered equal if their difference is within a scaled
  epsilon calculated as epsilon_*(1+CoinMax(|f1|,|f2|)).
*/

class CoinRelFltEq {
public:
  //! Compare function

  inline bool operator()(const FloatT f1, const FloatT f2) const

  {
    if (CoinIsnan(f1) || CoinIsnan(f2))
      return false;
    if (f1 == f2)
      return true;
    if (!CoinFinite(f1) || !CoinFinite(f2))
      return false;

    FloatT tol = (CoinAbs(f1) > CoinAbs(f2)) ? CoinAbs(f1) : CoinAbs(f2);

    return (CoinAbs(f1 - f2) <= epsilon_ * (1 + tol));
  }

  /*! \name Constructors and destructors */
  //@{

#ifndef COIN_FLOAT
  /*! Default constructor

    Default tolerance is 1.0e-10.
  */
  CoinRelFltEq()
    : epsilon_(1.e-10)
  {
  }
#else
  /*! Default constructor

    Default tolerance is 1.0e-6.
  */
  CoinRelFltEq()
    : epsilon_(1.e-6) {}; // as float
#endif

  //! Alternate constructor with epsilon as a parameter

  CoinRelFltEq(const FloatT epsilon)
    : epsilon_(epsilon)
  {
  }

  //! Destructor

  virtual ~CoinRelFltEq() {}

  //! Copy constructor

  CoinRelFltEq(const CoinRelFltEq &src)
    : epsilon_(src.epsilon_)
  {
  }

  //! Assignment

  CoinRelFltEq &operator=(const CoinRelFltEq &rhs)

  {
    if (this != &rhs)
      epsilon_ = rhs.epsilon_;
    return (*this);
  }

  //@}

private:
  /*! \name Private member data */
  //@{

  //! Base equality tolerance

  FloatT epsilon_;

  //@}
};

#endif

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/
