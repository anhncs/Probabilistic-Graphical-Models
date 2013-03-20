/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/varset.h>
#include <vector>
#include <map>


using namespace dai;


#define BOOST_TEST_MODULE VarSetTest


#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( ConstructorsTest ) {
    Var v1( 1, 2 );
    Var v2( 2, 2 );
    Var v3( 3, 2 );
    Var v4( 4, 2 );
    Var v5( 5, 2 );

    // check constructors
    VarSet x;
    BOOST_CHECK( x.empty() );
    BOOST_CHECK_EQUAL( x.size(), 0 );
    BOOST_CHECK( !x.contains( v1 ) );

    VarSet y( v1 );
    BOOST_CHECK( !y.empty() );
    BOOST_CHECK_EQUAL( y.size(), 1 );
    BOOST_CHECK( !y.contains( v2 ) );
    BOOST_CHECK( y.contains( v1 ) );

    VarSet z( v1, v2 );
    BOOST_CHECK_EQUAL( z.size(), 2 );
    BOOST_CHECK( !z.contains( v3 ) );
    BOOST_CHECK( z.contains( v1 ) );
    BOOST_CHECK( z.contains( v2 ) );

    VarSet v( v1, v1 );
    BOOST_CHECK_EQUAL( v.size(), 1 );
    BOOST_CHECK( v.contains( v1 ) );
    BOOST_CHECK( !v.contains( v2 ) );

    std::vector<Var> a;
    a.push_back( v1 );
    a.push_back( v3 );
    a.push_back( v4 );
    a.push_back( v3 );
    a.push_back( v2 );
    VarSet w( a.begin(), a.end() );
    BOOST_CHECK_EQUAL( w.size(), 4 );
    BOOST_CHECK( w.contains( v1 ) );
    BOOST_CHECK( w.contains( v2 ) );
    BOOST_CHECK( w.contains( v3 ) );
    BOOST_CHECK( w.contains( v4 ) );
    BOOST_CHECK( !w.contains( v5 ) );

    x = y;
    BOOST_CHECK( x == y );
}


BOOST_AUTO_TEST_CASE( ComparisonTest ) {
    // check comparison operators
    Var v1( 1, 2 );
    Var v2( 2, 2 );
    Var v3( 3, 2 );

    VarSet x, y;
    BOOST_CHECK( x == y );
    BOOST_CHECK( !(x != y) );

    VarSet z( v1 );
    BOOST_CHECK( x != z );
    BOOST_CHECK( !(x == z) );

    BOOST_CHECK( x < z );

    VarSet u( v2 );
    BOOST_CHECK( x < u );
    BOOST_CHECK( z < u );
    BOOST_CHECK( !(u < z) );

    VarSet v( v2, v1 );
    BOOST_CHECK( x < v );
    BOOST_CHECK( z < v );
    BOOST_CHECK( !(v < z) );
    BOOST_CHECK( v < u );
    BOOST_CHECK( !(u < v) );

    VarSet w( v1, v3 );
    BOOST_CHECK( x < w );
    BOOST_CHECK( v < w );
    BOOST_CHECK( z < w );
    BOOST_CHECK( w < u );
    BOOST_CHECK( !(w < x) );
    BOOST_CHECK( !(w < v) );
    BOOST_CHECK( !(w < z) );
    BOOST_CHECK( !(u < w) );
}


BOOST_AUTO_TEST_CASE( IteratorTest ) {
    // check iterator interface
    std::vector<Var> v;
    Var v1( 1, 2 );
    Var v2( 2, 2 );
    Var v3( 3, 2 );
    v.push_back( v3 );
    v.push_back( v2 );
    v.push_back( v2 );
    v.push_back( v1 );
    v.push_back( v3 );
    VarSet x( v.begin(), v.end(), v.size() );

    BOOST_CHECK( x.begin() != x.end() );
    BOOST_CHECK( x.rbegin() != x.rend() );

    BOOST_CHECK_EQUAL( x.front(), v1 );
    BOOST_CHECK_EQUAL( x.back(), v3 );

    VarSet::const_iterator j = x.begin();
    VarSet::iterator k = x.begin();
    for( size_t i = 1; i < 4; i++, j++, k++ ) {
        BOOST_CHECK_EQUAL( i, j->label() );
        BOOST_CHECK_EQUAL( i, k->label() );
    }
    BOOST_CHECK( j == x.end() );
    BOOST_CHECK( k == x.end() );

    VarSet::const_reverse_iterator rj = x.rbegin();
    VarSet::reverse_iterator rk = x.rbegin();
    for( size_t i = 3; i > 0; i--, rj++, rk++ ) {
        BOOST_CHECK_EQUAL( i, rj->label() );
        BOOST_CHECK_EQUAL( i, rk->label() );
    }
    BOOST_CHECK( rj == static_cast<VarSet::const_reverse_iterator>(x.rend()) );
    BOOST_CHECK( rk == x.rend() );
}


BOOST_AUTO_TEST_CASE( QueriesTest ) {
    Var v1( 1, 2 );
    Var v2( 2, 2 );
    Var v3( 3, 2 );
    Var v4( 4, 2 );

    // check queries
    VarSet x( v2, v1 );
    VarSet y( v2, v3 );
    VarSet z( v4, v3 );

    BOOST_CHECK_EQUAL( x.nrStates(), 4 );
    BOOST_CHECK_EQUAL( y.nrStates(), 4 );
    BOOST_CHECK_EQUAL( z.nrStates(), 4 );

    BOOST_CHECK( x.intersects( y ) );
    BOOST_CHECK( !x.intersects( z ) );
    BOOST_CHECK( y.intersects( x ) );
    BOOST_CHECK( y.intersects( z ) );
    BOOST_CHECK( !z.intersects( x ) );
    BOOST_CHECK( z.intersects( y ) );

    BOOST_CHECK( x.contains( v1 ) );
    BOOST_CHECK( x.contains( v2 ) );
    BOOST_CHECK( !x.contains( v3 ) );
    BOOST_CHECK( !x.contains( v4 ) );
    BOOST_CHECK( !y.contains( v1 ) );
    BOOST_CHECK( y.contains( v2 ) );
    BOOST_CHECK( y.contains( v3 ) );
    BOOST_CHECK( !y.contains( v4 ) );
    BOOST_CHECK( !z.contains( v1 ) );
    BOOST_CHECK( !z.contains( v2 ) );
    BOOST_CHECK( z.contains( v3 ) );
    BOOST_CHECK( z.contains( v4 ) );

    BOOST_CHECK_EQUAL( x.size(), 2 );

    BOOST_CHECK( !x.empty() );
    
    x.elements()[0] = v3;
    x.elements()[1] = v4;
    std::vector<Var> v;
    v.push_back( v3 );
    v.push_back( v4 );
    BOOST_CHECK( x.elements() == v );
}


BOOST_AUTO_TEST_CASE( OperatorTest ) {
    Var v1( 1, 2 );
    Var v2( 2, 2 );
    Var v3( 3, 2 );

    // check operators
    VarSet x;
    VarSet x1( v1 );
    VarSet x2( v2 );
    VarSet x3( v3 );
    VarSet x12( v2, v1 );
    VarSet x23( v2, v3 );
    VarSet x13( v1, v3 );
    VarSet x123 = x12 | v3;
    VarSet y;

    // check operator/
    BOOST_CHECK_EQUAL( x    / x,    x    );
    BOOST_CHECK_EQUAL( x    / x1,   x    );
    BOOST_CHECK_EQUAL( x    / x2,   x    );
    BOOST_CHECK_EQUAL( x    / x3,   x    );
    BOOST_CHECK_EQUAL( x    / x12,  x    );
    BOOST_CHECK_EQUAL( x    / x23,  x    );
    BOOST_CHECK_EQUAL( x    / x13,  x    );
    BOOST_CHECK_EQUAL( x    / x123, x    );
    BOOST_CHECK_EQUAL( x1   / x,    x1   );
    BOOST_CHECK_EQUAL( x1   / x1,   x    );
    BOOST_CHECK_EQUAL( x1   / x2,   x1   );
    BOOST_CHECK_EQUAL( x1   / x3,   x1   );
    BOOST_CHECK_EQUAL( x1   / x12,  x    );
    BOOST_CHECK_EQUAL( x1   / x23,  x1   );
    BOOST_CHECK_EQUAL( x1   / x13,  x    );
    BOOST_CHECK_EQUAL( x1   / x123, x    );
    BOOST_CHECK_EQUAL( x2   / x,    x2   );
    BOOST_CHECK_EQUAL( x2   / x1,   x2   );
    BOOST_CHECK_EQUAL( x2   / x2,   x    );
    BOOST_CHECK_EQUAL( x2   / x3,   x2   );
    BOOST_CHECK_EQUAL( x2   / x12,  x    );
    BOOST_CHECK_EQUAL( x2   / x23,  x    );
    BOOST_CHECK_EQUAL( x2   / x13,  x2   );
    BOOST_CHECK_EQUAL( x2   / x123, x    );
    BOOST_CHECK_EQUAL( x3   / x,    x3   );
    BOOST_CHECK_EQUAL( x3   / x1,   x3   );
    BOOST_CHECK_EQUAL( x3   / x2,   x3   );
    BOOST_CHECK_EQUAL( x3   / x3,   x    );
    BOOST_CHECK_EQUAL( x3   / x12,  x3   );
    BOOST_CHECK_EQUAL( x3   / x23,  x    );
    BOOST_CHECK_EQUAL( x3   / x13,  x    );
    BOOST_CHECK_EQUAL( x3   / x123, x    );
    BOOST_CHECK_EQUAL( x12  / x,    x12  );
    BOOST_CHECK_EQUAL( x12  / x1,   x2   );
    BOOST_CHECK_EQUAL( x12  / x2,   x1   );
    BOOST_CHECK_EQUAL( x12  / x3,   x12  );
    BOOST_CHECK_EQUAL( x12  / x12,  x    );
    BOOST_CHECK_EQUAL( x12  / x23,  x1   );
    BOOST_CHECK_EQUAL( x12  / x13,  x2   );
    BOOST_CHECK_EQUAL( x12  / x123, x    );
    BOOST_CHECK_EQUAL( x23  / x,    x23  );
    BOOST_CHECK_EQUAL( x23  / x1,   x23  );
    BOOST_CHECK_EQUAL( x23  / x2,   x3   );
    BOOST_CHECK_EQUAL( x23  / x3,   x2   );
    BOOST_CHECK_EQUAL( x23  / x12,  x3   );
    BOOST_CHECK_EQUAL( x23  / x23,  x    );
    BOOST_CHECK_EQUAL( x23  / x13,  x2   );
    BOOST_CHECK_EQUAL( x23  / x123, x    );
    BOOST_CHECK_EQUAL( x13  / x,    x13  );
    BOOST_CHECK_EQUAL( x13  / x1,   x3   );
    BOOST_CHECK_EQUAL( x13  / x2,   x13  );
    BOOST_CHECK_EQUAL( x13  / x3,   x1   );
    BOOST_CHECK_EQUAL( x13  / x12,  x3   );
    BOOST_CHECK_EQUAL( x13  / x23,  x1   );
    BOOST_CHECK_EQUAL( x13  / x13,  x    );
    BOOST_CHECK_EQUAL( x13  / x123, x    );
    BOOST_CHECK_EQUAL( x123 / x,    x123 );
    BOOST_CHECK_EQUAL( x123 / x1,   x23  );
    BOOST_CHECK_EQUAL( x123 / x2,   x13  );
    BOOST_CHECK_EQUAL( x123 / x3,   x12  );
    BOOST_CHECK_EQUAL( x123 / x12,  x3   );
    BOOST_CHECK_EQUAL( x123 / x23,  x1   );
    BOOST_CHECK_EQUAL( x123 / x13,  x2   );
    BOOST_CHECK_EQUAL( x123 / x123, x    );

    // check operator|
    BOOST_CHECK_EQUAL( x    | x,    x    );
    BOOST_CHECK_EQUAL( x    | x1,   x1   );
    BOOST_CHECK_EQUAL( x    | x2,   x2   );
    BOOST_CHECK_EQUAL( x    | x3,   x3   );
    BOOST_CHECK_EQUAL( x    | x12,  x12  );
    BOOST_CHECK_EQUAL( x    | x23,  x23  );
    BOOST_CHECK_EQUAL( x    | x13,  x13  );
    BOOST_CHECK_EQUAL( x    | x123, x123 );
    BOOST_CHECK_EQUAL( x1   | x,    x1   );
    BOOST_CHECK_EQUAL( x1   | x1,   x1   );
    BOOST_CHECK_EQUAL( x1   | x2,   x12  );
    BOOST_CHECK_EQUAL( x1   | x3,   x13  );
    BOOST_CHECK_EQUAL( x1   | x12,  x12  );
    BOOST_CHECK_EQUAL( x1   | x23,  x123 );
    BOOST_CHECK_EQUAL( x1   | x13,  x13  );
    BOOST_CHECK_EQUAL( x1   | x123, x123 );
    BOOST_CHECK_EQUAL( x2   | x,    x2   );
    BOOST_CHECK_EQUAL( x2   | x1,   x12  );
    BOOST_CHECK_EQUAL( x2   | x2,   x2   );
    BOOST_CHECK_EQUAL( x2   | x3,   x23  );
    BOOST_CHECK_EQUAL( x2   | x12,  x12  );
    BOOST_CHECK_EQUAL( x2   | x23,  x23  );
    BOOST_CHECK_EQUAL( x2   | x13,  x123 );
    BOOST_CHECK_EQUAL( x2   | x123, x123 );
    BOOST_CHECK_EQUAL( x3   | x,    x3   );
    BOOST_CHECK_EQUAL( x3   | x1,   x13  );
    BOOST_CHECK_EQUAL( x3   | x2,   x23  );
    BOOST_CHECK_EQUAL( x3   | x3,   x3   );
    BOOST_CHECK_EQUAL( x3   | x12,  x123 );
    BOOST_CHECK_EQUAL( x3   | x23,  x23  );
    BOOST_CHECK_EQUAL( x3   | x13,  x13  );
    BOOST_CHECK_EQUAL( x3   | x123, x123 );
    BOOST_CHECK_EQUAL( x12  | x,    x12  );
    BOOST_CHECK_EQUAL( x12  | x1,   x12  );
    BOOST_CHECK_EQUAL( x12  | x2,   x12  );
    BOOST_CHECK_EQUAL( x12  | x3,   x123 );
    BOOST_CHECK_EQUAL( x12  | x12,  x12  );
    BOOST_CHECK_EQUAL( x12  | x23,  x123 );
    BOOST_CHECK_EQUAL( x12  | x13,  x123 );
    BOOST_CHECK_EQUAL( x12  | x123, x123 );
    BOOST_CHECK_EQUAL( x23  | x,    x23  );
    BOOST_CHECK_EQUAL( x23  | x1,   x123 );
    BOOST_CHECK_EQUAL( x23  | x2,   x23  );
    BOOST_CHECK_EQUAL( x23  | x3,   x23  );
    BOOST_CHECK_EQUAL( x23  | x12,  x123 );
    BOOST_CHECK_EQUAL( x23  | x23,  x23  );
    BOOST_CHECK_EQUAL( x23  | x13,  x123 );
    BOOST_CHECK_EQUAL( x23  | x123, x123 );
    BOOST_CHECK_EQUAL( x13  | x,    x13  );
    BOOST_CHECK_EQUAL( x13  | x1,   x13  );
    BOOST_CHECK_EQUAL( x13  | x2,   x123 );
    BOOST_CHECK_EQUAL( x13  | x3,   x13  );
    BOOST_CHECK_EQUAL( x13  | x12,  x123 );
    BOOST_CHECK_EQUAL( x13  | x23,  x123 );
    BOOST_CHECK_EQUAL( x13  | x13,  x13  );
    BOOST_CHECK_EQUAL( x13  | x123, x123 );
    BOOST_CHECK_EQUAL( x123 | x,    x123 );
    BOOST_CHECK_EQUAL( x123 | x1,   x123 );
    BOOST_CHECK_EQUAL( x123 | x2,   x123 );
    BOOST_CHECK_EQUAL( x123 | x3,   x123 );
    BOOST_CHECK_EQUAL( x123 | x12,  x123 );
    BOOST_CHECK_EQUAL( x123 | x23,  x123 );
    BOOST_CHECK_EQUAL( x123 | x13,  x123 );
    BOOST_CHECK_EQUAL( x123 | x123, x123 );

    // check operator&
    BOOST_CHECK_EQUAL( x    & x,    x    );
    BOOST_CHECK_EQUAL( x    & x1,   x    );
    BOOST_CHECK_EQUAL( x    & x2,   x    );
    BOOST_CHECK_EQUAL( x    & x3,   x    );
    BOOST_CHECK_EQUAL( x    & x12,  x    );
    BOOST_CHECK_EQUAL( x    & x23,  x    );
    BOOST_CHECK_EQUAL( x    & x13,  x    );
    BOOST_CHECK_EQUAL( x    & x123, x    );
    BOOST_CHECK_EQUAL( x1   & x,    x    );
    BOOST_CHECK_EQUAL( x1   & x1,   x1   );
    BOOST_CHECK_EQUAL( x1   & x2,   x    );
    BOOST_CHECK_EQUAL( x1   & x3,   x    );
    BOOST_CHECK_EQUAL( x1   & x12,  x1   );
    BOOST_CHECK_EQUAL( x1   & x23,  x    );
    BOOST_CHECK_EQUAL( x1   & x13,  x1   );
    BOOST_CHECK_EQUAL( x1   & x123, x1   );
    BOOST_CHECK_EQUAL( x2   & x,    x    );
    BOOST_CHECK_EQUAL( x2   & x1,   x    );
    BOOST_CHECK_EQUAL( x2   & x2,   x2   );
    BOOST_CHECK_EQUAL( x2   & x3,   x    );
    BOOST_CHECK_EQUAL( x2   & x12,  x2   );
    BOOST_CHECK_EQUAL( x2   & x23,  x2   );
    BOOST_CHECK_EQUAL( x2   & x13,  x    );
    BOOST_CHECK_EQUAL( x2   & x123, x2   );
    BOOST_CHECK_EQUAL( x3   & x,    x    );
    BOOST_CHECK_EQUAL( x3   & x1,   x    );
    BOOST_CHECK_EQUAL( x3   & x2,   x    );
    BOOST_CHECK_EQUAL( x3   & x3,   x3   );
    BOOST_CHECK_EQUAL( x3   & x12,  x    );
    BOOST_CHECK_EQUAL( x3   & x23,  x3   );
    BOOST_CHECK_EQUAL( x3   & x13,  x3   );
    BOOST_CHECK_EQUAL( x3   & x123, x3   );
    BOOST_CHECK_EQUAL( x12  & x,    x    );
    BOOST_CHECK_EQUAL( x12  & x1,   x1   );
    BOOST_CHECK_EQUAL( x12  & x2,   x2   );
    BOOST_CHECK_EQUAL( x12  & x3,   x    );
    BOOST_CHECK_EQUAL( x12  & x12,  x12  );
    BOOST_CHECK_EQUAL( x12  & x23,  x2   );
    BOOST_CHECK_EQUAL( x12  & x13,  x1   );
    BOOST_CHECK_EQUAL( x12  & x123, x12  );
    BOOST_CHECK_EQUAL( x23  & x,    x    );
    BOOST_CHECK_EQUAL( x23  & x1,   x    );
    BOOST_CHECK_EQUAL( x23  & x2,   x2   );
    BOOST_CHECK_EQUAL( x23  & x3,   x3   );
    BOOST_CHECK_EQUAL( x23  & x12,  x2   );
    BOOST_CHECK_EQUAL( x23  & x23,  x23  );
    BOOST_CHECK_EQUAL( x23  & x13,  x3   );
    BOOST_CHECK_EQUAL( x23  & x123, x23  );
    BOOST_CHECK_EQUAL( x13  & x,    x    );
    BOOST_CHECK_EQUAL( x13  & x1,   x1   );
    BOOST_CHECK_EQUAL( x13  & x2,   x    );
    BOOST_CHECK_EQUAL( x13  & x3,   x3   );
    BOOST_CHECK_EQUAL( x13  & x12,  x1   );
    BOOST_CHECK_EQUAL( x13  & x23,  x3   );
    BOOST_CHECK_EQUAL( x13  & x13,  x13  );
    BOOST_CHECK_EQUAL( x13  & x123, x13  );
    BOOST_CHECK_EQUAL( x123 & x,    x    );
    BOOST_CHECK_EQUAL( x123 & x1,   x1   );
    BOOST_CHECK_EQUAL( x123 & x2,   x2   );
    BOOST_CHECK_EQUAL( x123 & x3,   x3   );
    BOOST_CHECK_EQUAL( x123 & x12,  x12  );
    BOOST_CHECK_EQUAL( x123 & x23,  x23  );
    BOOST_CHECK_EQUAL( x123 & x13,  x13  );
    BOOST_CHECK_EQUAL( x123 & x123, x123 );

    // check operator/=
    y = x   ; y /= x;    BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y /= x1;   BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y /= x2;   BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y /= x3;   BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y /= x12;  BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y /= x23;  BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y /= x13;  BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y /= x123; BOOST_CHECK_EQUAL( y, x    );
    y = x1  ; y /= x;    BOOST_CHECK_EQUAL( y, x1   );
    y = x1  ; y /= x1;   BOOST_CHECK_EQUAL( y, x    );
    y = x1  ; y /= x2;   BOOST_CHECK_EQUAL( y, x1   );
    y = x1  ; y /= x3;   BOOST_CHECK_EQUAL( y, x1   );
    y = x1  ; y /= x12;  BOOST_CHECK_EQUAL( y, x    );
    y = x1  ; y /= x23;  BOOST_CHECK_EQUAL( y, x1   );
    y = x1  ; y /= x13;  BOOST_CHECK_EQUAL( y, x    );
    y = x1  ; y /= x123; BOOST_CHECK_EQUAL( y, x    );
    y = x2  ; y /= x;    BOOST_CHECK_EQUAL( y, x2   );
    y = x2  ; y /= x1;   BOOST_CHECK_EQUAL( y, x2   );
    y = x2  ; y /= x2;   BOOST_CHECK_EQUAL( y, x    );
    y = x2  ; y /= x3;   BOOST_CHECK_EQUAL( y, x2   );
    y = x2  ; y /= x12;  BOOST_CHECK_EQUAL( y, x    );
    y = x2  ; y /= x23;  BOOST_CHECK_EQUAL( y, x    );
    y = x2  ; y /= x13;  BOOST_CHECK_EQUAL( y, x2   );
    y = x2  ; y /= x123; BOOST_CHECK_EQUAL( y, x    );
    y = x3  ; y /= x;    BOOST_CHECK_EQUAL( y, x3   );
    y = x3  ; y /= x1;   BOOST_CHECK_EQUAL( y, x3   );
    y = x3  ; y /= x2;   BOOST_CHECK_EQUAL( y, x3   );
    y = x3  ; y /= x3;   BOOST_CHECK_EQUAL( y, x    );
    y = x3  ; y /= x12;  BOOST_CHECK_EQUAL( y, x3   );
    y = x3  ; y /= x23;  BOOST_CHECK_EQUAL( y, x    );
    y = x3  ; y /= x13;  BOOST_CHECK_EQUAL( y, x    );
    y = x3  ; y /= x123; BOOST_CHECK_EQUAL( y, x    );
    y = x12 ; y /= x;    BOOST_CHECK_EQUAL( y, x12  );
    y = x12 ; y /= x1;   BOOST_CHECK_EQUAL( y, x2   );
    y = x12 ; y /= x2;   BOOST_CHECK_EQUAL( y, x1   );
    y = x12 ; y /= x3;   BOOST_CHECK_EQUAL( y, x12  );
    y = x12 ; y /= x12;  BOOST_CHECK_EQUAL( y, x    );
    y = x12 ; y /= x23;  BOOST_CHECK_EQUAL( y, x1   );
    y = x12 ; y /= x13;  BOOST_CHECK_EQUAL( y, x2   );
    y = x12 ; y /= x123; BOOST_CHECK_EQUAL( y, x    );
    y = x23 ; y /= x;    BOOST_CHECK_EQUAL( y, x23  );
    y = x23 ; y /= x1;   BOOST_CHECK_EQUAL( y, x23  );
    y = x23 ; y /= x2;   BOOST_CHECK_EQUAL( y, x3   );
    y = x23 ; y /= x3;   BOOST_CHECK_EQUAL( y, x2   );
    y = x23 ; y /= x12;  BOOST_CHECK_EQUAL( y, x3   );
    y = x23 ; y /= x23;  BOOST_CHECK_EQUAL( y, x    );
    y = x23 ; y /= x13;  BOOST_CHECK_EQUAL( y, x2   );
    y = x23 ; y /= x123; BOOST_CHECK_EQUAL( y, x    );
    y = x13 ; y /= x;    BOOST_CHECK_EQUAL( y, x13  );
    y = x13 ; y /= x1;   BOOST_CHECK_EQUAL( y, x3   );
    y = x13 ; y /= x2;   BOOST_CHECK_EQUAL( y, x13  );
    y = x13 ; y /= x3;   BOOST_CHECK_EQUAL( y, x1   );
    y = x13 ; y /= x12;  BOOST_CHECK_EQUAL( y, x3   );
    y = x13 ; y /= x23;  BOOST_CHECK_EQUAL( y, x1   );
    y = x13 ; y /= x13;  BOOST_CHECK_EQUAL( y, x    );
    y = x13 ; y /= x123; BOOST_CHECK_EQUAL( y, x    );
    y = x123; y /= x;    BOOST_CHECK_EQUAL( y, x123 );
    y = x123; y /= x1;   BOOST_CHECK_EQUAL( y, x23  );
    y = x123; y /= x2;   BOOST_CHECK_EQUAL( y, x13  );
    y = x123; y /= x3;   BOOST_CHECK_EQUAL( y, x12  );
    y = x123; y /= x12;  BOOST_CHECK_EQUAL( y, x3   );
    y = x123; y /= x23;  BOOST_CHECK_EQUAL( y, x1   );
    y = x123; y /= x13;  BOOST_CHECK_EQUAL( y, x2   );
    y = x123; y /= x123; BOOST_CHECK_EQUAL( y, x    );

    // check operator|=
    y = x   ; y |= x;    BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y |= x1;   BOOST_CHECK_EQUAL( y, x1   );
    y = x   ; y |= x2;   BOOST_CHECK_EQUAL( y, x2   );
    y = x   ; y |= x3;   BOOST_CHECK_EQUAL( y, x3   );
    y = x   ; y |= x12;  BOOST_CHECK_EQUAL( y, x12  );
    y = x   ; y |= x23;  BOOST_CHECK_EQUAL( y, x23  );
    y = x   ; y |= x13;  BOOST_CHECK_EQUAL( y, x13  );
    y = x   ; y |= x123; BOOST_CHECK_EQUAL( y, x123 );
    y = x1  ; y |= x;    BOOST_CHECK_EQUAL( y, x1   );
    y = x1  ; y |= x1;   BOOST_CHECK_EQUAL( y, x1   );
    y = x1  ; y |= x2;   BOOST_CHECK_EQUAL( y, x12  );
    y = x1  ; y |= x3;   BOOST_CHECK_EQUAL( y, x13  );
    y = x1  ; y |= x12;  BOOST_CHECK_EQUAL( y, x12  );
    y = x1  ; y |= x23;  BOOST_CHECK_EQUAL( y, x123 );
    y = x1  ; y |= x13;  BOOST_CHECK_EQUAL( y, x13  );
    y = x1  ; y |= x123; BOOST_CHECK_EQUAL( y, x123 );
    y = x2  ; y |= x;    BOOST_CHECK_EQUAL( y, x2   );
    y = x2  ; y |= x1;   BOOST_CHECK_EQUAL( y, x12  );
    y = x2  ; y |= x2;   BOOST_CHECK_EQUAL( y, x2   );
    y = x2  ; y |= x3;   BOOST_CHECK_EQUAL( y, x23  );
    y = x2  ; y |= x12;  BOOST_CHECK_EQUAL( y, x12  );
    y = x2  ; y |= x23;  BOOST_CHECK_EQUAL( y, x23  );
    y = x2  ; y |= x13;  BOOST_CHECK_EQUAL( y, x123 );
    y = x2  ; y |= x123; BOOST_CHECK_EQUAL( y, x123 );
    y = x3  ; y |= x;    BOOST_CHECK_EQUAL( y, x3   );
    y = x3  ; y |= x1;   BOOST_CHECK_EQUAL( y, x13  );
    y = x3  ; y |= x2;   BOOST_CHECK_EQUAL( y, x23  );
    y = x3  ; y |= x3;   BOOST_CHECK_EQUAL( y, x3   );
    y = x3  ; y |= x12;  BOOST_CHECK_EQUAL( y, x123 );
    y = x3  ; y |= x23;  BOOST_CHECK_EQUAL( y, x23  );
    y = x3  ; y |= x13;  BOOST_CHECK_EQUAL( y, x13  );
    y = x3  ; y |= x123; BOOST_CHECK_EQUAL( y, x123 );
    y = x12 ; y |= x;    BOOST_CHECK_EQUAL( y, x12  );
    y = x12 ; y |= x1;   BOOST_CHECK_EQUAL( y, x12  );
    y = x12 ; y |= x2;   BOOST_CHECK_EQUAL( y, x12  );
    y = x12 ; y |= x3;   BOOST_CHECK_EQUAL( y, x123 );
    y = x12 ; y |= x12;  BOOST_CHECK_EQUAL( y, x12  );
    y = x12 ; y |= x23;  BOOST_CHECK_EQUAL( y, x123 );
    y = x12 ; y |= x13;  BOOST_CHECK_EQUAL( y, x123 );
    y = x12 ; y |= x123; BOOST_CHECK_EQUAL( y, x123 );
    y = x23 ; y |= x;    BOOST_CHECK_EQUAL( y, x23  );
    y = x23 ; y |= x1;   BOOST_CHECK_EQUAL( y, x123 );
    y = x23 ; y |= x2;   BOOST_CHECK_EQUAL( y, x23  );
    y = x23 ; y |= x3;   BOOST_CHECK_EQUAL( y, x23  );
    y = x23 ; y |= x12;  BOOST_CHECK_EQUAL( y, x123 );
    y = x23 ; y |= x23;  BOOST_CHECK_EQUAL( y, x23  );
    y = x23 ; y |= x13;  BOOST_CHECK_EQUAL( y, x123 );
    y = x23 ; y |= x123; BOOST_CHECK_EQUAL( y, x123 );
    y = x13 ; y |= x;    BOOST_CHECK_EQUAL( y, x13  );
    y = x13 ; y |= x1;   BOOST_CHECK_EQUAL( y, x13  );
    y = x13 ; y |= x2;   BOOST_CHECK_EQUAL( y, x123 );
    y = x13 ; y |= x3;   BOOST_CHECK_EQUAL( y, x13  );
    y = x13 ; y |= x12;  BOOST_CHECK_EQUAL( y, x123 );
    y = x13 ; y |= x23;  BOOST_CHECK_EQUAL( y, x123 );
    y = x13 ; y |= x13;  BOOST_CHECK_EQUAL( y, x13  );
    y = x13 ; y |= x123; BOOST_CHECK_EQUAL( y, x123 );
    y = x123; y |= x;    BOOST_CHECK_EQUAL( y, x123 );
    y = x123; y |= x1;   BOOST_CHECK_EQUAL( y, x123 );
    y = x123; y |= x2;   BOOST_CHECK_EQUAL( y, x123 );
    y = x123; y |= x3;   BOOST_CHECK_EQUAL( y, x123 );
    y = x123; y |= x12;  BOOST_CHECK_EQUAL( y, x123 );
    y = x123; y |= x23;  BOOST_CHECK_EQUAL( y, x123 );
    y = x123; y |= x13;  BOOST_CHECK_EQUAL( y, x123 );
    y = x123; y |= x123; BOOST_CHECK_EQUAL( y, x123 );

    // check operator&=
    y = x   ; y &= x;    BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y &= x1;   BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y &= x2;   BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y &= x3;   BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y &= x12;  BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y &= x23;  BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y &= x13;  BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y &= x123; BOOST_CHECK_EQUAL( y, x    );
    y = x1  ; y &= x;    BOOST_CHECK_EQUAL( y, x    );
    y = x1  ; y &= x1;   BOOST_CHECK_EQUAL( y, x1   );
    y = x1  ; y &= x2;   BOOST_CHECK_EQUAL( y, x    );
    y = x1  ; y &= x3;   BOOST_CHECK_EQUAL( y, x    );
    y = x1  ; y &= x12;  BOOST_CHECK_EQUAL( y, x1   );
    y = x1  ; y &= x23;  BOOST_CHECK_EQUAL( y, x    );
    y = x1  ; y &= x13;  BOOST_CHECK_EQUAL( y, x1   );
    y = x1  ; y &= x123; BOOST_CHECK_EQUAL( y, x1   );
    y = x2  ; y &= x;    BOOST_CHECK_EQUAL( y, x    );
    y = x2  ; y &= x1;   BOOST_CHECK_EQUAL( y, x    );
    y = x2  ; y &= x2;   BOOST_CHECK_EQUAL( y, x2   );
    y = x2  ; y &= x3;   BOOST_CHECK_EQUAL( y, x    );
    y = x2  ; y &= x12;  BOOST_CHECK_EQUAL( y, x2   );
    y = x2  ; y &= x23;  BOOST_CHECK_EQUAL( y, x2   );
    y = x2  ; y &= x13;  BOOST_CHECK_EQUAL( y, x    );
    y = x2  ; y &= x123; BOOST_CHECK_EQUAL( y, x2   );
    y = x3  ; y &= x;    BOOST_CHECK_EQUAL( y, x    );
    y = x3  ; y &= x1;   BOOST_CHECK_EQUAL( y, x    );
    y = x3  ; y &= x2;   BOOST_CHECK_EQUAL( y, x    );
    y = x3  ; y &= x3;   BOOST_CHECK_EQUAL( y, x3   );
    y = x3  ; y &= x12;  BOOST_CHECK_EQUAL( y, x    );
    y = x3  ; y &= x23;  BOOST_CHECK_EQUAL( y, x3   );
    y = x3  ; y &= x13;  BOOST_CHECK_EQUAL( y, x3   );
    y = x3  ; y &= x123; BOOST_CHECK_EQUAL( y, x3   );
    y = x12 ; y &= x;    BOOST_CHECK_EQUAL( y, x    );
    y = x12 ; y &= x1;   BOOST_CHECK_EQUAL( y, x1   );
    y = x12 ; y &= x2;   BOOST_CHECK_EQUAL( y, x2   );
    y = x12 ; y &= x3;   BOOST_CHECK_EQUAL( y, x    );
    y = x12 ; y &= x12;  BOOST_CHECK_EQUAL( y, x12  );
    y = x12 ; y &= x23;  BOOST_CHECK_EQUAL( y, x2   );
    y = x12 ; y &= x13;  BOOST_CHECK_EQUAL( y, x1   );
    y = x12 ; y &= x123; BOOST_CHECK_EQUAL( y, x12  );
    y = x23 ; y &= x;    BOOST_CHECK_EQUAL( y, x    );
    y = x23 ; y &= x1;   BOOST_CHECK_EQUAL( y, x    );
    y = x23 ; y &= x2;   BOOST_CHECK_EQUAL( y, x2   );
    y = x23 ; y &= x3;   BOOST_CHECK_EQUAL( y, x3   );
    y = x23 ; y &= x12;  BOOST_CHECK_EQUAL( y, x2   );
    y = x23 ; y &= x23;  BOOST_CHECK_EQUAL( y, x23  );
    y = x23 ; y &= x13;  BOOST_CHECK_EQUAL( y, x3   );
    y = x23 ; y &= x123; BOOST_CHECK_EQUAL( y, x23  );
    y = x13 ; y &= x;    BOOST_CHECK_EQUAL( y, x    );
    y = x13 ; y &= x1;   BOOST_CHECK_EQUAL( y, x1   );
    y = x13 ; y &= x2;   BOOST_CHECK_EQUAL( y, x    );
    y = x13 ; y &= x3;   BOOST_CHECK_EQUAL( y, x3   );
    y = x13 ; y &= x12;  BOOST_CHECK_EQUAL( y, x1   );
    y = x13 ; y &= x23;  BOOST_CHECK_EQUAL( y, x3   );
    y = x13 ; y &= x13;  BOOST_CHECK_EQUAL( y, x13  );
    y = x13 ; y &= x123; BOOST_CHECK_EQUAL( y, x13  );
    y = x123; y &= x;    BOOST_CHECK_EQUAL( y, x    );
    y = x123; y &= x1;   BOOST_CHECK_EQUAL( y, x1   );
    y = x123; y &= x2;   BOOST_CHECK_EQUAL( y, x2   );
    y = x123; y &= x3;   BOOST_CHECK_EQUAL( y, x3   );
    y = x123; y &= x12;  BOOST_CHECK_EQUAL( y, x12  );
    y = x123; y &= x23;  BOOST_CHECK_EQUAL( y, x23  );
    y = x123; y &= x13;  BOOST_CHECK_EQUAL( y, x13  );
    y = x123; y &= x123; BOOST_CHECK_EQUAL( y, x123 );

    // check operator/=
    y = x   ; y /= v1;    BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y /= v2;    BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y /= v3;    BOOST_CHECK_EQUAL( y, x    );
    y = x1  ; y /= v1;    BOOST_CHECK_EQUAL( y, x    );
    y = x1  ; y /= v2;    BOOST_CHECK_EQUAL( y, x1   );
    y = x1  ; y /= v3;    BOOST_CHECK_EQUAL( y, x1   );
    y = x2  ; y /= v1;    BOOST_CHECK_EQUAL( y, x2   );
    y = x2  ; y /= v2;    BOOST_CHECK_EQUAL( y, x    );
    y = x2  ; y /= v3;    BOOST_CHECK_EQUAL( y, x2   );
    y = x3  ; y /= v1;    BOOST_CHECK_EQUAL( y, x3   );
    y = x3  ; y /= v2;    BOOST_CHECK_EQUAL( y, x3   );
    y = x3  ; y /= v3;    BOOST_CHECK_EQUAL( y, x    );
    y = x12 ; y /= v1;    BOOST_CHECK_EQUAL( y, x2   );
    y = x12 ; y /= v2;    BOOST_CHECK_EQUAL( y, x1   );
    y = x12 ; y /= v3;    BOOST_CHECK_EQUAL( y, x12  );
    y = x13 ; y /= v1;    BOOST_CHECK_EQUAL( y, x3   );
    y = x13 ; y /= v2;    BOOST_CHECK_EQUAL( y, x13  );
    y = x13 ; y /= v3;    BOOST_CHECK_EQUAL( y, x1   );
    y = x23 ; y /= v1;    BOOST_CHECK_EQUAL( y, x23  );
    y = x23 ; y /= v2;    BOOST_CHECK_EQUAL( y, x3   );
    y = x23 ; y /= v3;    BOOST_CHECK_EQUAL( y, x2   );
    y = x123; y /= v1;    BOOST_CHECK_EQUAL( y, x23  );
    y = x123; y /= v2;    BOOST_CHECK_EQUAL( y, x13  );
    y = x123; y /= v3;    BOOST_CHECK_EQUAL( y, x12  );

    // check operator|=
    y = x   ; y |= v1;    BOOST_CHECK_EQUAL( y, x1   );
    y = x   ; y |= v2;    BOOST_CHECK_EQUAL( y, x2   );
    y = x   ; y |= v3;    BOOST_CHECK_EQUAL( y, x3   );
    y = x1  ; y |= v1;    BOOST_CHECK_EQUAL( y, x1   );
    y = x1  ; y |= v2;    BOOST_CHECK_EQUAL( y, x12  );
    y = x1  ; y |= v3;    BOOST_CHECK_EQUAL( y, x13  );
    y = x2  ; y |= v1;    BOOST_CHECK_EQUAL( y, x12  );
    y = x2  ; y |= v2;    BOOST_CHECK_EQUAL( y, x2   );
    y = x2  ; y |= v3;    BOOST_CHECK_EQUAL( y, x23  );
    y = x3  ; y |= v1;    BOOST_CHECK_EQUAL( y, x13  );
    y = x3  ; y |= v2;    BOOST_CHECK_EQUAL( y, x23  );
    y = x3  ; y |= v3;    BOOST_CHECK_EQUAL( y, x3   );
    y = x12 ; y |= v1;    BOOST_CHECK_EQUAL( y, x12  );
    y = x12 ; y |= v2;    BOOST_CHECK_EQUAL( y, x12  );
    y = x12 ; y |= v3;    BOOST_CHECK_EQUAL( y, x123 );
    y = x13 ; y |= v1;    BOOST_CHECK_EQUAL( y, x13  );
    y = x13 ; y |= v2;    BOOST_CHECK_EQUAL( y, x123 );
    y = x13 ; y |= v3;    BOOST_CHECK_EQUAL( y, x13  );
    y = x23 ; y |= v1;    BOOST_CHECK_EQUAL( y, x123 );
    y = x23 ; y |= v2;    BOOST_CHECK_EQUAL( y, x23  );
    y = x23 ; y |= v3;    BOOST_CHECK_EQUAL( y, x23  );
    y = x123; y |= v1;    BOOST_CHECK_EQUAL( y, x123 );
    y = x123; y |= v2;    BOOST_CHECK_EQUAL( y, x123 );
    y = x123; y |= v3;    BOOST_CHECK_EQUAL( y, x123 );

    // check operator<<
    BOOST_CHECK(  (x    << x   ) );
    BOOST_CHECK(  (x    << x1  ) );
    BOOST_CHECK(  (x    << x2  ) );
    BOOST_CHECK(  (x    << x3  ) );
    BOOST_CHECK(  (x    << x12 ) );
    BOOST_CHECK(  (x    << x13 ) );
    BOOST_CHECK(  (x    << x23 ) );
    BOOST_CHECK(  (x    << x123) );
    BOOST_CHECK( !(x1   << x   ) );
    BOOST_CHECK(  (x1   << x1  ) );
    BOOST_CHECK( !(x1   << x2  ) );
    BOOST_CHECK( !(x1   << x3  ) );
    BOOST_CHECK(  (x1   << x12 ) );
    BOOST_CHECK(  (x1   << x13 ) );
    BOOST_CHECK( !(x1   << x23 ) );
    BOOST_CHECK(  (x1   << x123) );
    BOOST_CHECK( !(x2   << x   ) );
    BOOST_CHECK( !(x2   << x1  ) );
    BOOST_CHECK(  (x2   << x2  ) );
    BOOST_CHECK( !(x2   << x3  ) );
    BOOST_CHECK(  (x2   << x12 ) );
    BOOST_CHECK( !(x2   << x13 ) );
    BOOST_CHECK(  (x2   << x23 ) );
    BOOST_CHECK(  (x2   << x123) );
    BOOST_CHECK( !(x3   << x   ) );
    BOOST_CHECK( !(x3   << x1  ) );
    BOOST_CHECK( !(x3   << x2  ) );
    BOOST_CHECK(  (x3   << x3  ) );
    BOOST_CHECK( !(x3   << x12 ) );
    BOOST_CHECK(  (x3   << x13 ) );
    BOOST_CHECK(  (x3   << x23 ) );
    BOOST_CHECK(  (x3   << x123) );
    BOOST_CHECK( !(x12  << x   ) );
    BOOST_CHECK( !(x12  << x1  ) );
    BOOST_CHECK( !(x12  << x2  ) );
    BOOST_CHECK( !(x12  << x3  ) );
    BOOST_CHECK(  (x12  << x12 ) );
    BOOST_CHECK( !(x12  << x13 ) );
    BOOST_CHECK( !(x12  << x23 ) );
    BOOST_CHECK(  (x12  << x123) );
    BOOST_CHECK( !(x13  << x   ) );
    BOOST_CHECK( !(x13  << x1  ) );
    BOOST_CHECK( !(x13  << x2  ) );
    BOOST_CHECK( !(x13  << x3  ) );
    BOOST_CHECK( !(x13  << x12 ) );
    BOOST_CHECK(  (x13  << x13 ) );
    BOOST_CHECK( !(x13  << x23 ) );
    BOOST_CHECK(  (x13  << x123) );
    BOOST_CHECK( !(x23  << x   ) );
    BOOST_CHECK( !(x23  << x1  ) );
    BOOST_CHECK( !(x23  << x2  ) );
    BOOST_CHECK( !(x23  << x3  ) );
    BOOST_CHECK( !(x23  << x12 ) );
    BOOST_CHECK( !(x23  << x13 ) );
    BOOST_CHECK(  (x23  << x23 ) );
    BOOST_CHECK(  (x23  << x123) );
    BOOST_CHECK( !(x123 << x   ) );
    BOOST_CHECK( !(x123 << x1  ) );
    BOOST_CHECK( !(x123 << x2  ) );
    BOOST_CHECK( !(x123 << x3  ) );
    BOOST_CHECK( !(x123 << x12 ) );
    BOOST_CHECK( !(x123 << x13 ) );
    BOOST_CHECK( !(x123 << x23 ) );
    BOOST_CHECK(  (x123 << x123) );

    // check operator>>
    BOOST_CHECK(  (x    >> x   ) );
    BOOST_CHECK( !(x    >> x1  ) );
    BOOST_CHECK( !(x    >> x2  ) );
    BOOST_CHECK( !(x    >> x3  ) );
    BOOST_CHECK( !(x    >> x12 ) );
    BOOST_CHECK( !(x    >> x13 ) );
    BOOST_CHECK( !(x    >> x23 ) );
    BOOST_CHECK( !(x    >> x123) );
    BOOST_CHECK(  (x1   >> x   ) );
    BOOST_CHECK(  (x1   >> x1  ) );
    BOOST_CHECK( !(x1   >> x2  ) );
    BOOST_CHECK( !(x1   >> x3  ) );
    BOOST_CHECK( !(x1   >> x12 ) );
    BOOST_CHECK( !(x1   >> x13 ) );
    BOOST_CHECK( !(x1   >> x23 ) );
    BOOST_CHECK( !(x1   >> x123) );
    BOOST_CHECK(  (x2   >> x   ) );
    BOOST_CHECK( !(x2   >> x1  ) );
    BOOST_CHECK(  (x2   >> x2  ) );
    BOOST_CHECK( !(x2   >> x3  ) );
    BOOST_CHECK( !(x2   >> x12 ) );
    BOOST_CHECK( !(x2   >> x13 ) );
    BOOST_CHECK( !(x2   >> x23 ) );
    BOOST_CHECK( !(x2   >> x123) );
    BOOST_CHECK(  (x3   >> x   ) );
    BOOST_CHECK( !(x3   >> x1  ) );
    BOOST_CHECK( !(x3   >> x2  ) );
    BOOST_CHECK(  (x3   >> x3  ) );
    BOOST_CHECK( !(x3   >> x12 ) );
    BOOST_CHECK( !(x3   >> x13 ) );
    BOOST_CHECK( !(x3   >> x23 ) );
    BOOST_CHECK( !(x3   >> x123) );
    BOOST_CHECK(  (x12  >> x   ) );
    BOOST_CHECK(  (x12  >> x1  ) );
    BOOST_CHECK(  (x12  >> x2  ) );
    BOOST_CHECK( !(x12  >> x3  ) );
    BOOST_CHECK(  (x12  >> x12 ) );
    BOOST_CHECK( !(x12  >> x13 ) );
    BOOST_CHECK( !(x12  >> x23 ) );
    BOOST_CHECK( !(x12  >> x123) );
    BOOST_CHECK(  (x13  >> x   ) );
    BOOST_CHECK(  (x13  >> x1  ) );
    BOOST_CHECK( !(x13  >> x2  ) );
    BOOST_CHECK(  (x13  >> x3  ) );
    BOOST_CHECK( !(x13  >> x12 ) );
    BOOST_CHECK(  (x13  >> x13 ) );
    BOOST_CHECK( !(x13  >> x23 ) );
    BOOST_CHECK( !(x13  >> x123) );
    BOOST_CHECK(  (x23  >> x   ) );
    BOOST_CHECK( !(x23  >> x1  ) );
    BOOST_CHECK(  (x23  >> x2  ) );
    BOOST_CHECK(  (x23  >> x3  ) );
    BOOST_CHECK( !(x23  >> x12 ) );
    BOOST_CHECK( !(x23  >> x13 ) );
    BOOST_CHECK(  (x23  >> x23 ) );
    BOOST_CHECK( !(x23  >> x123) );
    BOOST_CHECK(  (x123 >> x   ) );
    BOOST_CHECK(  (x123 >> x1  ) );
    BOOST_CHECK(  (x123 >> x2  ) );
    BOOST_CHECK(  (x123 >> x3  ) );
    BOOST_CHECK(  (x123 >> x12 ) );
    BOOST_CHECK(  (x123 >> x13 ) );
    BOOST_CHECK(  (x123 >> x23 ) );
    BOOST_CHECK(  (x123 >> x123) );
}


BOOST_AUTO_TEST_CASE( StreamTest ) {
    Var v1( 1, 2 );
    Var v2( 2, 2 );

    // check output
    VarSet x( v1, v2 );

    std::stringstream ss;
    ss << x << std::endl;
    std::string s;
    std::getline( ss, s );
    BOOST_CHECK_EQUAL( s, "{x1, x2}" );
}


BOOST_AUTO_TEST_CASE( StateTest ) {
    // check calcState and calcLinearState
    Var x0(0, 2);
    Var x1(1, 3);

    VarSet X(x0, x1);

    size_t s = 0;
    for( size_t s1 = 0; s1 < x1.states(); s1++ )
        for( size_t s0 = 0; s0 < x0.states(); s0++ ) {
            std::map<Var,size_t> states;
            states[x0] = s0;
            states[x1] = s1;

            BOOST_CHECK_EQUAL( calcLinearState(X,states), s );
            BOOST_CHECK_EQUAL( calcState(X, s), states );
            s++;
        }

    for( s = 0; s < X.nrStates(); s++ )
        BOOST_CHECK_EQUAL( calcLinearState(X, calcState(X,s)), s );
}
