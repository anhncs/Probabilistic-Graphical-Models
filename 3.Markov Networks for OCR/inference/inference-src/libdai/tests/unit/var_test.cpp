/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/var.h>
#include <strstream>


using namespace dai;


#define BOOST_TEST_MODULE VarTest


#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( ConstructorsTest ) {
    // check constructors
    Var x;
    BOOST_CHECK_EQUAL( x.label(), 0 );
    BOOST_CHECK_EQUAL( x.states(), 0 );

    x = Var( 0, 2 );
    BOOST_CHECK_EQUAL( x.label(), 0 );
    BOOST_CHECK_EQUAL( x.states(), 2 );
}

BOOST_AUTO_TEST_CASE( AccMutTest ) {
    // check states and labels mutators
    Var x;
    x.states() = 3;
    BOOST_CHECK_EQUAL( x.states(), 3 );

    x.label() = 5;
    BOOST_CHECK_EQUAL( x.label(), 5 );
}

BOOST_AUTO_TEST_CASE( ComparisonTest ) {
    // check comparison operators
    Var x( 5, 3 );
    Var y( 6, 3 );
    Var z( 5, 3 );
    BOOST_CHECK( x < y );
    BOOST_CHECK( !(x < z) );
    BOOST_CHECK( y > x );
    BOOST_CHECK( !(z > x) );
    BOOST_CHECK( x <= y );
    BOOST_CHECK( x <= z );
    BOOST_CHECK( !(x >= y) );
    BOOST_CHECK( x >= z );
    BOOST_CHECK( !(x == y) );
    BOOST_CHECK( x == z );
    BOOST_CHECK( x != y );
    BOOST_CHECK( !(x != z) );
}

BOOST_AUTO_TEST_CASE( StreamTest ) {
    // check stream output
    Var x( 5, 3 );
    std::stringstream ss;
    ss << x;
    std::string s;
    ss >> s;
    BOOST_CHECK_EQUAL( s, "x5" );
}
