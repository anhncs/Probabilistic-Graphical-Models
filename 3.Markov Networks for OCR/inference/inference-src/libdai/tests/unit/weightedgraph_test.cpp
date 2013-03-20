/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/weightedgraph.h>
#include <dai/exceptions.h>
#include <strstream>
#include <vector>


using namespace dai;


#define BOOST_TEST_MODULE WeighedGraphTest


#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( DEdgeTest ) {
    DEdge a;
    BOOST_CHECK_EQUAL( a.first, 0 );
    BOOST_CHECK_EQUAL( a.second, 0 );
    DEdge b( 3, 5 );
    BOOST_CHECK_EQUAL( b.first, 3 );
    BOOST_CHECK_EQUAL( b.second, 5 );
    DEdge c( 5, 3 );
    BOOST_CHECK_EQUAL( c.first, 5 );
    BOOST_CHECK_EQUAL( c.second, 3 );
    DEdge d( c );
    DEdge e = c;
    DEdge f( 5, 4 );
    DEdge g( 3, 6 );

    BOOST_CHECK( !(a == b) );
    BOOST_CHECK( !(a == c) );
    BOOST_CHECK( !(b == c) );
    BOOST_CHECK( d == c );
    BOOST_CHECK( c == d );
    BOOST_CHECK( e == c );
    BOOST_CHECK( c == e );
    BOOST_CHECK( d == e );
    BOOST_CHECK( e == d );

    BOOST_CHECK( a < b );
    BOOST_CHECK( a < c );
    BOOST_CHECK( b < c );
    BOOST_CHECK( !(c < d) );
    BOOST_CHECK( !(b < a) );
    BOOST_CHECK( !(c < a) );
    BOOST_CHECK( !(c < b) );
    BOOST_CHECK( !(d < c) );
    BOOST_CHECK( a < f );
    BOOST_CHECK( b < f );
    BOOST_CHECK( c < f );
    BOOST_CHECK( g < f );
    BOOST_CHECK( !(f < a) );
    BOOST_CHECK( !(f < b) );
    BOOST_CHECK( !(f < c) );
    BOOST_CHECK( !(f < g) );
    BOOST_CHECK( a < g );
    BOOST_CHECK( b < g );
    BOOST_CHECK( !(c < g) );
    BOOST_CHECK( !(g < a) );
    BOOST_CHECK( !(g < b) );
    BOOST_CHECK( g < c );

    std::stringstream ss;
    ss << c;
    std::string s;
    ss >> s;
    BOOST_CHECK_EQUAL( s, "(5->3)" );
}


BOOST_AUTO_TEST_CASE( UEdgeTest ) {
    UEdge a;
    BOOST_CHECK_EQUAL( a.first, 0 );
    BOOST_CHECK_EQUAL( a.second, 0 );
    UEdge b( 3, 5 );
    BOOST_CHECK_EQUAL( b.first, 3 );
    BOOST_CHECK_EQUAL( b.second, 5 );
    UEdge c( 5, 3 );
    BOOST_CHECK_EQUAL( c.first, 5 );
    BOOST_CHECK_EQUAL( c.second, 3 );
    UEdge d( c );
    UEdge e = c;
    UEdge f( 5, 4 );
    UEdge g( 3, 6 );

    UEdge h( DEdge( 5, 3 ) );
    BOOST_CHECK_EQUAL( h.first, 5 );
    BOOST_CHECK_EQUAL( h.second, 3 );

    BOOST_CHECK( !(a == b) );
    BOOST_CHECK( !(a == c) );
    BOOST_CHECK( b == c );
    BOOST_CHECK( d == c );
    BOOST_CHECK( c == d );
    BOOST_CHECK( e == c );
    BOOST_CHECK( c == e );
    BOOST_CHECK( d == e );
    BOOST_CHECK( e == d );

    BOOST_CHECK( a < b );
    BOOST_CHECK( a < c );
    BOOST_CHECK( !(b < c) );
    BOOST_CHECK( !(c < d) );
    BOOST_CHECK( !(b < a) );
    BOOST_CHECK( !(c < a) );
    BOOST_CHECK( !(c < b) );
    BOOST_CHECK( !(d < c) );
    BOOST_CHECK( a < f );
    BOOST_CHECK( b < f );
    BOOST_CHECK( c < f );
    BOOST_CHECK( g < f );
    BOOST_CHECK( !(f < a) );
    BOOST_CHECK( !(f < b) );
    BOOST_CHECK( !(f < c) );
    BOOST_CHECK( !(f < g) );
    BOOST_CHECK( a < g );
    BOOST_CHECK( b < g );
    BOOST_CHECK( c < g );
    BOOST_CHECK( !(g < a) );
    BOOST_CHECK( !(g < b) );
    BOOST_CHECK( !(g < c) );

    std::stringstream ss;
    std::string s;
    ss << c;
    ss >> s;
    BOOST_CHECK_EQUAL( s, "{3--5}" );
    ss << b;
    ss >> s;
    BOOST_CHECK_EQUAL( s, "{3--5}" );
}


BOOST_AUTO_TEST_CASE( RootedTreeTest ) {
    typedef UEdge E;
    std::vector<E> edges;
    edges.push_back( E( 1, 2 ) );
    edges.push_back( E( 2, 3 ) );
    edges.push_back( E( 3, 1 ) );
    GraphEL G( edges.begin(), edges.end() );
    
    BOOST_CHECK_THROW( RootedTree T( G, 0 ), Exception );
    BOOST_CHECK_THROW( RootedTree T( G, 1 ), Exception );

    edges.back().second = 4;
    G = GraphEL( edges.begin(), edges.end() );
    RootedTree T;
    T = RootedTree( G, 1 );
    BOOST_CHECK_EQUAL( T.size(), 3 );
    BOOST_CHECK_EQUAL( T[0], DEdge( 1, 2 ) );
    BOOST_CHECK_EQUAL( T[1], DEdge( 2, 3 ) );
    BOOST_CHECK_EQUAL( T[2], DEdge( 3, 4 ) );

    edges.push_back( E(5, 6) );
    G = GraphEL( edges.begin(), edges.end() );
    BOOST_CHECK_THROW( RootedTree T( G, 1 ), Exception );

    GraphAL H( 3 );
    H.addEdge( 0, 1 );
    H.addEdge( 1, 2 );
    H.addEdge( 2, 1 );
    G = GraphEL( H );
    for( GraphEL::const_iterator e = G.begin(); e != G.end(); e++ )
        BOOST_CHECK( H.hasEdge( e->first, e->second ) );
}


BOOST_AUTO_TEST_CASE( SpanningTreeTest ) {
    RootedTree T;
    WeightedGraph<int> G;
    G[UEdge(0,1)] = 1;
    G[UEdge(0,2)] = 2;
    G[UEdge(1,2)] = 3;
    G[UEdge(1,3)] = 4;
    G[UEdge(2,3)] = 5;
   
    RootedTree TMin;
    TMin.push_back( DEdge( 0,1 ) );
    TMin.push_back( DEdge( 0,2 ) );
    TMin.push_back( DEdge( 1,3 ) );
    RootedTree TMax;
    TMax.push_back( DEdge( 0,2 ) );
    TMax.push_back( DEdge( 2,3 ) );
    TMax.push_back( DEdge( 3,1 ) );

    T = MinSpanningTree( G, true );
    BOOST_CHECK_EQUAL( T, TMin );
    T = MinSpanningTree( G, false );
    BOOST_CHECK_EQUAL( T, TMin );

    T = MaxSpanningTree( G, true );
    BOOST_CHECK_EQUAL( T, TMax );
    T = MaxSpanningTree( G, false );
    BOOST_CHECK_EQUAL( T, TMax );

    WeightedGraph<double> H;
    H[UEdge(1,2)] = 1;
    H[UEdge(1,3)] = 2;
    H[UEdge(2,3)] = 3;
    H[UEdge(2,4)] = 4;
    H[UEdge(3,4)] = 5;
    BOOST_CHECK_THROW( T = MinSpanningTree( H, true ), Exception );
}
