/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/enum.h>
#include <strstream>
#include <iostream>
#include <string>


using namespace dai;


#define BOOST_TEST_MODULE EnumTest


#include <boost/test/unit_test.hpp>


DAI_ENUM(colors,RED,GREEN,BLUE);


BOOST_AUTO_TEST_CASE( ConstructorsTest ) {
    colors c;
    BOOST_CHECK( c == colors::RED );
    BOOST_CHECK_EQUAL( static_cast<size_t>(c), 0 );
    BOOST_CHECK_EQUAL( strcmp( static_cast<char const *>(c), "RED" ), 0 );

    colors d(colors::GREEN);
    BOOST_CHECK( d == colors::GREEN );
    BOOST_CHECK_EQUAL( static_cast<size_t>(d), 1 );
    BOOST_CHECK_EQUAL( strcmp( static_cast<char const *>(d), "GREEN" ), 0 );

    colors e("BLUE");
    BOOST_CHECK( e == colors::BLUE );
    BOOST_CHECK_EQUAL( static_cast<size_t>(e), 2 );
    BOOST_CHECK_EQUAL( strcmp( static_cast<char const *>(e), "BLUE" ), 0 );

    BOOST_CHECK_THROW( colors f("BLUEISH"), Exception );

    colors f = e;
    colors g(f);
    BOOST_CHECK( f == colors::BLUE );
    BOOST_CHECK( g == colors::BLUE );
    BOOST_CHECK( static_cast<colors::value>(f) == static_cast<colors::value>(e) );
    BOOST_CHECK( static_cast<colors::value>(e) == static_cast<colors::value>(g) );
    BOOST_CHECK( static_cast<colors::value>(f) == static_cast<colors::value>(g) );
}


BOOST_AUTO_TEST_CASE( StreamTest ) {
    std::stringstream ss1, ss2, ss3, ss4, ss5, ss6, ss7;
    std::string s;

    ss1 << colors(colors::RED);
    ss1 >> s;
    BOOST_CHECK_EQUAL( s, "RED" );

    ss2 << colors(colors::GREEN);
    std::getline( ss2, s );
    BOOST_CHECK_EQUAL( s, "GREEN" );

    ss3 << colors(colors::BLUE);
    ss3 >> s;
    BOOST_CHECK_EQUAL( s, "BLUE" );

    colors c;
    ss4 << colors(colors::RED);
    ss4 >> c;
    BOOST_CHECK_EQUAL( c, colors::RED );
    ss5 << colors(colors::GREEN);
    ss5 >> c;
    BOOST_CHECK_EQUAL( c, colors::GREEN );
    ss6 << colors(colors::BLUE);
    ss6 >> c;
    BOOST_CHECK_EQUAL( c, colors::BLUE );

    ss7 << "BLUEISH";
    BOOST_CHECK_THROW( ss7 >> c, Exception );
}
