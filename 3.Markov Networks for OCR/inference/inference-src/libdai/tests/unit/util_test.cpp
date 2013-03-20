/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/util.h>
#include <strstream>
#include <string>
#include <vector>
#include <map>
#include <set>


using namespace dai;


#define BOOST_TEST_MODULE UtilTest


#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( IsNanTest ) {
    double x = 0.0, y = 0.0;
    BOOST_CHECK( dai::isnan( x / y ) );
    BOOST_CHECK( !dai::isnan( x ) );
}


BOOST_AUTO_TEST_CASE( RndTest ) {
    rnd_seed( 123 );
    Real a1 = rnd_uniform();
    Real b1 = rnd_stdnormal();
    int c1 = rnd_int(0,1);
    int d1 = rnd(2);

    rnd_seed( 123 );
    Real a2 = rnd_uniform();
    Real b2 = rnd_stdnormal();
    int c2 = rnd_int(0,1);
    int d2 = rnd(2);

    BOOST_CHECK_EQUAL( a1, a2 );
    BOOST_CHECK_EQUAL( b1, b2 );
    BOOST_CHECK_EQUAL( c1, c2 );
    BOOST_CHECK_EQUAL( d1, d2 );

    for( size_t i = 0; i < 10000; i++ ) {
        Real x = rnd_uniform();
        BOOST_CHECK( x >= 0.0 );
        BOOST_CHECK( x < 1 );
    }

    for( int min = -5; min <= 5; min++ )
        for( int max = min; max <= min + 10; max++ )
            for( size_t i = 0; i < 1000; i++ ) {
                int j = rnd_int( min, max );
                BOOST_CHECK( j >= min );
                BOOST_CHECK( j <= max );
            }

    for( int max = 1; max <= 100; max++ )
        for( size_t i = 0; i < 100; i++ ) {
            int j = rnd( max );
            BOOST_CHECK( j >= 0 );
            BOOST_CHECK( j < max );
        }
}


BOOST_AUTO_TEST_CASE( StreamTest ) {
    std::vector<int> v;
    v.push_back( 1 );
    v.push_back( 3 );
    v.push_back( 2 );
    std::stringstream ss;
    ss << v;
    std::string s;
    std::getline( ss, s );
    BOOST_CHECK_EQUAL( s, "(1, 3, 2)" );

    std::set<int> x;
    x.insert( 6 );
    x.insert( 5 );
    x.insert( 4 );
    std::stringstream ss2;
    ss2 << x;
    std::getline( ss2, s );
    BOOST_CHECK_EQUAL( s, "{4, 5, 6}" );

    std::map<int,int> y;
    y[1] = 6;
    y[3] = 5;
    y[2] = 4;
    std::stringstream ss3;
    ss3 << y;
    std::getline( ss3, s );
    BOOST_CHECK_EQUAL( s, "{1->6, 2->4, 3->5}" );

    std::pair<int,double> z;
    z.first = 5;
    z.second = 1.2345;
    std::stringstream ss4;
    ss4 << z;
    std::getline( ss4, s );
    BOOST_CHECK_EQUAL( s, "(5, 1.2345)" );
}


BOOST_AUTO_TEST_CASE( concatTest ) {
    std::vector<int> a;
    a.push_back( 0 );
    a.push_back( 1 );
    std::vector<int> b;
    b.push_back( 2 );
    b.push_back( 3 );
    b.push_back( 4 );
    std::vector<int> c;
    c.push_back( 0 );
    c.push_back( 1 );
    c.push_back( 2 );
    c.push_back( 3 );
    c.push_back( 4 );
    BOOST_CHECK( concat( a, b ) == c );
}


BOOST_AUTO_TEST_CASE( stringTest ) {
    int a = 5;
    BOOST_CHECK_EQUAL( toString( a ), std::string("5") );
    BOOST_CHECK_EQUAL( fromString<int>( "5" ), a );
    
    bool b = true;
    BOOST_CHECK_EQUAL( toString( b ), std::string("1") );
    BOOST_CHECK_EQUAL( fromString<bool>( "1" ), b );

    double c = -3.5;
    BOOST_CHECK_EQUAL( toString( c ), std::string("-3.5") );
    BOOST_CHECK_EQUAL( fromString<double>( "-3.5" ), c );

    unsigned char d = 'e';
    BOOST_CHECK_EQUAL( toString( d ), std::string("e") );
    BOOST_CHECK_EQUAL( fromString<unsigned char>( "e" ), d );
}


BOOST_AUTO_TEST_CASE( tokenizeStringTest ) {
    std::string s;
    std::vector<std::string> tokens;

    s = "";
    tokens = tokenizeString( s, true, " \t" );
    BOOST_CHECK_EQUAL( tokens[0], "" );
    BOOST_CHECK_EQUAL( tokens.size(), 1 );

    s = " ";
    tokens = tokenizeString( s, true, " \t" );
    BOOST_CHECK_EQUAL( tokens.size(), 2 );
    BOOST_CHECK_EQUAL( tokens[0], "" );
    BOOST_CHECK_EQUAL( tokens[1], "" );
    
    s = " \t";
    tokens = tokenizeString( s, true, " \t" );
    BOOST_CHECK_EQUAL( tokens.size(), 3 );
    BOOST_CHECK_EQUAL( tokens[0], "" );
    BOOST_CHECK_EQUAL( tokens[1], "" );
    BOOST_CHECK_EQUAL( tokens[2], "" );

    s = " \tHello";
    tokens = tokenizeString( s, true, " \t" );
    BOOST_CHECK_EQUAL( tokens.size(), 3 );
    BOOST_CHECK_EQUAL( tokens[0], "" );
    BOOST_CHECK_EQUAL( tokens[1], "" );
    BOOST_CHECK_EQUAL( tokens[2], "Hello" );

    s = " \tHello\r\n there";
    tokens = tokenizeString( s, true, " \t" );
    BOOST_CHECK_EQUAL( tokens.size(), 4 );
    BOOST_CHECK_EQUAL( tokens[0], "" );
    BOOST_CHECK_EQUAL( tokens[1], "" );
    BOOST_CHECK_EQUAL( tokens[2], "Hello\r\n" );
    BOOST_CHECK_EQUAL( tokens[3], "there" );

    s = " \tHello\r\n there !";
    tokens = tokenizeString( s, true, " \t" );
    BOOST_CHECK_EQUAL( tokens.size(), 5 );
    BOOST_CHECK_EQUAL( tokens[0], "" );
    BOOST_CHECK_EQUAL( tokens[1], "" );
    BOOST_CHECK_EQUAL( tokens[2], "Hello\r\n" );
    BOOST_CHECK_EQUAL( tokens[3], "there" );
    BOOST_CHECK_EQUAL( tokens[4], "!" );

    s = " \tHello\r\n there !\r";
    tokens = tokenizeString( s, true, " \t" );
    BOOST_CHECK_EQUAL( tokens.size(), 5 );
    BOOST_CHECK_EQUAL( tokens[0], "" );
    BOOST_CHECK_EQUAL( tokens[1], "" );
    BOOST_CHECK_EQUAL( tokens[2], "Hello\r\n" );
    BOOST_CHECK_EQUAL( tokens[3], "there" );
    BOOST_CHECK_EQUAL( tokens[4], "!\r" );

    s = " \tHello\r\n there !\r";
    tokens = tokenizeString( s, true, " \t\r\n" );
    BOOST_CHECK_EQUAL( tokens.size(), 8 );
    BOOST_CHECK_EQUAL( tokens[0], "" );
    BOOST_CHECK_EQUAL( tokens[1], "" );
    BOOST_CHECK_EQUAL( tokens[2], "Hello" );
    BOOST_CHECK_EQUAL( tokens[3], "" );
    BOOST_CHECK_EQUAL( tokens[4], "" );
    BOOST_CHECK_EQUAL( tokens[5], "there" );
    BOOST_CHECK_EQUAL( tokens[6], "!" );
    BOOST_CHECK_EQUAL( tokens[7], "" );


    s = "";
    tokens = tokenizeString( s, false, " \t" );
    BOOST_CHECK_EQUAL( tokens.size(), 0 );

    s = " ";
    tokens = tokenizeString( s, false, " \t" );
    BOOST_CHECK_EQUAL( tokens.size(), 0 );
    
    s = " \t";
    tokens = tokenizeString( s, false, " \t" );
    BOOST_CHECK_EQUAL( tokens.size(), 0 );

    s = " \tHello";
    tokens = tokenizeString( s, false, " \t" );
    BOOST_CHECK_EQUAL( tokens.size(), 1 );
    BOOST_CHECK_EQUAL( tokens[0], "Hello" );

    s = " \tHello\r\n there";
    tokens = tokenizeString( s, false, " \t" );
    BOOST_CHECK_EQUAL( tokens.size(), 2 );
    BOOST_CHECK_EQUAL( tokens[0], "Hello\r\n" );
    BOOST_CHECK_EQUAL( tokens[1], "there" );

    s = " \tHello\r\n there !";
    tokens = tokenizeString( s, false, " \t" );
    BOOST_CHECK_EQUAL( tokens.size(), 3 );
    BOOST_CHECK_EQUAL( tokens[0], "Hello\r\n" );
    BOOST_CHECK_EQUAL( tokens[1], "there" );
    BOOST_CHECK_EQUAL( tokens[2], "!" );

    s = " \tHello\r\n there !\r";
    tokens = tokenizeString( s, false, " \t" );
    BOOST_CHECK_EQUAL( tokens.size(), 3 );
    BOOST_CHECK_EQUAL( tokens[0], "Hello\r\n" );
    BOOST_CHECK_EQUAL( tokens[1], "there" );
    BOOST_CHECK_EQUAL( tokens[2], "!\r" );

    s = " \tHello\r\n there !\r";
    tokens = tokenizeString( s, false, " \t\r\n" );
    BOOST_CHECK_EQUAL( tokens.size(), 3 );
    BOOST_CHECK_EQUAL( tokens[0], "Hello" );
    BOOST_CHECK_EQUAL( tokens[1], "there" );
    BOOST_CHECK_EQUAL( tokens[2], "!" );
}
