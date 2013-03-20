/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/bipgraph.h>
#include <vector>
#include <strstream>


using namespace dai;


#define BOOST_TEST_MODULE BipartiteGraphTest


#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( ConstructorsTest ) {
    // check constructors
    BipartiteGraph G;
    BOOST_CHECK_EQUAL( G.nrNodes1(), 0 );
    BOOST_CHECK_EQUAL( G.nrNodes2(), 0 );
    BOOST_CHECK_EQUAL( G.nrEdges(), 0 );
    BOOST_CHECK( G.isConnected() );
    BOOST_CHECK( G.isTree() );
    G.checkConsistency();

    BipartiteGraph G1( 2, 3 );
    BOOST_CHECK_EQUAL( G1.nrNodes1(), 2 );
    BOOST_CHECK_EQUAL( G1.nrNodes2(), 3 );
    BOOST_CHECK_EQUAL( G1.nrEdges(), 0 );
    BOOST_CHECK( !G1.isConnected() );
    BOOST_CHECK( !G1.isTree() );
    BOOST_CHECK( !(G1 == G) );
    G1.checkConsistency();

    std::vector<Edge> edges;
    edges.push_back( Edge(0, 0) );
    edges.push_back( Edge(0, 1) );
    edges.push_back( Edge(1, 1) );
    edges.push_back( Edge(1, 2) );
    edges.push_back( Edge(1, 2) );
    BipartiteGraph G2( 2, 3, edges.begin(), edges.end() );
    BOOST_CHECK_EQUAL( G2.nrNodes1(), 2 );
    BOOST_CHECK_EQUAL( G2.nrNodes2(), 3 );
    BOOST_CHECK_EQUAL( G2.nrEdges(), 4 );
    BOOST_CHECK( G2.isConnected() );
    BOOST_CHECK( G2.isTree() );
    BOOST_CHECK( !(G2 == G) );
    BOOST_CHECK( !(G2 == G1) );
    G2.checkConsistency();

    edges.push_back( Edge(1, 0) );
    BipartiteGraph G3( 2, 3, edges.begin(), edges.end() );
    BOOST_CHECK_EQUAL( G3.nrNodes1(), 2 );
    BOOST_CHECK_EQUAL( G3.nrNodes2(), 3 );
    BOOST_CHECK_EQUAL( G3.nrEdges(), 5 );
    BOOST_CHECK( G3.isConnected() );
    BOOST_CHECK( !G3.isTree() );
    BOOST_CHECK( !(G3 == G) );
    BOOST_CHECK( !(G3 == G1) );
    BOOST_CHECK( !(G3 == G2) );
    G3.checkConsistency();

    BipartiteGraph G4( 3, 3, edges.begin(), edges.end() );
    BOOST_CHECK_EQUAL( G4.nrNodes1(), 3 );
    BOOST_CHECK_EQUAL( G4.nrNodes2(), 3 );
    BOOST_CHECK_EQUAL( G4.nrEdges(), 5 );
    BOOST_CHECK( !G4.isConnected() );
    BOOST_CHECK( !G4.isTree() );
    BOOST_CHECK( !(G4 == G) );
    BOOST_CHECK( !(G4 == G1) );
    BOOST_CHECK( !(G4 == G2) );
    BOOST_CHECK( !(G4 == G3) );
    G4.checkConsistency();

    G.construct( 3, 3, edges.begin(), edges.end() );
    BOOST_CHECK_EQUAL( G.nrNodes1(), 3 );
    BOOST_CHECK_EQUAL( G.nrNodes2(), 3 );
    BOOST_CHECK_EQUAL( G.nrEdges(), 5 );
    BOOST_CHECK( !G.isConnected() );
    BOOST_CHECK( !G.isTree() );
    BOOST_CHECK( !(G == G1) );
    BOOST_CHECK( !(G == G2) );
    BOOST_CHECK( !(G == G3) );
    BOOST_CHECK( G == G4 );
    G.checkConsistency();
    
    BipartiteGraph G5( G4 );
    BOOST_CHECK( G5 == G4 );

    BipartiteGraph G6 = G4;
    BOOST_CHECK( G6 == G4 );
}


BOOST_AUTO_TEST_CASE( NeighborTest ) {
    // check nb() accessor / mutator
    std::vector<Edge> edges;
    edges.push_back( Edge(0, 0) );
    edges.push_back( Edge(0, 1) );
    edges.push_back( Edge(1, 1) );
    edges.push_back( Edge(1, 2) );
    BipartiteGraph G( 2, 3, edges.begin(), edges.end() );
    BOOST_CHECK_EQUAL( G.nb1(0).size(), 2 );
    BOOST_CHECK_EQUAL( G.nb1(1).size(), 2 );
    BOOST_CHECK_EQUAL( G.nb2(0).size(), 1 );
    BOOST_CHECK_EQUAL( G.nb2(1).size(), 2 );
    BOOST_CHECK_EQUAL( G.nb2(2).size(), 1 );
    BOOST_CHECK_EQUAL( G.nb1(0,0).iter, 0 );
    BOOST_CHECK_EQUAL( G.nb1(0,0).node, 0 );
    BOOST_CHECK_EQUAL( G.nb1(0,0).dual, 0 );
    BOOST_CHECK_EQUAL( G.nb1(0,1).iter, 1 );
    BOOST_CHECK_EQUAL( G.nb1(0,1).node, 1 );
    BOOST_CHECK_EQUAL( G.nb1(0,1).dual, 0 );
    BOOST_CHECK_EQUAL( G.nb1(1,0).iter, 0 );
    BOOST_CHECK_EQUAL( G.nb1(1,0).node, 1 );
    BOOST_CHECK_EQUAL( G.nb1(1,0).dual, 1 );
    BOOST_CHECK_EQUAL( G.nb1(1,1).iter, 1 );
    BOOST_CHECK_EQUAL( G.nb1(1,1).node, 2 );
    BOOST_CHECK_EQUAL( G.nb1(1,1).dual, 0 );
    BOOST_CHECK_EQUAL( G.nb2(0,0).iter, 0 );
    BOOST_CHECK_EQUAL( G.nb2(0,0).node, 0 );
    BOOST_CHECK_EQUAL( G.nb2(0,0).dual, 0 );
    BOOST_CHECK_EQUAL( G.nb2(1,0).iter, 0 );
    BOOST_CHECK_EQUAL( G.nb2(1,0).node, 0 );
    BOOST_CHECK_EQUAL( G.nb2(1,0).dual, 1 );
    BOOST_CHECK_EQUAL( G.nb2(1,1).iter, 1 );
    BOOST_CHECK_EQUAL( G.nb2(1,1).node, 1 );
    BOOST_CHECK_EQUAL( G.nb2(1,1).dual, 0 );
    BOOST_CHECK_EQUAL( G.nb2(2,0).iter, 0 );
    BOOST_CHECK_EQUAL( G.nb2(2,0).node, 1 );
    BOOST_CHECK_EQUAL( G.nb2(2,0).dual, 1 );
    BOOST_CHECK_EQUAL( G.nb1Set(0).size(), 2 );
    BOOST_CHECK_EQUAL( G.nb1Set(1).size(), 2 );
    BOOST_CHECK_EQUAL( G.nb2Set(0).size(), 1 );
    BOOST_CHECK_EQUAL( G.nb2Set(1).size(), 2 );
    BOOST_CHECK_EQUAL( G.nb2Set(2).size(), 1 );
    BOOST_CHECK( G.nb1Set(0) == SmallSet<size_t>( 0, 1 ) );
    BOOST_CHECK( G.nb1Set(1) == SmallSet<size_t>( 1, 2 ) );
    BOOST_CHECK( G.nb2Set(0) == SmallSet<size_t>( 0 ) );
    BOOST_CHECK( G.nb2Set(1) == SmallSet<size_t>( 0, 1 ) );
    BOOST_CHECK( G.nb2Set(2) == SmallSet<size_t>( 1 ) );
}


BOOST_AUTO_TEST_CASE( AddEraseTest ) {
    // check addition and erasure of nodes and edges
    std::vector<Edge> edges;
    edges.push_back( Edge( 0, 0 ) );
    edges.push_back( Edge( 0, 1 ) );
    edges.push_back( Edge( 1, 1 ) );
    BipartiteGraph G( 2, 2, edges.begin(), edges.end() );
    G.checkConsistency();
    BOOST_CHECK_EQUAL( G.nrNodes1(), 2 );
    BOOST_CHECK_EQUAL( G.nrNodes2(), 2 );
    BOOST_CHECK_EQUAL( G.nrEdges(), 3 );
    BOOST_CHECK( G.isConnected() );
    BOOST_CHECK( G.isTree() );
    BOOST_CHECK_EQUAL( G.addNode1(), 2 );
    BOOST_CHECK( !G.isConnected() );
    BOOST_CHECK( !G.isTree() );
    BOOST_CHECK_EQUAL( G.addNode2(), 2 );
    BOOST_CHECK( !G.isConnected() );
    BOOST_CHECK( !G.isTree() );
    BOOST_CHECK_EQUAL( G.addNode1(), 3 );
    BOOST_CHECK( !G.isConnected() );
    BOOST_CHECK( !G.isTree() );
    G.checkConsistency();
    std::vector<size_t> nbs;
    nbs.push_back( 2 );
    nbs.push_back( 0 );
    BOOST_CHECK_EQUAL( G.addNode1( nbs.begin(), nbs.end() ), 4 );
    G.checkConsistency();
    BOOST_CHECK( !G.isConnected() );
    BOOST_CHECK( !G.isTree() );
    BOOST_CHECK_EQUAL( G.addNode2( nbs.begin(), nbs.end() ), 3 );
    G.checkConsistency();
    BOOST_CHECK( !G.isConnected() );
    BOOST_CHECK( !G.isTree() );
    G.addEdge( 3, 3 );
    G.checkConsistency();
    BOOST_CHECK( G.isConnected() );
    BOOST_CHECK( G.isTree() );
    G.addEdge( 1, 3 );
    G.checkConsistency();
    BOOST_CHECK( G.isConnected() );
    BOOST_CHECK( !G.isTree() );
    BOOST_CHECK_EQUAL( G.nrNodes1(), 5 );
    BOOST_CHECK_EQUAL( G.nrNodes2(), 4 );
    BOOST_CHECK_EQUAL( G.nrEdges(), 9 );
    G.eraseEdge( 0, 3 );
    G.checkConsistency();
    BOOST_CHECK( G.isConnected() );
    BOOST_CHECK( G.isTree() );
    G.eraseEdge( 4, 2 );
    G.checkConsistency();
    BOOST_CHECK( !G.isConnected() );
    BOOST_CHECK( !G.isTree() );
    G.eraseNode2( 2 );
    G.checkConsistency();
    BOOST_CHECK( G.isConnected() );
    BOOST_CHECK( G.isTree() );
    G.eraseNode1( 0 );
    G.checkConsistency();
    BOOST_CHECK( !G.isConnected() );
    BOOST_CHECK( !G.isTree() );
    G.addEdge( 1, 0 );
    G.checkConsistency();
    BOOST_CHECK( G.isConnected() );
    BOOST_CHECK( G.isTree() );
    G.eraseNode1( 2 );
    G.checkConsistency();
    BOOST_CHECK( G.isConnected() );
    BOOST_CHECK( G.isTree() );
    G.eraseNode2( 2 );
    G.checkConsistency();
    BOOST_CHECK( !G.isConnected() );
    BOOST_CHECK( !G.isTree() );
    G.addEdge( 1, 1 );
    G.checkConsistency();
    BOOST_CHECK( G.isConnected() );
    BOOST_CHECK( G.isTree() );
    G.eraseNode2( 1 );
    G.checkConsistency();
    BOOST_CHECK( !G.isConnected() );
    BOOST_CHECK( !G.isTree() );
    G.eraseNode1( 1 );
    G.checkConsistency();
    BOOST_CHECK( !G.isConnected() );
    BOOST_CHECK( !G.isTree() );
    G.addEdge( 0, 0 );
    G.checkConsistency();
    BOOST_CHECK( G.isConnected() );
    BOOST_CHECK( G.isTree() );
    G.eraseNode2( 0 );
    G.checkConsistency();
    BOOST_CHECK( !G.isConnected() );
    BOOST_CHECK( !G.isTree() );
    G.eraseNode1( 0 );
    G.checkConsistency();
    BOOST_CHECK( G.isConnected() );
    BOOST_CHECK( G.isTree() );
    G.eraseNode1( 0 );
    G.checkConsistency();
    BOOST_CHECK( G.isConnected() );
    BOOST_CHECK( G.isTree() );
    BOOST_CHECK_EQUAL( G.nrNodes1(), 0 );
    BOOST_CHECK_EQUAL( G.nrNodes2(), 0 );
    BOOST_CHECK_EQUAL( G.nrEdges(), 0 );
}


BOOST_AUTO_TEST_CASE( RandomAddEraseTest ) {
    // check adding and erasing nodes and edges randomly
    BipartiteGraph G;
    for( size_t maxN1 = 2; maxN1 < 10; maxN1++ )
        for( size_t maxN2 = 2; maxN2 < 10; maxN2++ )
            for( size_t repeats = 0; repeats < 100000; repeats++ ) {
                size_t action = rnd( 5 );
                size_t N1 = G.nrNodes1();
                size_t N2 = G.nrNodes2();
                size_t M = G.nrEdges();
                size_t maxM = N1 * N2;
                if( action == 0 ) {
                    // add node
                    if( rnd( 2 ) == 0 ) {
                        // add node of type 1
                        if( N1 < maxN1 )
                            G.addNode1();
                    } else {
                        // add node of type 2
                        if( N2 < maxN2 )
                            G.addNode2();
                    }
                } else if( action == 1 ) {
                    // erase node
                    if( rnd( 2 ) == 0 ) {
                        // erase node of type 1
                        if( N1 > 0 )
                            G.eraseNode1( rnd( N1 ) );
                    } else {
                        // erase node of type 2
                        if( N2 > 0 )
                            G.eraseNode2( rnd( N2 ) );
                    }
                } else if( action == 2 || action == 3 ) {
                    // add edge
                    if( N1 >= 1 && N2 >= 1 && M < maxM ) {
                        size_t n1 = 0;
                        size_t n2 = 0;
                        if( rnd( 2 ) == 0 ) {
                            do {
                                n1 = rnd( N1 );
                            } while( G.nb1(n1).size() >= N2 );
                            do {
                                n2 = rnd( N2 );
                            } while( G.hasEdge( n1, n2 ) );
                        } else {
                            do {
                                n2 = rnd( N2 );
                            } while( G.nb2(n2).size() >= N1 );
                            do {
                                n1 = rnd( N1 );
                            } while( G.hasEdge( n1, n2 ) );
                        }
                        G.addEdge( n1, n2 );
                    }
                } else if( action == 4 ) {
                    // erase edge
                    if( M > 0 ) {
                        size_t n1 = 0;
                        size_t n2 = 0;
                        if( rnd( 2 ) == 0 ) {
                            do {
                                n1 = rnd( N1 );
                            } while( G.nb1(n1).size() == 0 );
                            do {
                                n2 = rnd( N2 );
                            } while( !G.hasEdge( n1, n2 ) );
                        } else {
                            do {
                                n2 = rnd( N2 );
                            } while( G.nb2(n2).size() == 0 );
                            do {
                                n1 = rnd( N1 );
                            } while( !G.hasEdge( n1, n2 ) );
                        }
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
    edges.push_back( Edge( 1, 1 ) );
    edges.push_back( Edge( 0, 0 ) );
    BipartiteGraph G( 3, 2, edges.begin(), edges.end() );
    G.checkConsistency();
    SmallSet<size_t> v;
    SmallSet<size_t> v0( 0 );
    SmallSet<size_t> v1( 1 );
    SmallSet<size_t> v01( 0, 1 );
    BOOST_CHECK_EQUAL( G.delta1( 0, true ), v01 );
    BOOST_CHECK_EQUAL( G.delta1( 1, true ), v01 );
    BOOST_CHECK_EQUAL( G.delta1( 2, true ), v );
    BOOST_CHECK_EQUAL( G.delta2( 0, true ), v01 );
    BOOST_CHECK_EQUAL( G.delta2( 1, true ), v01 );
    BOOST_CHECK_EQUAL( G.delta1( 0, false ), v1 );
    BOOST_CHECK_EQUAL( G.delta1( 1, false ), v0 );
    BOOST_CHECK_EQUAL( G.delta1( 2, false ), v );
    BOOST_CHECK_EQUAL( G.delta2( 0, false ), v1 );
    BOOST_CHECK_EQUAL( G.delta2( 1, false ), v0 );
    BOOST_CHECK( G.hasEdge( 0, 0 ) );
    BOOST_CHECK( G.hasEdge( 0, 1 ) );
    BOOST_CHECK( G.hasEdge( 1, 1 ) );
    BOOST_CHECK( !G.hasEdge( 1, 0 ) );
    BOOST_CHECK( !G.hasEdge( 2, 0 ) );
    BOOST_CHECK( !G.hasEdge( 2, 1 ) );
    BOOST_CHECK_EQUAL( G.findNb1( 0, 0 ), 1 );
    BOOST_CHECK_EQUAL( G.findNb1( 0, 1 ), 0 );
    BOOST_CHECK_EQUAL( G.findNb1( 1, 1 ), 0 );
    BOOST_CHECK_EQUAL( G.findNb2( 0, 0 ), 0 );
    BOOST_CHECK_EQUAL( G.findNb2( 0, 1 ), 0 );
    BOOST_CHECK_EQUAL( G.findNb2( 1, 1 ), 1 );
}


BOOST_AUTO_TEST_CASE( StreamTest ) {
    // check printDot
    std::vector<Edge> edges;
    edges.push_back( Edge(0, 0) );
    edges.push_back( Edge(0, 1) );
    edges.push_back( Edge(1, 1) );
    edges.push_back( Edge(1, 2) );
    BipartiteGraph G( 2, 3, edges.begin(), edges.end() );

    std::stringstream ss;
    std::string s;

    G.printDot( ss );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "graph BipartiteGraph {" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "node[shape=circle,width=0.4,fixedsize=true];" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "node[shape=box,width=0.3,height=0.3,fixedsize=true];" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\ty0;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\ty1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\ty2;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0 -- y0;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0 -- y1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx1 -- y1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx1 -- y2;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "}" );

    ss << G;
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "graph BipartiteGraph {" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "node[shape=circle,width=0.4,fixedsize=true];" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "node[shape=box,width=0.3,height=0.3,fixedsize=true];" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\ty0;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\ty1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\ty2;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0 -- y0;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0 -- y1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx1 -- y1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx1 -- y2;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "}" );
}
