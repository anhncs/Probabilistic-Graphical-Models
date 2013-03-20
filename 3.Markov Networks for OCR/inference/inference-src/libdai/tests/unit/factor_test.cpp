/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/factor.h>
#include <strstream>


using namespace dai;


const Real tol = 1e-8;


#define BOOST_TEST_MODULE FactorTest


#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>


BOOST_AUTO_TEST_CASE( ConstructorsTest ) {
    // check constructors
    Factor x1;
    BOOST_CHECK_EQUAL( x1.nrStates(), 1 );
    BOOST_CHECK( x1.p() == Prob( 1, 1.0 ) );
    BOOST_CHECK( x1.vars() == VarSet() );

    Factor x2( 5.0 );
    BOOST_CHECK_EQUAL( x2.nrStates(), 1 );
    BOOST_CHECK( x2.p() == Prob( 1, 5.0 ) );
    BOOST_CHECK( x2.vars() == VarSet() );

    Var v1( 0, 3 );
    Factor x3( v1 );
    BOOST_CHECK_EQUAL( x3.nrStates(), 3 );
    BOOST_CHECK_SMALL( dist( x3.p(), Prob( 3, 1.0 / 3.0 ), DISTL1 ), tol );
    BOOST_CHECK( x3.vars() == VarSet( v1 ) );
    BOOST_CHECK_CLOSE( x3[0], (Real)(1.0 / 3.0), tol );
    BOOST_CHECK_CLOSE( x3[1], (Real)(1.0 / 3.0), tol );
    BOOST_CHECK_CLOSE( x3[2], (Real)(1.0 / 3.0), tol );

    Var v2( 1, 2 );
    Factor x4( VarSet( v1, v2 ) );
    BOOST_CHECK_EQUAL( x4.nrStates(), 6 );
    BOOST_CHECK_SMALL( dist( x4.p(), Prob( 6, 1.0 / 6.0 ), DISTL1 ), tol );
    BOOST_CHECK( x4.vars() == VarSet( v1, v2 ) );
    for( size_t i = 0; i < 6; i++ )
        BOOST_CHECK_CLOSE( x4[i], (Real)(1.0 / 6.0), tol );

    Factor x5( VarSet( v1, v2 ), 1.0 );
    BOOST_CHECK_EQUAL( x5.nrStates(), 6 );
    BOOST_CHECK( x5.p() == Prob( 6, 1.0 ) );
    BOOST_CHECK( x5.vars() == VarSet( v1, v2 ) );
    for( size_t i = 0; i < 6; i++ )
        BOOST_CHECK_EQUAL( x5[i], (Real)1.0 );

    std::vector<Real> x( 6, 1.0 );
    for( size_t i = 0; i < 6; i++ )
        x[i] = 10.0 - i;
    Factor x6( VarSet( v1, v2 ), x );
    BOOST_CHECK_EQUAL( x6.nrStates(), 6 );
    BOOST_CHECK( x6.vars() == VarSet( v1, v2 ) );
    for( size_t i = 0; i < 6; i++ )
        BOOST_CHECK_EQUAL( x6[i], x[i] );

    x.resize( 4 );
    BOOST_CHECK_THROW( Factor x7( VarSet( v1, v2 ), x ), Exception );

    x.resize( 6 );
    x[4] = 10.0 - 4; x[5] = 10.0 - 5;
    Factor x8( VarSet( v2, v1 ), &(x[0]) );
    BOOST_CHECK_EQUAL( x8.nrStates(), 6 );
    BOOST_CHECK( x8.vars() == VarSet( v1, v2 ) );
    for( size_t i = 0; i < 6; i++ )
        BOOST_CHECK_EQUAL( x8[i], x[i] );

    Prob xx( x );
    Factor x9( VarSet( v2, v1 ), xx );
    BOOST_CHECK_EQUAL( x9.nrStates(), 6 );
    BOOST_CHECK( x9.vars() == VarSet( v1, v2 ) );
    for( size_t i = 0; i < 6; i++ )
        BOOST_CHECK_EQUAL( x9[i], x[i] );

    xx.resize( 4 );
    BOOST_CHECK_THROW( Factor x10( VarSet( v2, v1 ), xx ), Exception );

    std::vector<Real> w;
    w.push_back( 0.1 );
    w.push_back( 3.5 );
    w.push_back( 2.8 );
    w.push_back( 6.3 );
    w.push_back( 8.4 );
    w.push_back( 0.0 );
    w.push_back( 7.4 );
    w.push_back( 2.4 );
    w.push_back( 8.9 );
    w.push_back( 1.3 );
    w.push_back( 1.6 );
    w.push_back( 2.6 );
    Var v4( 4, 3 );
    Var v8( 8, 2 );
    Var v7( 7, 2 );
    std::vector<Var> vars;
    vars.push_back( v4 );
    vars.push_back( v8 );
    vars.push_back( v7 );
    Factor x11( vars, w );
    BOOST_CHECK_EQUAL( x11.nrStates(), 12 );
    BOOST_CHECK( x11.vars() == VarSet( vars.begin(), vars.end() ) );
    BOOST_CHECK_EQUAL( x11[0], (Real)0.1 );
    BOOST_CHECK_EQUAL( x11[1], (Real)3.5 );
    BOOST_CHECK_EQUAL( x11[2], (Real)2.8 );
    BOOST_CHECK_EQUAL( x11[3], (Real)7.4 );
    BOOST_CHECK_EQUAL( x11[4], (Real)2.4 );
    BOOST_CHECK_EQUAL( x11[5], (Real)8.9 );
    BOOST_CHECK_EQUAL( x11[6], (Real)6.3 );
    BOOST_CHECK_EQUAL( x11[7], (Real)8.4 );
    BOOST_CHECK_EQUAL( x11[8], (Real)0.0 );
    BOOST_CHECK_EQUAL( x11[9], (Real)1.3 );
    BOOST_CHECK_EQUAL( x11[10], (Real)1.6 );
    BOOST_CHECK_EQUAL( x11[11], (Real)2.6 );

    Factor x12( x11 );
    BOOST_CHECK( x12 == x11 );
    
    Factor x13 = x12;
    BOOST_CHECK( x13 == x11 );
}


BOOST_AUTO_TEST_CASE( QueriesTest ) {
    Factor x( Var( 5, 5 ), 0.0 );
    for( size_t i = 0; i < x.nrStates(); i++ )
        x.set( i, 2.0 - i );

    // test min, max, sum, sumAbs, maxAbs
    BOOST_CHECK_CLOSE( x.sum(), (Real)0.0, tol );
    BOOST_CHECK_CLOSE( x.max(), (Real)2.0, tol );
    BOOST_CHECK_CLOSE( x.min(), (Real)-2.0, tol );
    BOOST_CHECK_CLOSE( x.sumAbs(), (Real)6.0, tol );
    BOOST_CHECK_CLOSE( x.maxAbs(), (Real)2.0, tol );
    x.set( 1, 1.0 );
    BOOST_CHECK_CLOSE( x.maxAbs(), (Real)2.0, tol );
    x /= x.sum();

    // test entropy
    BOOST_CHECK( x.entropy() < Prob(5).entropy() );
    for( size_t i = 1; i < 100; i++ )
        BOOST_CHECK_CLOSE( Factor( Var(0,i) ).entropy(), dai::log((Real)i), tol );

    // test hasNaNs and hasNegatives
    BOOST_CHECK( !Factor( 0.0 ).hasNaNs() );
    Real c = 0.0;
    BOOST_CHECK( Factor( c / c ).hasNaNs() );
    BOOST_CHECK( !Factor( 0.0 ).hasNegatives() );
    BOOST_CHECK( !Factor( 1.0 ).hasNegatives() );
    BOOST_CHECK( Factor( -1.0 ).hasNegatives() );
    x.set( 0, 0.0 ); x.set( 1, 0.0 ); x.set( 2, -1.0 ); x.set( 3, 1.0 ); x.set( 4, 100.0 );
    BOOST_CHECK( x.hasNegatives() );
    x.set( 2, -INFINITY );
    BOOST_CHECK( x.hasNegatives() );
    x.set( 2, INFINITY );
    BOOST_CHECK( !x.hasNegatives() );
    x.set( 2, -1.0 );

    // test strength
    Var x0(0,2);
    Var x1(1,2);
    BOOST_CHECK_CLOSE( createFactorIsing( x0, x1, 1.0 ).strength( x0, x1 ), std::tanh( (Real)1.0 ), tol );
    BOOST_CHECK_CLOSE( createFactorIsing( x0, x1, -1.0 ).strength( x0, x1 ), std::tanh( (Real)1.0 ), tol );
    BOOST_CHECK_CLOSE( createFactorIsing( x0, x1, 0.5 ).strength( x0, x1 ), std::tanh( (Real)0.5 ), tol );

    // test ==
    Factor a(Var(0,3)), b(Var(0,3));
    Factor d(Var(1,3));
    BOOST_CHECK( !(a == d) );
    BOOST_CHECK( !(b == d) );
    BOOST_CHECK( a == b );
    a.set( 0, 0.0 );
    BOOST_CHECK( !(a == b) );
    b.set( 2, 0.0 );
    BOOST_CHECK( !(a == b) );
    b.set( 0, 0.0 );
    BOOST_CHECK( !(a == b) );
    a.set( 1, 0.0 );
    BOOST_CHECK( !(a == b) );
    b.set( 1, 0.0 );
    BOOST_CHECK( !(a == b) );
    a.set( 2, 0.0 );
    BOOST_CHECK( a == b );
}


BOOST_AUTO_TEST_CASE( UnaryTransformationsTest ) {
    Var v( 0, 3 );
    Factor x( v );
    x.set( 0, -2.0 );
    x.set( 1, 0.0 );
    x.set( 2, 2.0 );

    Factor y = -x;
    BOOST_CHECK_CLOSE( y[0], (Real)2.0, tol );
    BOOST_CHECK_CLOSE( y[1], (Real)0.0, tol );
    BOOST_CHECK_CLOSE( y[2], (Real)-2.0, tol );

    y = x.abs();
    BOOST_CHECK_CLOSE( y[0], (Real)2.0, tol );
    BOOST_CHECK_CLOSE( y[1], (Real)0.0, tol );
    BOOST_CHECK_CLOSE( y[2], (Real)2.0, tol );

    y = x.exp();
    BOOST_CHECK_CLOSE( y[0], dai::exp((Real)-2.0), tol );
    BOOST_CHECK_CLOSE( y[1], (Real)1.0, tol );
    BOOST_CHECK_CLOSE( y[2], (Real)1.0 / y[0], tol );

    y = x.log(false);
    BOOST_CHECK( dai::isnan( y[0] ) );
    BOOST_CHECK_EQUAL( y[1], -INFINITY );
    BOOST_CHECK_CLOSE( y[2], dai::log((Real)2.0), tol );

    y = x.log(true);
    BOOST_CHECK( dai::isnan( y[0] ) );
    BOOST_CHECK_CLOSE( y[1], (Real)0.0, tol );
    BOOST_CHECK_CLOSE( y[2], dai::log((Real)2.0), tol );

    y = x.inverse(false);
    BOOST_CHECK_CLOSE( y[0], (Real)-0.5, tol );
    BOOST_CHECK_EQUAL( y[1], INFINITY );
    BOOST_CHECK_CLOSE( y[2], (Real)0.5, tol );

    y = x.inverse(true);
    BOOST_CHECK_CLOSE( y[0], (Real)-0.5, tol );
    BOOST_CHECK_CLOSE( y[1], (Real)0.0, tol );
    BOOST_CHECK_CLOSE( y[2], (Real)0.5, tol );

    x.set( 0, 2.0 );
    y = x.normalized();
    BOOST_CHECK_CLOSE( y[0], (Real)0.5, tol );
    BOOST_CHECK_CLOSE( y[1], (Real)0.0, tol );
    BOOST_CHECK_CLOSE( y[2], (Real)0.5, tol );

    y = x.normalized( NORMPROB );
    BOOST_CHECK_CLOSE( y[0], (Real)0.5, tol );
    BOOST_CHECK_CLOSE( y[1], (Real)0.0, tol );
    BOOST_CHECK_CLOSE( y[2], (Real)0.5, tol );

    x.set( 0, -2.0 );
    y = x.normalized( NORMLINF );
    BOOST_CHECK_CLOSE( y[0], (Real)-1.0, tol );
    BOOST_CHECK_CLOSE( y[1], (Real)0.0, tol );
    BOOST_CHECK_CLOSE( y[2], (Real)1.0, tol );
}


BOOST_AUTO_TEST_CASE( UnaryOperationsTest ) {
    Var v( 0, 3 );
    Factor xorg( v );
    xorg.set( 0, 2.0 );
    xorg.set( 1, 0.0 );
    xorg.set( 2, 1.0 );
    Factor y( v );

    Factor x = xorg;
    BOOST_CHECK( x.setUniform() == Factor( v ) );
    BOOST_CHECK( x == Factor( v ) );

    y.set( 0, dai::exp(2.0) );
    y.set( 1, 1.0 );
    y.set( 2, dai::exp(1.0) );
    x = xorg;
    BOOST_CHECK_SMALL( dist( x.takeExp(), y, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, y, DISTL1 ), tol );

    y.set( 0, dai::log(2.0) );
    y.set( 1, -INFINITY );
    y.set( 2, 0.0 );
    x = xorg;
    Factor z = x.takeLog();
    BOOST_CHECK_CLOSE( z[0], y[0], tol );
    //BOOST_CHECK_CLOSE( z[1], y[1], tol );
    BOOST_CHECK_CLOSE( z[2], y[2], tol );
    BOOST_CHECK( z.vars() == y.vars() );
    BOOST_CHECK( x == z );
    x = xorg;
    z = x.takeLog(false);
    BOOST_CHECK_CLOSE( z[0], y[0], tol );
    //BOOST_CHECK_CLOSE( z[1], y[1], tol );
    BOOST_CHECK_CLOSE( z[2], y[2], tol );
    BOOST_CHECK( z.vars() == y.vars() );
    BOOST_CHECK( x == z );

    y.set( 1, 0.0 );
    x = xorg;
    BOOST_CHECK_SMALL( dist( x.takeLog(true), y, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, y, DISTL1 ), tol );

    y.set( 0, 2.0 / 3.0 );
    y.set( 1, 0.0 / 3.0 );
    y.set( 2, 1.0 / 3.0 );
    x = xorg;
    BOOST_CHECK_CLOSE( x.normalize(), (Real)3.0, tol );
    BOOST_CHECK_SMALL( dist( x, y, DISTL1 ), tol );

    x = xorg;
    BOOST_CHECK_CLOSE( x.normalize( NORMPROB ), (Real)3.0, tol );
    BOOST_CHECK_SMALL( dist( x, y, DISTL1 ), tol );

    y.set( 0, 2.0 / 2.0 );
    y.set( 1, 0.0 / 2.0 );
    y.set( 2, 1.0 / 2.0 );
    x = xorg;
    BOOST_CHECK_CLOSE( x.normalize( NORMLINF ), (Real)2.0, tol );
    BOOST_CHECK_SMALL( dist( x, y, DISTL1 ), tol );

    xorg.set( 0, -2.0 );
    y.set( 0, 2.0 );
    y.set( 1, 0.0 );
    y.set( 2, 1.0 );
    x = xorg;
    BOOST_CHECK_SMALL( dist( x.takeAbs(), y, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, y, DISTL1 ), tol );

    for( size_t repeat = 0; repeat < 10000; repeat++ ) {
        x.randomize();
        for( size_t i = 0; i < x.nrStates(); i++ ) {
            BOOST_CHECK( x[i] < (Real)1.0 );
            BOOST_CHECK( x[i] >= (Real)0.0 );
        }
    }
}


BOOST_AUTO_TEST_CASE( ScalarOperationsTest ) {
    Var v( 0, 3 );
    Factor xorg( v ), x( v );
    xorg.set( 0, 2.0 );
    xorg.set( 1, 0.0 );
    xorg.set( 2, 1.0 );
    Factor y( v );

    x = xorg;
    BOOST_CHECK( x.fill( 1.0 ) == Factor(v, 1.0) );
    BOOST_CHECK( x == Factor(v, 1.0) );
    BOOST_CHECK( x.fill( 2.0 ) == Factor(v, 2.0) );
    BOOST_CHECK( x == Factor(v, 2.0) );
    BOOST_CHECK( x.fill( 0.0 ) == Factor(v, 0.0) );
    BOOST_CHECK( x == Factor(v, 0.0) );

    x = xorg;
    y.set( 0, 3.0 ); y.set( 1, 1.0 ); y.set( 2, 2.0 );
    BOOST_CHECK_SMALL( dist( (x += 1.0), y, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, y, DISTL1 ), tol );
    y.set( 0, 1.0 ); y.set( 1, -1.0 ); y.set( 2, 0.0 );
    BOOST_CHECK_SMALL( dist( (x += -2.0), y, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, y, DISTL1 ), tol );

    x = xorg;
    y.set( 0, 1.0 ); y.set( 1, -1.0 ); y.set( 2, 0.0 );
    BOOST_CHECK_SMALL( dist( (x -= 1.0), y, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, y, DISTL1 ), tol );
    y.set( 0, 3.0 ); y.set( 1, 1.0 ); y.set( 2, 2.0 );
    BOOST_CHECK_SMALL( dist( (x -= -2.0), y, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, y, DISTL1 ), tol );

    x = xorg;
    BOOST_CHECK_SMALL( dist( (x *= 1.0), x, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, x, DISTL1 ), tol );
    y.set( 0, 4.0 ); y.set( 1, 0.0 ); y.set( 2, 2.0 );
    BOOST_CHECK_SMALL( dist( (x *= 2.0), y, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, y, DISTL1 ), tol );
    y.set( 0, -1.0 ); y.set( 1, 0.0 ); y.set( 2, -0.5 );
    BOOST_CHECK_SMALL( dist( (x *= -0.25), y, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, y, DISTL1 ), tol );

    x = xorg;
    BOOST_CHECK_SMALL( dist( (x /= 1.0), x, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, x, DISTL1 ), tol );
    y.set( 0, 1.0 ); y.set( 1, 0.0 ); y.set( 2, 0.5 );
    BOOST_CHECK_SMALL( dist( (x /= 2.0), y, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, y, DISTL1 ), tol );
    y.set( 0, -4.0 ); y.set( 1, 0.0 ); y.set( 2, -2.0 );
    BOOST_CHECK_SMALL( dist( (x /= -0.25), y, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, y, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( (x /= 0.0), Factor(v, 0.0), DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, Factor(v, 0.0), DISTL1 ), tol );

    x = xorg;
    BOOST_CHECK_SMALL( dist( (x ^= 1.0), x, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, x, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( (x ^= 0.0), Factor(v, 1.0), DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, Factor(v, 1.0), DISTL1 ), tol );
    x = xorg;
    y.set( 0, 4.0 ); y.set( 1, 0.0 ); y.set( 2, 1.0 );
    BOOST_CHECK_SMALL( dist( (x ^= 2.0), y, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( x, y, DISTL1 ), tol );
    y.set( 0, 2.0 ); y.set( 1, 0.0 ); y.set( 2, 1.0 );
    BOOST_CHECK( (x ^= 0.5) == y );
    BOOST_CHECK( x == y );
}


BOOST_AUTO_TEST_CASE( ScalarTransformationsTest ) {
    Var v( 0, 3 );
    Factor x( v );
    x.set( 0, 2.0 );
    x.set( 1, 0.0 );
    x.set( 2, 1.0 );
    Factor y( v );

    y.set( 0, 3.0 ); y.set( 1, 1.0 ); y.set( 2, 2.0 );
    BOOST_CHECK_SMALL( dist( (x + 1.0), y, DISTL1 ), tol );
    y.set( 0, 0.0 ); y.set( 1, -2.0 ); y.set( 2, -1.0 );
    BOOST_CHECK_SMALL( dist( (x + (-2.0)), y, DISTL1 ), tol );

    y.set( 0, 1.0 ); y.set( 1, -1.0 ); y.set( 2, 0.0 );
    BOOST_CHECK_SMALL( dist( (x - 1.0), y, DISTL1 ), tol );
    y.set( 0, 4.0 ); y.set( 1, 2.0 ); y.set( 2, 3.0 );
    BOOST_CHECK_SMALL( dist( (x - (-2.0)), y, DISTL1 ), tol );

    BOOST_CHECK_SMALL( dist( (x * 1.0), x, DISTL1 ), tol );
    y.set( 0, 4.0 ); y.set( 1, 0.0 ); y.set( 2, 2.0 );
    BOOST_CHECK_SMALL( dist( (x * 2.0), y, DISTL1 ), tol );
    y.set( 0, -1.0 ); y.set( 1, 0.0 ); y.set( 2, -0.5 );
    BOOST_CHECK_SMALL( dist( (x * -0.5), y, DISTL1 ), tol );

    BOOST_CHECK_SMALL( dist( (x / 1.0), x, DISTL1 ), tol );
    y.set( 0, 1.0 ); y.set( 1, 0.0 ); y.set( 2, 0.5 );
    BOOST_CHECK_SMALL( dist( (x / 2.0), y, DISTL1 ), tol );
    y.set( 0, -4.0 ); y.set( 1, 0.0 ); y.set( 2, -2.0 );
    BOOST_CHECK_SMALL( dist( (x / -0.5), y, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( (x / 0.0), Factor(v, 0.0), DISTL1 ), tol );

    BOOST_CHECK_SMALL( dist( (x ^ 1.0), x, DISTL1 ), tol );
    BOOST_CHECK_SMALL( dist( (x ^ 0.0), Factor(v, 1.0), DISTL1 ), tol );
    y.set( 0, 4.0 ); y.set( 1, 0.0 ); y.set( 2, 1.0 );
    BOOST_CHECK_SMALL( dist( (x ^ 2.0), y, DISTL1 ), tol );
    y.set( 0, std::sqrt(2.0) ); y.set( 1, 0.0 ); y.set( 2, 1.0 );
    BOOST_CHECK_SMALL( dist( (x ^ 0.5), y, DISTL1 ), tol );
}


BOOST_AUTO_TEST_CASE( SimilarFactorOperationsTest ) {
    size_t N = 6;
    Var v( 0, N );
    Factor xorg( v ), x( v );
    xorg.set( 0, 2.0 ); xorg.set( 1, 0.0 ); xorg.set( 2, 1.0 ); xorg.set( 3, 0.0 ); xorg.set( 4, 2.0 ); xorg.set( 5, 3.0 );
    Factor y( v );
    y.set( 0, 0.5 ); y.set( 1, -1.0 ); y.set( 2, 0.0 ); y.set( 3, 0.0 ); y.set( 4, -2.0 ); y.set( 5, 3.0 );
    Factor z( v ), r( v );

    z.set( 0, 2.5 ); z.set( 1, -1.0 ); z.set( 2, 1.0 ); z.set( 3, 0.0 ); z.set( 4, 0.0 ); z.set( 5, 6.0 );
    x = xorg;
    r = (x += y);
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    BOOST_CHECK( x == r );
    x = xorg;
    BOOST_CHECK( x.binaryOp( y, std::plus<Real>() ) == r );
    BOOST_CHECK( x == r );

    z.set( 0, 1.5 ); z.set( 1, 1.0 ); z.set( 2, 1.0 ); z.set( 3, 0.0 ); z.set( 4, 4.0 ); z.set( 5, 0.0 );
    x = xorg;
    r = (x -= y);
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    BOOST_CHECK( x == r );
    x = xorg;
    BOOST_CHECK( x.binaryOp( y, std::minus<Real>() ) == r );
    BOOST_CHECK( x == r );

    z.set( 0, 1.0 ); z.set( 1, 0.0 ); z.set( 2, 0.0 ); z.set( 3, 0.0 ); z.set( 4, -4.0 ); z.set( 5, 9.0 );
    x = xorg;
    r = (x *= y);
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    BOOST_CHECK( x == r );
    x = xorg;
    BOOST_CHECK( x.binaryOp( y, std::multiplies<Real>() ) == r );
    BOOST_CHECK( x == r );

    z.set( 0, 4.0 ); z.set( 1, 0.0 ); z.set( 2, 0.0 ); z.set( 3, 0.0 ); z.set( 4, -1.0 ); z.set( 5, 1.0 );
    x = xorg;
    r = (x /= y);
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    BOOST_CHECK( x == r );
    x = xorg;
    BOOST_CHECK( x.binaryOp( y, fo_divides0<Real>() ) == r );
    BOOST_CHECK( x == r );
}


BOOST_AUTO_TEST_CASE( SimilarFactorTransformationsTest ) {
    size_t N = 6;
    Var v( 0, N );
    Factor x( v );
    x.set( 0, 2.0 ); x.set( 1, 0.0 ); x.set( 2, 1.0 ); x.set( 3, 0.0 ); x.set( 4, 2.0 ); x.set( 5, 3.0 );
    Factor y( v );
    y.set( 0, 0.5 ); y.set( 1, -1.0 ); y.set( 2, 0.0 ); y.set( 3, 0.0 ); y.set( 4, -2.0 ); y.set( 5, 3.0 );
    Factor z( v ), r( v );

    z.set( 0, 2.5 ); z.set( 1, -1.0 ); z.set( 2, 1.0 ); z.set( 3, 0.0 ); z.set( 4, 0.0 ); z.set( 5, 6.0 );
    r = x + y;
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    z = x.binaryTr( y, std::plus<Real>() );
    BOOST_CHECK( r == z );

    z.set( 0, 1.5 ); z.set( 1, 1.0 ); z.set( 2, 1.0 ); z.set( 3, 0.0 ); z.set( 4, 4.0 ); z.set( 5, 0.0 );
    r = x - y;
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    z = x.binaryTr( y, std::minus<Real>() );
    BOOST_CHECK( r == z );

    z.set( 0, 1.0 ); z.set( 1, 0.0 ); z.set( 2, 0.0 ); z.set( 3, 0.0 ); z.set( 4, -4.0 ); z.set( 5, 9.0 );
    r = x * y;
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    z = x.binaryTr( y, std::multiplies<Real>() );
    BOOST_CHECK( r == z );

    z.set( 0, 4.0 ); z.set( 1, 0.0 ); z.set( 2, 0.0 ); z.set( 3, 0.0 ); z.set( 4, -1.0 ); z.set( 5, 1.0 );
    r = x / y;
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    z = x.binaryTr( y, fo_divides0<Real>() );
    BOOST_CHECK( r == z );
}


BOOST_AUTO_TEST_CASE( FactorOperationsTest ) {
    size_t N = 9;
    Var v1( 1, 3 );
    Var v2( 2, 3 );
    Factor xorg( v1 ), x( v1 );
    xorg.set( 0, 2.0 ); xorg.set( 1, 0.0 ); xorg.set( 2, -1.0 );
    Factor y( v2 );
    y.set( 0, 0.5 ); y.set( 1, -1.0 ); y.set( 2, 0.0 );
    Factor r;

    Factor z( VarSet( v1, v2 ) );
    z.set( 0,  2.5 ); z.set( 1,  0.5 ); z.set( 2, -0.5 ); 
    z.set( 3,  1.0 ); z.set( 4, -1.0 ); z.set( 5, -2.0 );
    z.set( 6,  2.0 ); z.set( 7,  0.0 ); z.set( 8, -1.0 );
    x = xorg;
    r = (x += y);
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    BOOST_CHECK( x == r );
    x = xorg;
    BOOST_CHECK( x.binaryOp( y, std::plus<Real>() ) == r );
    BOOST_CHECK( x == r );

    z.set( 0,  1.5 ); z.set( 1, -0.5 ); z.set( 2, -1.5 ); 
    z.set( 3,  3.0 ); z.set( 4,  1.0 ); z.set( 5,  0.0 );
    z.set( 6,  2.0 ); z.set( 7,  0.0 ); z.set( 8, -1.0 );
    x = xorg;
    r = (x -= y);
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    BOOST_CHECK( x == r );
    x = xorg;
    BOOST_CHECK( x.binaryOp( y, std::minus<Real>() ) == r );
    BOOST_CHECK( x == r );

    z.set( 0,  1.0 ); z.set( 1,  0.0 ); z.set( 2, -0.5 ); 
    z.set( 3, -2.0 ); z.set( 4,  0.0 ); z.set( 5,  1.0 );
    z.set( 6,  0.0 ); z.set( 7,  0.0 ); z.set( 8,  0.0 );
    x = xorg;
    r = (x *= y);
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    BOOST_CHECK( x == r );
    x = xorg;
    BOOST_CHECK( x.binaryOp( y, std::multiplies<Real>() ) == r );
    BOOST_CHECK( x == r );

    z.set( 0,  4.0 ); z.set( 1,  0.0 ); z.set( 2, -2.0 ); 
    z.set( 3, -2.0 ); z.set( 4,  0.0 ); z.set( 5,  1.0 );
    z.set( 6,  0.0 ); z.set( 7,  0.0 ); z.set( 8,  0.0 );
    x = xorg;
    r = (x /= y);
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    BOOST_CHECK( x == r );
    x = xorg;
    BOOST_CHECK( x.binaryOp( y, fo_divides0<Real>() ) == r );
    BOOST_CHECK( x == r );
}


BOOST_AUTO_TEST_CASE( FactorTransformationsTest ) {
    size_t N = 9;
    Var v1( 1, 3 );
    Var v2( 2, 3 );
    Factor x( v1 );
    x.set( 0, 2.0 ); x.set( 1, 0.0 ); x.set( 2, -1.0 );
    Factor y( v2 );
    y.set( 0, 0.5 ); y.set( 1, -1.0 ); y.set( 2, 0.0 );
    Factor r;

    Factor z( VarSet( v1, v2 ) );
    z.set( 0,  2.5 ); z.set( 1,  0.5 ); z.set( 2, -0.5 ); 
    z.set( 3,  1.0 ); z.set( 4, -1.0 ); z.set( 5, -2.0 );
    z.set( 6,  2.0 ); z.set( 7,  0.0 ); z.set( 8, -1.0 );
    r = x + y;
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    z = x.binaryTr( y, std::plus<Real>() );
    BOOST_CHECK( r == z );

    z.set( 0,  1.5 ); z.set( 1, -0.5 ); z.set( 2, -1.5 ); 
    z.set( 3,  3.0 ); z.set( 4,  1.0 ); z.set( 5,  0.0 );
    z.set( 6,  2.0 ); z.set( 7,  0.0 ); z.set( 8, -1.0 );
    r = x - y;
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    z = x.binaryTr( y, std::minus<Real>() );
    BOOST_CHECK( r == z );

    z.set( 0,  1.0 ); z.set( 1,  0.0 ); z.set( 2, -0.5 ); 
    z.set( 3, -2.0 ); z.set( 4,  0.0 ); z.set( 5,  1.0 );
    z.set( 6,  0.0 ); z.set( 7,  0.0 ); z.set( 8,  0.0 );
    r = x * y;
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    z = x.binaryTr( y, std::multiplies<Real>() );
    BOOST_CHECK( r == z );

    z.set( 0,  4.0 ); z.set( 1,  0.0 ); z.set( 2, -2.0 ); 
    z.set( 3, -2.0 ); z.set( 4,  0.0 ); z.set( 5,  1.0 );
    z.set( 6,  0.0 ); z.set( 7,  0.0 ); z.set( 8,  0.0 );
    r = x / y;
    for( size_t i = 0; i < N; i++ )
        BOOST_CHECK_CLOSE( r[i], z[i], tol );
    z = x.binaryOp( y, fo_divides0<Real>() );
    BOOST_CHECK( r == z );
}


BOOST_AUTO_TEST_CASE( MiscOperationsTest ) {
    Var v1(1, 2);
    Var v2(2, 3);
    Factor x( VarSet( v1, v2 ) );
    x.randomize();

    // slice
    Factor y = x.slice( v1, 0 );
    BOOST_CHECK( y.vars() == VarSet( v2 ) );
    BOOST_CHECK_EQUAL( y.nrStates(), 3 );
    BOOST_CHECK_EQUAL( y[0], x[0] );
    BOOST_CHECK_EQUAL( y[1], x[2] );
    BOOST_CHECK_EQUAL( y[2], x[4] );
    y = x.slice( v1, 1 );
    BOOST_CHECK( y.vars() == VarSet( v2 ) );
    BOOST_CHECK_EQUAL( y.nrStates(), 3 );
    BOOST_CHECK_EQUAL( y[0], x[1] );
    BOOST_CHECK_EQUAL( y[1], x[3] );
    BOOST_CHECK_EQUAL( y[2], x[5] );
    y = x.slice( v2, 0 );
    BOOST_CHECK( y.vars() == VarSet( v1 ) );
    BOOST_CHECK_EQUAL( y.nrStates(), 2 );
    BOOST_CHECK_EQUAL( y[0], x[0] );
    BOOST_CHECK_EQUAL( y[1], x[1] );
    y = x.slice( v2, 1 );
    BOOST_CHECK( y.vars() == VarSet( v1 ) );
    BOOST_CHECK_EQUAL( y.nrStates(), 2 );
    BOOST_CHECK_EQUAL( y[0], x[2] );
    BOOST_CHECK_EQUAL( y[1], x[3] );
    y = x.slice( v2, 2 );
    BOOST_CHECK( y.vars() == VarSet( v1 ) );
    BOOST_CHECK_EQUAL( y.nrStates(), 2 );
    BOOST_CHECK_EQUAL( y[0], x[4] );
    BOOST_CHECK_EQUAL( y[1], x[5] );
    for( size_t i = 0; i < x.nrStates(); i++ ) {
        y = x.slice( VarSet( v1, v2 ), 0 );
        BOOST_CHECK( y.vars() == VarSet() );
        BOOST_CHECK_EQUAL( y.nrStates(), 1 );
        BOOST_CHECK_EQUAL( y[0], x[0] );
    }
    y = x.slice( VarSet(), 0 );
    BOOST_CHECK_EQUAL( y, x );

    // embed
    Var v3(3, 4);
    BOOST_CHECK_THROW( x.embed( VarSet( v3 ) ), Exception );
    BOOST_CHECK_THROW( x.embed( VarSet( v3, v2 ) ), Exception );
    y = x.embed( VarSet( v3, v2 ) | v1 );
    for( size_t i = 0; i < y.nrStates(); i++ )
        BOOST_CHECK_EQUAL( y[i], x[i % 6] );
    y = x.embed( VarSet( v1, v2 ) );
    BOOST_CHECK_EQUAL( x, y );

    // marginal
    y = x.marginal( v1 );
    BOOST_CHECK( y.vars() == VarSet( v1 ) );
    BOOST_CHECK_CLOSE( y[0], (x[0] + x[2] + x[4]) / x.sum(), tol );
    BOOST_CHECK_CLOSE( y[1], (x[1] + x[3] + x[5]) / x.sum(),tol );
    y = x.marginal( v2 );
    BOOST_CHECK( y.vars() == VarSet( v2 ) );
    BOOST_CHECK_CLOSE( y[0], (x[0] + x[1]) / x.sum(), tol );
    BOOST_CHECK_CLOSE( y[1], (x[2] + x[3]) / x.sum(), tol );
    BOOST_CHECK_CLOSE( y[2], (x[4] + x[5]) / x.sum(), tol );
    y = x.marginal( VarSet() );
    BOOST_CHECK( y.vars() == VarSet() );
    BOOST_CHECK_CLOSE( y[0], (Real)1.0, tol );
    y = x.marginal( VarSet( v1, v2 ) );
    BOOST_CHECK_SMALL( dist( y, x.normalized(), DISTL1 ), tol );

    y = x.marginal( v1, true );
    BOOST_CHECK( y.vars() == VarSet( v1 ) );
    BOOST_CHECK_CLOSE( y[0], (x[0] + x[2] + x[4]) / x.sum(), tol );
    BOOST_CHECK_CLOSE( y[1], (x[1] + x[3] + x[5]) / x.sum(), tol );
    y = x.marginal( v2, true );
    BOOST_CHECK( y.vars() == VarSet( v2 ) );
    BOOST_CHECK_CLOSE( y[0], (x[0] + x[1]) / x.sum(), tol );
    BOOST_CHECK_CLOSE( y[1], (x[2] + x[3]) / x.sum(), tol );
    BOOST_CHECK_CLOSE( y[2], (x[4] + x[5]) / x.sum(), tol );
    y = x.marginal( VarSet(), true );
    BOOST_CHECK( y.vars() == VarSet() );
    BOOST_CHECK_CLOSE( y[0], (Real)1.0, tol );
    y = x.marginal( VarSet( v1, v2 ), true );
    BOOST_CHECK_SMALL( dist( y, x.normalized(), DISTL1 ), tol );

    y = x.marginal( v1, false );
    BOOST_CHECK( y.vars() == VarSet( v1 ) );
    BOOST_CHECK_CLOSE( y[0], x[0] + x[2] + x[4], tol );
    BOOST_CHECK_CLOSE( y[1], x[1] + x[3] + x[5], tol );
    y = x.marginal( v2, false );
    BOOST_CHECK( y.vars() == VarSet( v2 ) );
    BOOST_CHECK_CLOSE( y[0], x[0] + x[1], tol );
    BOOST_CHECK_CLOSE( y[1], x[2] + x[3], tol );
    BOOST_CHECK_CLOSE( y[2], x[4] + x[5], tol );
    y = x.marginal( VarSet(), false );
    BOOST_CHECK( y.vars() == VarSet() );
    BOOST_CHECK_CLOSE( y[0], x.sum(), tol );
    y = x.marginal( VarSet( v1, v2 ), false );
    BOOST_CHECK_SMALL( dist( y, x, DISTL1 ), tol );

    // maxMarginal
    y = x.maxMarginal( v1 );
    BOOST_CHECK( y.vars() == VarSet( v1 ) );
    BOOST_CHECK_CLOSE( y[0], x.slice( v1, 0 ).max() / (x.slice( v1, 0 ).max() + x.slice( v1, 1 ).max()), tol );
    BOOST_CHECK_CLOSE( y[1], x.slice( v1, 1 ).max() / (x.slice( v1, 0 ).max() + x.slice( v1, 1 ).max()), tol );
    y = x.maxMarginal( v2 );
    BOOST_CHECK( y.vars() == VarSet( v2 ) );
    BOOST_CHECK_CLOSE( y[0], x.slice( v2, 0 ).max() / (x.slice( v2, 0 ).max() + x.slice( v2, 1 ).max() + x.slice( v2, 2 ).max()), tol );
    BOOST_CHECK_CLOSE( y[1], x.slice( v2, 1 ).max() / (x.slice( v2, 0 ).max() + x.slice( v2, 1 ).max() + x.slice( v2, 2 ).max()), tol );
    BOOST_CHECK_CLOSE( y[2], x.slice( v2, 2 ).max() / (x.slice( v2, 0 ).max() + x.slice( v2, 1 ).max() + x.slice( v2, 2 ).max()), tol );
    y = x.maxMarginal( VarSet() );
    BOOST_CHECK( y.vars() == VarSet() );
    BOOST_CHECK_CLOSE( y[0], (Real)1.0, tol );
    y = x.maxMarginal( VarSet( v1, v2 ) );
    BOOST_CHECK_SMALL( dist( y, x.normalized(), DISTL1 ), tol );

    y = x.maxMarginal( v1, true );
    BOOST_CHECK( y.vars() == VarSet( v1 ) );
    BOOST_CHECK_CLOSE( y[0], x.slice( v1, 0 ).max() / (x.slice( v1, 0 ).max() + x.slice( v1, 1 ).max()), tol );
    BOOST_CHECK_CLOSE( y[1], x.slice( v1, 1 ).max() / (x.slice( v1, 0 ).max() + x.slice( v1, 1 ).max()), tol );
    y = x.maxMarginal( v2, true );
    BOOST_CHECK( y.vars() == VarSet( v2 ) );
    BOOST_CHECK_CLOSE( y[0], x.slice( v2, 0 ).max() / (x.slice( v2, 0 ).max() + x.slice( v2, 1 ).max() + x.slice( v2, 2 ).max()), tol );
    BOOST_CHECK_CLOSE( y[1], x.slice( v2, 1 ).max() / (x.slice( v2, 0 ).max() + x.slice( v2, 1 ).max() + x.slice( v2, 2 ).max()), tol );
    BOOST_CHECK_CLOSE( y[2], x.slice( v2, 2 ).max() / (x.slice( v2, 0 ).max() + x.slice( v2, 1 ).max() + x.slice( v2, 2 ).max()), tol );
    y = x.maxMarginal( VarSet(), true );
    BOOST_CHECK( y.vars() == VarSet() );
    BOOST_CHECK_CLOSE( y[0], (Real)1.0, tol );
    y = x.maxMarginal( VarSet( v1, v2 ), true );
    BOOST_CHECK_SMALL( dist( y, x.normalized(), DISTL1 ), tol );

    y = x.maxMarginal( v1, false );
    BOOST_CHECK( y.vars() == VarSet( v1 ) );
    BOOST_CHECK_EQUAL( y[0], x.slice( v1, 0 ).max() );
    BOOST_CHECK_EQUAL( y[1], x.slice( v1, 1 ).max() );
    y = x.maxMarginal( v2, false );
    BOOST_CHECK( y.vars() == VarSet( v2 ) );
    BOOST_CHECK_EQUAL( y[0], x.slice( v2, 0 ).max() );
    BOOST_CHECK_EQUAL( y[1], x.slice( v2, 1 ).max() );
    BOOST_CHECK_EQUAL( y[2], x.slice( v2, 2 ).max() );
    y = x.maxMarginal( VarSet(), false );
    BOOST_CHECK( y.vars() == VarSet() );
    BOOST_CHECK_EQUAL( y[0], x.max() );
    y = x.maxMarginal( VarSet( v1, v2 ), false );
    BOOST_CHECK( y == x );
}


BOOST_AUTO_TEST_CASE( RelatedFunctionsTest ) {
    Var v( 0, 3 );
    Factor x(v), y(v), z(v);
    x.set( 0, 0.2 );
    x.set( 1, 0.8 );
    x.set( 2, 0.0 );
    y.set( 0, 0.0 );
    y.set( 1, 0.6 );
    y.set( 2, 0.4 );

    z = min( x, y );
    BOOST_CHECK_EQUAL( z[0], (Real)0.0 );
    BOOST_CHECK_EQUAL( z[1], (Real)0.6 );
    BOOST_CHECK_EQUAL( z[2], (Real)0.0 );
    z = max( x, y );
    BOOST_CHECK_EQUAL( z[0], (Real)0.2 );
    BOOST_CHECK_EQUAL( z[1], (Real)0.8 );
    BOOST_CHECK_EQUAL( z[2], (Real)0.4 );

    BOOST_CHECK_CLOSE( dist( x, x, DISTL1 ), (Real)0.0, tol );
    BOOST_CHECK_CLOSE( dist( y, y, DISTL1 ), (Real)0.0, tol );
    BOOST_CHECK_CLOSE( dist( x, y, DISTL1 ), (Real)(0.2 + 0.2 + 0.4), tol );
    BOOST_CHECK_CLOSE( dist( y, x, DISTL1 ), (Real)(0.2 + 0.2 + 0.4), tol );
    BOOST_CHECK_CLOSE( dist( x, x, DISTLINF ), (Real)0.0, tol );
    BOOST_CHECK_CLOSE( dist( y, y, DISTLINF ), (Real)0.0, tol );
    BOOST_CHECK_CLOSE( dist( x, y, DISTLINF ), (Real)0.4, tol );
    BOOST_CHECK_CLOSE( dist( y, x, DISTLINF ), (Real)0.4, tol );
    BOOST_CHECK_CLOSE( dist( x, x, DISTTV ), (Real)0.0, tol );
    BOOST_CHECK_CLOSE( dist( y, y, DISTTV ), (Real)0.0, tol );
    BOOST_CHECK_CLOSE( dist( x, y, DISTTV ), (Real)(0.5 * (0.2 + 0.2 + 0.4)), tol );
    BOOST_CHECK_CLOSE( dist( y, x, DISTTV ), (Real)(0.5 * (0.2 + 0.2 + 0.4)), tol );
    BOOST_CHECK_CLOSE( dist( x, x, DISTKL ), (Real)0.0, tol );
    BOOST_CHECK_CLOSE( dist( y, y, DISTKL ), (Real)0.0, tol );
    BOOST_CHECK_EQUAL( dist( x, y, DISTKL ), INFINITY );
    BOOST_CHECK_EQUAL( dist( y, x, DISTKL ), INFINITY );
    BOOST_CHECK_CLOSE( dist( x, x, DISTHEL ), (Real)0.0, tol );
    BOOST_CHECK_CLOSE( dist( y, y, DISTHEL ), (Real)0.0, tol );
    BOOST_CHECK_CLOSE( dist( x, y, DISTHEL ), (Real)(0.5 * (0.2 + dai::pow(std::sqrt(0.8) - std::sqrt(0.6), 2.0) + 0.4)), tol );
    BOOST_CHECK_CLOSE( dist( y, x, DISTHEL ), (Real)(0.5 * (0.2 + dai::pow(std::sqrt(0.8) - std::sqrt(0.6), 2.0) + 0.4)), tol );
    x.set( 1, 0.7 ); x.set( 2, 0.1 );
    y.set( 0, 0.1 ); y.set( 1, 0.5 );
    BOOST_CHECK_CLOSE( dist( x, y, DISTKL ), (Real)(0.2 * dai::log(0.2 / 0.1) + 0.7 * dai::log(0.7 / 0.5) + 0.1 * dai::log(0.1 / 0.4)), tol );
    BOOST_CHECK_CLOSE( dist( y, x, DISTKL ), (Real)(0.1 * dai::log(0.1 / 0.2) + 0.5 * dai::log(0.5 / 0.7) + 0.4 * dai::log(0.4 / 0.1)), tol );

    std::stringstream ss;
    ss << x;
    std::string s;
    std::getline( ss, s );
    BOOST_CHECK_EQUAL( s, std::string("({x0}, (0.2, 0.7, 0.1))") );
    std::stringstream ss2;
    ss2 << y;
    std::getline( ss2, s );
    BOOST_CHECK_EQUAL( s, std::string("({x0}, (0.1, 0.5, 0.4))") );

    z = min( x, y );
    BOOST_CHECK_EQUAL( z[0], (Real)0.1 );
    BOOST_CHECK_EQUAL( z[1], (Real)0.5 );
    BOOST_CHECK_EQUAL( z[2], (Real)0.1 );
    z = max( x, y );
    BOOST_CHECK_EQUAL( z[0], (Real)0.2 );
    BOOST_CHECK_EQUAL( z[1], (Real)0.7 );
    BOOST_CHECK_EQUAL( z[2], (Real)0.4 );

    for( Real J = -1.0; J <= 1.01; J += 0.1 ) {
        Factor x = createFactorIsing( Var(0,2), Var(1,2), J ).normalized();
        BOOST_CHECK_CLOSE( x[0], dai::exp(J) / (4.0 * std::cosh(J)), tol );
        BOOST_CHECK_CLOSE( x[1], dai::exp(-J) / (4.0 * std::cosh(J)), tol );
        BOOST_CHECK_CLOSE( x[2], dai::exp(-J) / (4.0 * std::cosh(J)), tol );
        BOOST_CHECK_CLOSE( x[3], dai::exp(J) / (4.0 * std::cosh(J)), tol );
        BOOST_CHECK_SMALL( MutualInfo( x ) - (J * std::tanh(J) - dai::log(std::cosh(J))), tol );
    }
    Var v1( 1, 3 );
    Var v2( 2, 4 );
    BOOST_CHECK_SMALL( MutualInfo( (Factor(v1).randomize() * Factor(v2).randomize()).normalized() ), tol );
    BOOST_CHECK_THROW( MutualInfo( createFactorIsing( Var(0,2), 1.0 ).normalized() ), Exception );
    BOOST_CHECK_THROW( createFactorIsing( v1, 0.0 ), Exception );
    BOOST_CHECK_THROW( createFactorIsing( v1, v2, 0.0 ), Exception );
    for( Real J = -1.0; J <= 1.01; J += 0.1 ) {
        Factor x = createFactorIsing( Var(0,2), J ).normalized();
        BOOST_CHECK_CLOSE( x[0], dai::exp(-J) / (2.0 * std::cosh(J)), tol );
        BOOST_CHECK_CLOSE( x[1], dai::exp(J) / (2.0 * std::cosh(J)), tol );
        BOOST_CHECK_SMALL( x.entropy() - (-J * std::tanh(J) + dai::log(2.0 * std::cosh(J))), tol );
    }
    
    x = createFactorDelta( v1, 2 );
    BOOST_CHECK_EQUAL( x[0], (Real)0.0 );
    BOOST_CHECK_EQUAL( x[1], (Real)0.0 );
    BOOST_CHECK_EQUAL( x[2], (Real)1.0 );
    x = createFactorDelta( v1, 1 );
    BOOST_CHECK_EQUAL( x[0], (Real)0.0 );
    BOOST_CHECK_EQUAL( x[1], (Real)1.0 );
    BOOST_CHECK_EQUAL( x[2], (Real)0.0 );
    x = createFactorDelta( v1, 0 );
    BOOST_CHECK_EQUAL( x[0], (Real)1.0 );
    BOOST_CHECK_EQUAL( x[1], (Real)0.0 );
    BOOST_CHECK_EQUAL( x[2], (Real)0.0 );
    BOOST_CHECK_THROW( createFactorDelta( v1, 4 ), Exception );

    for( size_t i = 0; i < 12; i++ ) {
        Factor xx = createFactorDelta( VarSet( v1, v2 ), i );
        for( size_t j = 0; j < 12; j++ )
            BOOST_CHECK_EQUAL( xx[j], (Real)(i == j) );
    }
    BOOST_CHECK_THROW( createFactorDelta( VarSet( v1, v2 ), 12 ), Exception );
}
