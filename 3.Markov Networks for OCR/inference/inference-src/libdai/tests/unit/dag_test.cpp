/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/dag.h>
#include <vector>
#include <set>
#include <strstream>


using namespace dai;


#define BOOST_TEST_MODULE DAGTest


#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( ConstructorsTest ) {
    // check constructors
    DAG G0;
    BOOST_CHECK_EQUAL( G0.nrNodes(), 0 );
    BOOST_CHECK_EQUAL( G0.nrEdges(), 0 );
    BOOST_CHECK_EQUAL( G0.isConnected(), true );
    G0.checkConsistency();

    DAG G2( 2 );
    BOOST_CHECK_EQUAL( G2.nrNodes(), 2 );
    BOOST_CHECK_EQUAL( G2.nrEdges(), 0 );
    BOOST_CHECK_EQUAL( G2.isConnected(), false );
    G2.checkConsistency();
    BOOST_CHECK( !(G2 == G0) );
    
    std::vector<Edge> edges;
    edges.push_back( Edge( 0, 1 ) );
    edges.push_back( Edge( 1, 2 ) );
    edges.push_back( Edge( 2, 1 ) );
    edges.push_back( Edge( 1, 2 ) );
    DAG G3( 3, edges.begin(), edges.end() );
    BOOST_CHECK_EQUAL( G3.nrNodes(), 3 );
    BOOST_CHECK_EQUAL( G3.nrEdges(), 2 );
    BOOST_CHECK_EQUAL( G3.isConnected(), true );
    G3.checkConsistency();
    BOOST_CHECK( !(G3 == G0) );
    BOOST_CHECK( !(G3 == G2) );

    DAG G4( G3 );
    BOOST_CHECK( !(G4 == G0) );
    BOOST_CHECK( !(G4 == G2) );
    BOOST_CHECK( G4 == G3 );

    DAG G5 = G3;
    BOOST_CHECK( !(G5 == G0) );
    BOOST_CHECK( !(G5 == G2) );
    BOOST_CHECK( G5 == G3 );
}


BOOST_AUTO_TEST_CASE( NeighborTest ) {
    // check pa(), ch() accessors / mutators
    std::vector<Edge> edges;
    edges.push_back( Edge( 0, 2 ) );
    edges.push_back( Edge( 1, 2 ) );
    edges.push_back( Edge( 1, 3 ) );
    DAG G( 4, edges.begin(), edges.end() );

    BOOST_CHECK_EQUAL( G.pa(0).size(), 0 );
    BOOST_CHECK_EQUAL( G.pa(1).size(), 0 );
    BOOST_CHECK_EQUAL( G.pa(2).size(), 2 );
    BOOST_CHECK_EQUAL( G.pa(3).size(), 1 );
    BOOST_CHECK_EQUAL( G.pa(2,0).iter, 0 );
    BOOST_CHECK_EQUAL( G.pa(2,0).node, 0 );
    BOOST_CHECK_EQUAL( G.pa(2,0).dual, 0 );
    BOOST_CHECK_EQUAL( G.pa(2,1).iter, 1 );
    BOOST_CHECK_EQUAL( G.pa(2,1).node, 1 );
    BOOST_CHECK_EQUAL( G.pa(2,1).dual, 0 );
    BOOST_CHECK_EQUAL( G.pa(3,0).iter, 0 );
    BOOST_CHECK_EQUAL( G.pa(3,0).node, 1 );
    BOOST_CHECK_EQUAL( G.pa(3,0).dual, 1 );

    BOOST_CHECK( G.paSet(0) == SmallSet<size_t>() );
    BOOST_CHECK( G.paSet(1) == SmallSet<size_t>() );
    BOOST_CHECK( G.paSet(2) == SmallSet<size_t>(0,1) );
    BOOST_CHECK( G.paSet(3) == SmallSet<size_t>(1) );

    BOOST_CHECK_EQUAL( G.ch(0).size(), 1 );
    BOOST_CHECK_EQUAL( G.ch(1).size(), 2 );
    BOOST_CHECK_EQUAL( G.ch(2).size(), 0 );
    BOOST_CHECK_EQUAL( G.ch(3).size(), 0 );
    BOOST_CHECK_EQUAL( G.ch(0,0).iter, 0 );
    BOOST_CHECK_EQUAL( G.ch(0,0).node, 2 );
    BOOST_CHECK_EQUAL( G.ch(0,0).dual, 0 );
    BOOST_CHECK_EQUAL( G.ch(1,0).iter, 0 );
    BOOST_CHECK_EQUAL( G.ch(1,0).node, 2 );
    BOOST_CHECK_EQUAL( G.ch(1,0).dual, 1 );
    BOOST_CHECK_EQUAL( G.ch(1,1).iter, 1 );
    BOOST_CHECK_EQUAL( G.ch(1,1).node, 3 );
    BOOST_CHECK_EQUAL( G.ch(1,1).dual, 0 );

    BOOST_CHECK( G.chSet(0) == SmallSet<size_t>(2) );
    BOOST_CHECK( G.chSet(1) == SmallSet<size_t>(2,3) );
    BOOST_CHECK( G.chSet(2) == SmallSet<size_t>() );
    BOOST_CHECK( G.chSet(3) == SmallSet<size_t>() );
}


BOOST_AUTO_TEST_CASE( AddEraseTest ) {
    // check addition and erasure of nodes and edges
    std::vector<Edge> edges;
    edges.push_back( Edge( 0, 1 ) );
    edges.push_back( Edge( 1, 2 ) );
    edges.push_back( Edge( 1, 0 ) );
    DAG G( 2 );
    G.construct( 3, edges.begin(), edges.end() );
    G.checkConsistency();
    BOOST_CHECK_EQUAL( G.nrNodes(), 3 );
    BOOST_CHECK_EQUAL( G.nrEdges(), 2 );
    BOOST_CHECK_EQUAL( G, DAG(3).addEdge(0,1).addEdge(1,2) );
    BOOST_CHECK_EQUAL( G.addNode(), 3 );
    BOOST_CHECK_EQUAL( G, DAG(4).addEdge(0,1).addEdge(1,2) );
    G.checkConsistency();
    std::vector<size_t> pa;
    pa.push_back( 3 );
    BOOST_CHECK_EQUAL( G.addNode( pa.begin(), pa.end() ), 4 );
    BOOST_CHECK_EQUAL( G, DAG(5).addEdge(0,1).addEdge(1,2).addEdge(3,4) );
    BOOST_CHECK_EQUAL( G.addNode(), 5 );
    BOOST_CHECK_EQUAL( G, DAG(6).addEdge(0,1).addEdge(1,2).addEdge(3,4) );
    G.checkConsistency();
    G.addEdge( 0, 4 );
    BOOST_CHECK_EQUAL( G, DAG(6).addEdge(0,1).addEdge(1,2).addEdge(3,4).addEdge(0,4) );
    G.checkConsistency();
    G.addEdge( 0, 5 );
    BOOST_CHECK_EQUAL( G, DAG(6).addEdge(0,1).addEdge(1,2).addEdge(3,4).addEdge(0,4).addEdge(0,5) );
    G.checkConsistency();
    BOOST_CHECK_EQUAL( G.nrNodes(), 6 );
    BOOST_CHECK_EQUAL( G.nrEdges(), 5 );
    G.addEdge( 2, 3 );
    BOOST_CHECK_EQUAL( G, DAG(6).addEdge(0,1).addEdge(1,2).addEdge(3,4).addEdge(0,4).addEdge(0,5).addEdge(2,3) );
    G.addEdge( 3, 0 );
    BOOST_CHECK_EQUAL( G, DAG(6).addEdge(0,1).addEdge(1,2).addEdge(3,4).addEdge(0,4).addEdge(0,5).addEdge(2,3) );

    G.addEdge( 5, 3 );
    BOOST_CHECK_EQUAL( G, DAG(6).addEdge(0,1).addEdge(1,2).addEdge(3,4).addEdge(0,4).addEdge(0,5).addEdge(2,3).addEdge(5,3) );
    G.eraseNode( 0 );
    BOOST_CHECK_EQUAL( G, DAG(5).addEdge(0,1).addEdge(1,2).addEdge(4,2).addEdge(2,3) );
    G.checkConsistency();
    G.eraseEdge( 0, 1 );
    BOOST_CHECK_EQUAL( G, DAG(5).addEdge(1,2).addEdge(4,2).addEdge(2,3) );
    G.checkConsistency();
    BOOST_CHECK( !G.isConnected() );
    G.eraseNode( 0 );
    BOOST_CHECK_EQUAL( G, DAG(4).addEdge(0,1).addEdge(3,1).addEdge(1,2) );
    G.checkConsistency();
    G.addEdge( 3, 2 );
    BOOST_CHECK_EQUAL( G, DAG(4).addEdge(0,1).addEdge(3,1).addEdge(1,2).addEdge(3,2) );
    G.checkConsistency();
    G.eraseNode( 1 );
    BOOST_CHECK_EQUAL( G, DAG(3).addEdge(2,1) );
    G.checkConsistency();
    BOOST_CHECK( !G.isConnected() );
    G.eraseNode( 2 );
    BOOST_CHECK_EQUAL( G, DAG(2) );
    G.checkConsistency();
    BOOST_CHECK( !G.isConnected() );
    G.addEdge( 1, 0 );
    BOOST_CHECK_EQUAL( G, DAG(2).addEdge(1,0) );
    G.checkConsistency();
    BOOST_CHECK( G.isConnected() );
    G.eraseNode( 1 );
    BOOST_CHECK_EQUAL( G, DAG(1) );
    G.checkConsistency();
    BOOST_CHECK( G.isConnected() );
    G.eraseNode( 0 );
    BOOST_CHECK_EQUAL( G, DAG() );
    BOOST_CHECK( G.isConnected() );
    BOOST_CHECK_EQUAL( G.nrNodes(), 0 );
    BOOST_CHECK_EQUAL( G.nrEdges(), 0 );

    G.addNode();
    G.addNode();
    G.addNode();
    G.addNode();
    G.addEdge( 0, 1 );
    G.addEdge( 2, 3 );
    G.addEdge( 0, 3 );
    G.checkConsistency();
    G.eraseNode( 2 );
    G.checkConsistency();
}


BOOST_AUTO_TEST_CASE( RandomAddEraseTest ) {
    // check adding and erasing nodes and edges randomly
    DAG G;
    for( size_t maxN = 2; maxN < 50; maxN++ )
        for( size_t repeats = 0; repeats < 10000; repeats++ ) {
            size_t action = rnd( 5 );
            size_t N = G.nrNodes();
            size_t M = G.nrEdges();
            size_t maxM = N * (N - 1) / 2;
            if( action == 0 ) {
                // add node
                if( N < maxN )
                    G.addNode();
            } else if( action == 1 ) {
                // erase node
                if( N > 0 )
                    G.eraseNode( rnd( N ) );
            } else if( action == 2 || action == 3 ) {
                // add edge
                if( N >= 2 && M < maxM ) {
                    size_t n1 = 0;
                    size_t n2 = 0;
                    do {
                        n1 = rnd( N );
                        n2 = rnd( N );
                    } while( n1 == n2 || G.ch(n1).size() >= N - 1 || G.pa(n2).size() >= N - 1 || G.hasEdge( n1, n2 ) || G.existsDirectedPath( n2, n1 ) );
                    G.addEdge( n1, n2 );
                }
            } else if( action == 4 ) {
                // erase edge
                if( M > 0 ) {
                    size_t n1 = 0;
                    do {
                        n1 = rnd( N );
                    } while( G.ch(n1).size() == 0 );
                    size_t n2 = 0;
                    do {
                        n2 = rnd( N );
                    } while( !G.hasEdge( n1, n2 ) );
                    G.eraseEdge( n1, n2 );
                }
            }
            G.checkConsistency();
        }
}


BOOST_AUTO_TEST_CASE( QueriesTest ) {
    // check queries which have not been tested in another test case
    std::vector<Edge> edges;
    edges.push_back( Edge( 0, 1 ) );
    edges.push_back( Edge( 0, 3 ) );
    edges.push_back( Edge( 2, 3 ) );
    DAG G( 4, edges.begin(), edges.end() );
    G.checkConsistency();
    
    BOOST_CHECK( !G.hasEdge( 0, 0 ) );
    BOOST_CHECK(  G.hasEdge( 0, 1 ) );
    BOOST_CHECK( !G.hasEdge( 0, 2 ) );
    BOOST_CHECK(  G.hasEdge( 0, 3 ) );
    BOOST_CHECK( !G.hasEdge( 1, 0 ) );
    BOOST_CHECK( !G.hasEdge( 1, 1 ) );
    BOOST_CHECK( !G.hasEdge( 1, 2 ) );
    BOOST_CHECK( !G.hasEdge( 1, 3 ) );
    BOOST_CHECK( !G.hasEdge( 2, 0 ) );
    BOOST_CHECK( !G.hasEdge( 2, 1 ) );
    BOOST_CHECK( !G.hasEdge( 2, 2 ) );
    BOOST_CHECK(  G.hasEdge( 2, 3 ) );
    BOOST_CHECK( !G.hasEdge( 3, 0 ) );
    BOOST_CHECK( !G.hasEdge( 3, 1 ) );
    BOOST_CHECK( !G.hasEdge( 3, 2 ) );
    BOOST_CHECK( !G.hasEdge( 3, 3 ) );
    
    BOOST_CHECK_THROW( G.findPa( 0, 0 ), Exception );
    BOOST_CHECK_THROW( G.findPa( 0, 1 ), Exception );
    BOOST_CHECK_THROW( G.findPa( 0, 2 ), Exception );
    BOOST_CHECK_THROW( G.findPa( 0, 3 ), Exception );
    BOOST_CHECK_EQUAL( G.findPa( 1, 0 ), 0 );
    BOOST_CHECK_THROW( G.findPa( 1, 1 ), Exception );
    BOOST_CHECK_THROW( G.findPa( 1, 2 ), Exception );
    BOOST_CHECK_THROW( G.findPa( 1, 3 ), Exception );
    BOOST_CHECK_THROW( G.findPa( 2, 0 ), Exception );
    BOOST_CHECK_THROW( G.findPa( 2, 1 ), Exception );
    BOOST_CHECK_THROW( G.findPa( 2, 2 ), Exception );
    BOOST_CHECK_THROW( G.findPa( 2, 3 ), Exception );
    BOOST_CHECK_EQUAL( G.findPa( 3, 0 ), 0 );
    BOOST_CHECK_THROW( G.findPa( 3, 1 ), Exception );
    BOOST_CHECK_EQUAL( G.findPa( 3, 2 ), 1 );
    BOOST_CHECK_THROW( G.findPa( 3, 3 ), Exception );

    BOOST_CHECK_THROW( G.findCh( 0, 0 ), Exception );
    BOOST_CHECK_EQUAL( G.findCh( 0, 1 ), 0 );
    BOOST_CHECK_THROW( G.findCh( 0, 2 ), Exception );
    BOOST_CHECK_EQUAL( G.findCh( 0, 3 ), 1 );
    BOOST_CHECK_THROW( G.findCh( 1, 0 ), Exception );
    BOOST_CHECK_THROW( G.findCh( 1, 1 ), Exception );
    BOOST_CHECK_THROW( G.findCh( 1, 2 ), Exception );
    BOOST_CHECK_THROW( G.findCh( 1, 3 ), Exception );
    BOOST_CHECK_THROW( G.findCh( 2, 0 ), Exception );
    BOOST_CHECK_THROW( G.findCh( 2, 1 ), Exception );
    BOOST_CHECK_THROW( G.findCh( 2, 2 ), Exception );
    BOOST_CHECK_EQUAL( G.findCh( 2, 3 ), 0 );
    BOOST_CHECK_THROW( G.findCh( 3, 0 ), Exception );
    BOOST_CHECK_THROW( G.findCh( 3, 1 ), Exception );
    BOOST_CHECK_THROW( G.findCh( 3, 2 ), Exception );
    BOOST_CHECK_THROW( G.findCh( 3, 3 ), Exception );
    
    BOOST_CHECK(  G.existsDirectedPath( 0, 0 ) );
    BOOST_CHECK(  G.existsDirectedPath( 0, 1 ) );
    BOOST_CHECK( !G.existsDirectedPath( 0, 2 ) );
    BOOST_CHECK(  G.existsDirectedPath( 0, 3 ) );
    BOOST_CHECK( !G.existsDirectedPath( 1, 0 ) );
    BOOST_CHECK(  G.existsDirectedPath( 1, 1 ) );
    BOOST_CHECK( !G.existsDirectedPath( 1, 2 ) );
    BOOST_CHECK( !G.existsDirectedPath( 1, 3 ) );
    BOOST_CHECK( !G.existsDirectedPath( 2, 0 ) );
    BOOST_CHECK( !G.existsDirectedPath( 2, 1 ) );
    BOOST_CHECK(  G.existsDirectedPath( 2, 2 ) );
    BOOST_CHECK(  G.existsDirectedPath( 2, 3 ) );
    BOOST_CHECK( !G.existsDirectedPath( 3, 0 ) );
    BOOST_CHECK( !G.existsDirectedPath( 3, 1 ) );
    BOOST_CHECK( !G.existsDirectedPath( 3, 2 ) );
    BOOST_CHECK(  G.existsDirectedPath( 3, 3 ) );

    std::set<size_t> s;
    G.addNode();
    G.addEdge( 3, 4 );
    G.addNode();
    G.addEdge( 1, 4 );
    G.addEdge( 1, 5 );
    BOOST_CHECK( G.descendants( 4 ) == s );
    BOOST_CHECK( G.descendants( 5 ) == s );
    s.insert( 4 );
    BOOST_CHECK( G.descendants( 3 ) == s );
    s.insert( 5 );
    BOOST_CHECK( G.descendants( 1 ) == s );
    s.erase( 5 );
    s.insert( 3 );
    BOOST_CHECK( G.descendants( 2 ) == s );
    s.insert( 1 );
    s.insert( 5 );
    BOOST_CHECK( G.descendants( 0 ) == s );

    s.clear();
    BOOST_CHECK( G.ancestors( 0 ) == s );
    BOOST_CHECK( G.ancestors( 2 ) == s );
    s.insert( 0 );
    BOOST_CHECK( G.ancestors( 1 ) == s );
    s.insert( 2 );
    BOOST_CHECK( G.ancestors( 3 ) == s );
    s.erase( 2 );
    s.insert( 1 );
    BOOST_CHECK( G.ancestors( 5 ) == s );
    s.insert( 2 );
    s.insert( 3 );
    BOOST_CHECK( G.ancestors( 4 ) == s );
}


BOOST_AUTO_TEST_CASE( StreamTest ) {
    // check printDot
    DAG G( 4 );
    G.addEdge( 0, 1 );
    G.addEdge( 0, 2 );
    G.addEdge( 1, 3 );
    G.addEdge( 2, 3 );
    G.addEdge( 2, 2 );
    G.addEdge( 3, 2 );

    std::stringstream ss;
    std::string s;

    G.printDot( ss );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "digraph DAG {" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "node[shape=circle,width=0.4,fixedsize=true];" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx2;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx3;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0 -> x1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0 -> x2;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx1 -> x3;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx2 -> x3;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "}" );

    ss << G;
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "digraph DAG {" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "node[shape=circle,width=0.4,fixedsize=true];" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx2;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx3;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0 -> x1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0 -> x2;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx1 -> x3;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx2 -> x3;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "}" );
}
