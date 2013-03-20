/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/smallset.h>
#include <vector>


using namespace dai;


#define BOOST_TEST_MODULE SmallSetTest


#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( ConstructorsTest ) {
    // check constructors
    SmallSet<int> x;
    BOOST_CHECK( x.empty() );
    BOOST_CHECK_EQUAL( x.size(), 0 );
    BOOST_CHECK( !x.contains( 5 ) );

    SmallSet<int> y( 1 );
    BOOST_CHECK( !y.empty() );
    BOOST_CHECK_EQUAL( y.size(), 1 );
    BOOST_CHECK( !y.contains( 5 ) );
    BOOST_CHECK( y.contains( 1 ) );

    SmallSet<size_t> u( 0, 1 );
    BOOST_CHECK( !u.empty() );
    BOOST_CHECK_EQUAL( u.size(), 2 );
    BOOST_CHECK( !u.contains( 2 ) );
    BOOST_CHECK( u.contains( 1 ) );
    BOOST_CHECK( u.contains( 0 ) );

    SmallSet<double> z( 1.0, 2.0 );
    BOOST_CHECK_EQUAL( z.size(), 2 );
    BOOST_CHECK( !z.contains( 5.0 ) );
    BOOST_CHECK( z.contains( 1.0 ) );
    BOOST_CHECK( z.contains( 2.0 ) );

    SmallSet<char> v( 'a', 'a' );
    BOOST_CHECK_EQUAL( v.size(), 1 );
    BOOST_CHECK( v.contains( 'a' ) );
    BOOST_CHECK( !v.contains( 'b' ) );

    std::vector<int> a;
    a.push_back( 1 );
    a.push_back( 3 );
    a.push_back( 4 );
    a.push_back( 3 );
    a.push_back( 2 );
    SmallSet<double> w( a.begin(), a.end(), a.size() );
    BOOST_CHECK_EQUAL( w.size(), 4 );
    BOOST_CHECK( w.contains( 1 ) );
    BOOST_CHECK( w.contains( 2 ) );
    BOOST_CHECK( w.contains( 3 ) );
    BOOST_CHECK( w.contains( 4 ) );
    BOOST_CHECK( !w.contains( 0 ) );

    x = y;
    BOOST_CHECK( x == y );
}


BOOST_AUTO_TEST_CASE( ComparisonTest ) {
    // check comparison operators
    SmallSet<int> x, y;
    BOOST_CHECK( x == y );
    BOOST_CHECK( !(x != y) );

    SmallSet<int> z( 1 );
    BOOST_CHECK( x != z );
    BOOST_CHECK( !(x == z) );

    BOOST_CHECK( x < z );

    SmallSet<int> u( 2 );
    BOOST_CHECK( x < u );
    BOOST_CHECK( z < u );
    BOOST_CHECK( !(u < z) );

    SmallSet<int> v( 2, 1 );
    BOOST_CHECK( x < v );
    BOOST_CHECK( z < v );
    BOOST_CHECK( !(v < z) );
    BOOST_CHECK( v < u );
    BOOST_CHECK( !(u < v) );

    SmallSet<int> w( 1, 3 );
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
    std::vector<int> v;
    v.push_back( 3 );
    v.push_back( 2 );
    v.push_back( 2 );
    v.push_back( 1 );
    v.push_back( 3 );
    SmallSet<int> x( v.begin(), v.end(), v.size() );

    BOOST_CHECK( x.begin() != x.end() );
    BOOST_CHECK( x.rbegin() != x.rend() );

    BOOST_CHECK_EQUAL( x.front(), 1 );
    BOOST_CHECK_EQUAL( x.back(), 3 );

    SmallSet<int>::const_iterator j = x.begin();
    SmallSet<int>::iterator k = x.begin();
    for( size_t i = 1; i < 4; i++, j++, k++ ) {
        BOOST_CHECK_EQUAL( i, *j );
        BOOST_CHECK_EQUAL( i, *k );
    }
    BOOST_CHECK( j == x.end() );
    BOOST_CHECK( k == x.end() );

    SmallSet<int>::const_reverse_iterator rj = x.rbegin();
    SmallSet<int>::reverse_iterator rk = x.rbegin();
    for( size_t i = 3; i > 0; i--, rj++, rk++ ) {
        BOOST_CHECK_EQUAL( i, *rj );
        BOOST_CHECK_EQUAL( i, *rk );
    }

    BOOST_CHECK( rj == static_cast<SmallSet<int>::const_reverse_iterator>(x.rend()) );
    BOOST_CHECK( rk == x.rend() );
}


BOOST_AUTO_TEST_CASE( QueriesTest ) {
    // check queries
    SmallSet<int> x( 2, 1 );
    SmallSet<int> y( 2, 3 );
    SmallSet<int> z( 4, 3 );

    BOOST_CHECK( x.intersects( y ) );
    BOOST_CHECK( !x.intersects( z ) );
    BOOST_CHECK( y.intersects( x ) );
    BOOST_CHECK( y.intersects( z ) );
    BOOST_CHECK( !z.intersects( x ) );
    BOOST_CHECK( z.intersects( y ) );

    BOOST_CHECK( x.contains( 1 ) );
    BOOST_CHECK( x.contains( 2 ) );
    BOOST_CHECK( !x.contains( 3 ) );
    BOOST_CHECK( !x.contains( 4 ) );
    BOOST_CHECK( !y.contains( 1 ) );
    BOOST_CHECK( y.contains( 2 ) );
    BOOST_CHECK( y.contains( 3 ) );
    BOOST_CHECK( !y.contains( 4 ) );
    BOOST_CHECK( !z.contains( 1 ) );
    BOOST_CHECK( !z.contains( 2 ) );
    BOOST_CHECK( z.contains( 3 ) );
    BOOST_CHECK( z.contains( 4 ) );

    BOOST_CHECK_EQUAL( x.size(), 2 );

    BOOST_CHECK( !x.empty() );
    
    x.elements()[0] = 3;
    x.elements()[1] = 4;
    std::vector<int> v;
    v.push_back( 3 );
    v.push_back( 4 );
    BOOST_CHECK( x.elements() == v );
}


BOOST_AUTO_TEST_CASE( OperatorTest ) {
    // check operators
    SmallSet<int> x;
    SmallSet<int> x1( 1 );
    SmallSet<int> x2( 2 );
    SmallSet<int> x3( 3 );
    SmallSet<int> x12( 2, 1 );
    SmallSet<int> x23( 2, 3 );
    SmallSet<int> x13( 1, 3 );
    SmallSet<int> x123 = x12 | 3;
    SmallSet<int> y;

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

    // check insert
    y = x   ; y.insert( 1 );   BOOST_CHECK_EQUAL( y, x1   );
    y = x   ; y.insert( 2 );   BOOST_CHECK_EQUAL( y, x2   );
    y = x   ; y.insert( 3 );   BOOST_CHECK_EQUAL( y, x3   );
    y = x1  ; y.insert( 1 );   BOOST_CHECK_EQUAL( y, x1   );
    y = x1  ; y.insert( 2 );   BOOST_CHECK_EQUAL( y, x12  );
    y = x1  ; y.insert( 3 );   BOOST_CHECK_EQUAL( y, x13  );
    y = x2  ; y.insert( 1 );   BOOST_CHECK_EQUAL( y, x12  );
    y = x2  ; y.insert( 2 );   BOOST_CHECK_EQUAL( y, x2   );
    y = x2  ; y.insert( 3 );   BOOST_CHECK_EQUAL( y, x23  );
    y = x3  ; y.insert( 1 );   BOOST_CHECK_EQUAL( y, x13  );
    y = x3  ; y.insert( 2 );   BOOST_CHECK_EQUAL( y, x23  );
    y = x3  ; y.insert( 3 );   BOOST_CHECK_EQUAL( y, x3   );
    y = x12 ; y.insert( 1 );   BOOST_CHECK_EQUAL( y, x12  );
    y = x12 ; y.insert( 2 );   BOOST_CHECK_EQUAL( y, x12  );
    y = x12 ; y.insert( 3 );   BOOST_CHECK_EQUAL( y, x123 );
    y = x23 ; y.insert( 1 );   BOOST_CHECK_EQUAL( y, x123 );
    y = x23 ; y.insert( 2 );   BOOST_CHECK_EQUAL( y, x23  );
    y = x23 ; y.insert( 3 );   BOOST_CHECK_EQUAL( y, x23  );
    y = x13 ; y.insert( 1 );   BOOST_CHECK_EQUAL( y, x13  );
    y = x13 ; y.insert( 2 );   BOOST_CHECK_EQUAL( y, x123 );
    y = x13 ; y.insert( 3 );   BOOST_CHECK_EQUAL( y, x13  );
    y = x123; y.insert( 1 );   BOOST_CHECK_EQUAL( y, x123 );
    y = x123; y.insert( 2 );   BOOST_CHECK_EQUAL( y, x123 );
    y = x123; y.insert( 3 );   BOOST_CHECK_EQUAL( y, x123 );

    // check erase
    y = x   ; y.erase( 1 );    BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y.erase( 2 );    BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y.erase( 3 );    BOOST_CHECK_EQUAL( y, x    );
    y = x1  ; y.erase( 1 );    BOOST_CHECK_EQUAL( y, x    );
    y = x1  ; y.erase( 2 );    BOOST_CHECK_EQUAL( y, x1   );
    y = x1  ; y.erase( 3 );    BOOST_CHECK_EQUAL( y, x1   );
    y = x2  ; y.erase( 1 );    BOOST_CHECK_EQUAL( y, x2   );
    y = x2  ; y.erase( 2 );    BOOST_CHECK_EQUAL( y, x    );
    y = x2  ; y.erase( 3 );    BOOST_CHECK_EQUAL( y, x2   );
    y = x3  ; y.erase( 1 );    BOOST_CHECK_EQUAL( y, x3   );
    y = x3  ; y.erase( 2 );    BOOST_CHECK_EQUAL( y, x3   );
    y = x3  ; y.erase( 3 );    BOOST_CHECK_EQUAL( y, x    );
    y = x12 ; y.erase( 1 );    BOOST_CHECK_EQUAL( y, x2   );
    y = x12 ; y.erase( 2 );    BOOST_CHECK_EQUAL( y, x1   );
    y = x12 ; y.erase( 3 );    BOOST_CHECK_EQUAL( y, x12  );
    y = x13 ; y.erase( 1 );    BOOST_CHECK_EQUAL( y, x3   );
    y = x13 ; y.erase( 2 );    BOOST_CHECK_EQUAL( y, x13  );
    y = x13 ; y.erase( 3 );    BOOST_CHECK_EQUAL( y, x1   );
    y = x23 ; y.erase( 1 );    BOOST_CHECK_EQUAL( y, x23  );
    y = x23 ; y.erase( 2 );    BOOST_CHECK_EQUAL( y, x3   );
    y = x23 ; y.erase( 3 );    BOOST_CHECK_EQUAL( y, x2   );
    y = x123; y.erase( 1 );    BOOST_CHECK_EQUAL( y, x23  );
    y = x123; y.erase( 2 );    BOOST_CHECK_EQUAL( y, x13  );
    y = x123; y.erase( 3 );    BOOST_CHECK_EQUAL( y, x12  );

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
    y = x   ; y /= 1;    BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y /= 2;    BOOST_CHECK_EQUAL( y, x    );
    y = x   ; y /= 3;    BOOST_CHECK_EQUAL( y, x    );
    y = x1  ; y /= 1;    BOOST_CHECK_EQUAL( y, x    );
    y = x1  ; y /= 2;    BOOST_CHECK_EQUAL( y, x1   );
    y = x1  ; y /= 3;    BOOST_CHECK_EQUAL( y, x1   );
    y = x2  ; y /= 1;    BOOST_CHECK_EQUAL( y, x2   );
    y = x2  ; y /= 2;    BOOST_CHECK_EQUAL( y, x    );
    y = x2  ; y /= 3;    BOOST_CHECK_EQUAL( y, x2   );
    y = x3  ; y /= 1;    BOOST_CHECK_EQUAL( y, x3   );
    y = x3  ; y /= 2;    BOOST_CHECK_EQUAL( y, x3   );
    y = x3  ; y /= 3;    BOOST_CHECK_EQUAL( y, x    );
    y = x12 ; y /= 1;    BOOST_CHECK_EQUAL( y, x2   );
    y = x12 ; y /= 2;    BOOST_CHECK_EQUAL( y, x1   );
    y = x12 ; y /= 3;    BOOST_CHECK_EQUAL( y, x12  );
    y = x13 ; y /= 1;    BOOST_CHECK_EQUAL( y, x3   );
    y = x13 ; y /= 2;    BOOST_CHECK_EQUAL( y, x13  );
    y = x13 ; y /= 3;    BOOST_CHECK_EQUAL( y, x1   );
    y = x23 ; y /= 1;    BOOST_CHECK_EQUAL( y, x23  );
    y = x23 ; y /= 2;    BOOST_CHECK_EQUAL( y, x3   );
    y = x23 ; y /= 3;    BOOST_CHECK_EQUAL( y, x2   );
    y = x123; y /= 1;    BOOST_CHECK_EQUAL( y, x23  );
    y = x123; y /= 2;    BOOST_CHECK_EQUAL( y, x13  );
    y = x123; y /= 3;    BOOST_CHECK_EQUAL( y, x12  );

    // check operator|=
    y = x   ; y |= 1;    BOOST_CHECK_EQUAL( y, x1   );
    y = x   ; y |= 2;    BOOST_CHECK_EQUAL( y, x2   );
    y = x   ; y |= 3;    BOOST_CHECK_EQUAL( y, x3   );
    y = x1  ; y |= 1;    BOOST_CHECK_EQUAL( y, x1   );
    y = x1  ; y |= 2;    BOOST_CHECK_EQUAL( y, x12  );
    y = x1  ; y |= 3;    BOOST_CHECK_EQUAL( y, x13  );
    y = x2  ; y |= 1;    BOOST_CHECK_EQUAL( y, x12  );
    y = x2  ; y |= 2;    BOOST_CHECK_EQUAL( y, x2   );
    y = x2  ; y |= 3;    BOOST_CHECK_EQUAL( y, x23  );
    y = x3  ; y |= 1;    BOOST_CHECK_EQUAL( y, x13  );
    y = x3  ; y |= 2;    BOOST_CHECK_EQUAL( y, x23  );
    y = x3  ; y |= 3;    BOOST_CHECK_EQUAL( y, x3   );
    y = x12 ; y |= 1;    BOOST_CHECK_EQUAL( y, x12  );
    y = x12 ; y |= 2;    BOOST_CHECK_EQUAL( y, x12  );
    y = x12 ; y |= 3;    BOOST_CHECK_EQUAL( y, x123 );
    y = x13 ; y |= 1;    BOOST_CHECK_EQUAL( y, x13  );
    y = x13 ; y |= 2;    BOOST_CHECK_EQUAL( y, x123 );
    y = x13 ; y |= 3;    BOOST_CHECK_EQUAL( y, x13  );
    y = x23 ; y |= 1;    BOOST_CHECK_EQUAL( y, x123 );
    y = x23 ; y |= 2;    BOOST_CHECK_EQUAL( y, x23  );
    y = x23 ; y |= 3;    BOOST_CHECK_EQUAL( y, x23  );
    y = x123; y |= 1;    BOOST_CHECK_EQUAL( y, x123 );
    y = x123; y |= 2;    BOOST_CHECK_EQUAL( y, x123 );
    y = x123; y |= 3;    BOOST_CHECK_EQUAL( y, x123 );

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
