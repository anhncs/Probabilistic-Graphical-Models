/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/graph.h>
#include <vector>
#include <strstream>


using namespace dai;


#define BOOST_TEST_MODULE GraphALTest


#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( ConstructorsTest ) {
    // check constructors
    GraphAL G0;
    BOOST_CHECK_EQUAL( G0.nrNodes(), 0 );
    BOOST_CHECK_EQUAL( G0.nrEdges(), 0 );
    BOOST_CHECK_EQUAL( G0.isConnected(), true );
    BOOST_CHECK_EQUAL( G0.isTree(), true );
    G0.checkConsistency();

    GraphAL G2( 2 );
    BOOST_CHECK_EQUAL( G2.nrNodes(), 2 );
    BOOST_CHECK_EQUAL( G2.nrEdges(), 0 );
    BOOST_CHECK_EQUAL( G2.isConnected(), false );
    BOOST_CHECK_EQUAL( G2.isTree(), false );
    G2.checkConsistency();
    BOOST_CHECK( !(G2 == G0) );
    
    std::vector<Edge> edges;
    edges.push_back( Edge( 0, 1 ) );
    edges.push_back( Edge( 1, 2 ) );
    edges.push_back( Edge( 2, 1 ) );
    edges.push_back( Edge( 1, 2 ) );
    GraphAL G3( 3, edges.begin(), edges.end() );
    BOOST_CHECK_EQUAL( G3.nrNodes(), 3 );
    BOOST_CHECK_EQUAL( G3.nrEdges(), 2 );
    BOOST_CHECK_EQUAL( G3.isConnected(), true );
    BOOST_CHECK_EQUAL( G3.isTree(), true );
    G3.checkConsistency();
    BOOST_CHECK( !(G3 == G0) );
    BOOST_CHECK( !(G3 == G2) );

    GraphAL G4( G3 );
    BOOST_CHECK( !(G4 == G0) );
    BOOST_CHECK( !(G4 == G2) );
    BOOST_CHECK( G4 == G3 );

    GraphAL G5 = G3;
    BOOST_CHECK( !(G5 == G0) );
    BOOST_CHECK( !(G5 == G2) );
    BOOST_CHECK( G5 == G3 );
}


BOOST_AUTO_TEST_CASE( NeighborTest ) {
    // check nb() accessor / mutator
    std::vector<Edge> edges;
    edges.push_back( Edge( 0, 1 ) );
    edges.push_back( Edge( 1, 2 ) );
    GraphAL G( 3, edges.begin(), edges.end() );
    BOOST_CHECK_EQUAL( G.nb(0).size(), 1 );
    BOOST_CHECK_EQUAL( G.nb(1).size(), 2 );
    BOOST_CHECK_EQUAL( G.nb(2).size(), 1 );
    BOOST_CHECK_EQUAL( G.nb(0,0).iter, 0 );
    BOOST_CHECK_EQUAL( G.nb(0,0).node, 1 );
    BOOST_CHECK_EQUAL( G.nb(0,0).dual, 0 );
    BOOST_CHECK_EQUAL( G.nb(1,0).iter, 0 );
    BOOST_CHECK_EQUAL( G.nb(1,0).node, 0 );
    BOOST_CHECK_EQUAL( G.nb(1,0).dual, 0 );
    BOOST_CHECK_EQUAL( G.nb(1,1).iter, 1 );
    BOOST_CHECK_EQUAL( G.nb(1,1).node, 2 );
    BOOST_CHECK_EQUAL( G.nb(1,1).dual, 0 );
    BOOST_CHECK_EQUAL( G.nb(2,0).iter, 0 );
    BOOST_CHECK_EQUAL( G.nb(2,0).node, 1 );
    BOOST_CHECK_EQUAL( G.nb(2,0).dual, 1 );
    BOOST_CHECK( G.nbSet(0) == SmallSet<size_t>( 1 ) );
    BOOST_CHECK( G.nbSet(1) == SmallSet<size_t>( 0, 2 ) );
    BOOST_CHECK( G.nbSet(2) == SmallSet<size_t>( 1 ) );
}


BOOST_AUTO_TEST_CASE( AddEraseTest ) {
    // check addition and erasure of nodes and edges
    std::vector<Edge> edges;
    edges.push_back( Edge( 0, 1 ) );
    edges.push_back( Edge( 1, 2 ) );
    edges.push_back( Edge( 1, 0 ) );
    GraphAL G( 2 );
    G.construct( 3, edges.begin(), edges.end() );
    G.checkConsistency();
    BOOST_CHECK_EQUAL( G.nrNodes(), 3 );
    BOOST_CHECK_EQUAL( G.nrEdges(), 2 );
    BOOST_CHECK_EQUAL( G.addNode(), 3 );
    G.checkConsistency();
    std::vector<size_t> nbs;
    nbs.push_back( 3 );
    BOOST_CHECK_EQUAL( G.addNode( nbs.begin(), nbs.end() ), 4 );
    BOOST_CHECK_EQUAL( G.addNode(), 5 );
    G.checkConsistency();
    G.addEdge( 0, 4 );
    G.checkConsistency();
    G.addEdge( 0, 5 );
    BOOST_CHECK( G.isTree() );
    G.checkConsistency();
    BOOST_CHECK_EQUAL( G.nrNodes(), 6 );
    BOOST_CHECK_EQUAL( G.nrEdges(), 5 );
    G.addEdge( 2, 3 );
    BOOST_CHECK( !G.isTree() );

    G.addEdge( 5, 3 );
    G.eraseNode( 0 );
    G.checkConsistency();
    BOOST_CHECK( G.isTree() );
    G.eraseEdge( 0, 1 );
    G.checkConsistency();
    BOOST_CHECK( !G.isTree() );
    BOOST_CHECK( !G.isConnected() );
    G.eraseNode( 0 );
    G.checkConsistency();
    BOOST_CHECK( G.isTree() );
    G.addEdge( 3, 2 );
    G.checkConsistency();
    BOOST_CHECK( !G.isTree() );
    G.eraseNode( 1 );
    G.checkConsistency();
    BOOST_CHECK( !G.isTree() );
    BOOST_CHECK( !G.isConnected() );
    G.eraseNode( 2 );
    G.checkConsistency();
    BOOST_CHECK( !G.isTree() );
    BOOST_CHECK( !G.isConnected() );
    G.addEdge( 1, 0 );
    G.checkConsistency();
    BOOST_CHECK( G.isTree() );
    BOOST_CHECK( G.isConnected() );
    G.eraseNode( 1 );
    G.checkConsistency();
    BOOST_CHECK( G.isTree() );
    BOOST_CHECK( G.isConnected() );
    G.eraseNode( 0 );
    BOOST_CHECK( G.isTree() );
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
    GraphAL G;
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
                    do {
                        n1 = rnd( N );
                    } while( G.nb(n1).size() >= N - 1 );
                    size_t n2 = 0;
                    do {
                        n2 = rnd( N );
                    } while( G.hasEdge( n1, n2 ) );
                    G.addEdge( n1, n2 );
                }
            } else if( action == 4 ) {
                // erase edge
                if( M > 0 ) {
                    size_t n1 = 0;
                    do {
                        n1 = rnd( N );
                    } while( G.nb(n1).size() == 0 );
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


BOOST_AUTO_TEST_CASE( QueriesAndCreationTest ) {
    // check queries and createGraph* functions

    // createGraphFull
    for( size_t N = 0; N < 20; N++ ) {
        GraphAL G = createGraphFull( N );
        BOOST_CHECK_EQUAL( G.nrNodes(), N );
        BOOST_CHECK_EQUAL( G.nrEdges(), N > 0 ? N * (N-1) / 2 : 0 );
        BOOST_CHECK( G.isConnected() );
        BOOST_CHECK_EQUAL( G.isTree(), N < 3 );
        for( size_t n1 = 0; n1 < G.nrNodes(); n1++ ) {
            bforeach( const Neighbor &n2, G.nb(n1) ) {
                BOOST_CHECK( G.hasEdge( n1, n2 ) );
                BOOST_CHECK( G.hasEdge( n2, n1 ) );
            }
            for( size_t n2 = 0; n2 < G.nrNodes(); n2++ )
                if( G.hasEdge( n1, n2 ) ) {
                    BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ), n2 );
                    BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ), n1 );
                    BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ).iter, G.findNb( n1, n2 ) );
                    BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ).iter, G.findNb( n2, n1 ) );
                }
        }
        G.checkConsistency();
    }

    // createGraphGrid
    for( size_t N1 = 0; N1 < 10; N1++ )
        for( size_t N2 = 0; N2 < 10; N2++ ) {
            GraphAL G = createGraphGrid( N1, N2, false );
            BOOST_CHECK_EQUAL( G.nrNodes(), N1 * N2 );
            BOOST_CHECK_EQUAL( G.nrEdges(), (N1 > 0 && N2 > 0) ? 2 * (N1-1) * (N2-1) + (N1-1) + (N2-1) : 0 );
            BOOST_CHECK( G.isConnected() );
            BOOST_CHECK_EQUAL( G.isTree(), (N1 <= 1) || (N2 <= 1) );
            for( size_t n1 = 0; n1 < G.nrNodes(); n1++ ) {
                bforeach( const Neighbor &n2, G.nb(n1) ) {
                    BOOST_CHECK( G.hasEdge( n1, n2 ) );
                    BOOST_CHECK( G.hasEdge( n2, n1 ) );
                }
                for( size_t n2 = 0; n2 < G.nrNodes(); n2++ )
                    if( G.hasEdge( n1, n2 ) ) {
                        BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ), n2 );
                        BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ), n1 );
                        BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ).iter, G.findNb( n1, n2 ) );
                        BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ).iter, G.findNb( n2, n1 ) );
                    }
            }
            G.checkConsistency();
            
            G = createGraphGrid( N1, N2, true );
            BOOST_CHECK_EQUAL( G.nrNodes(), N1 * N2 );
            if( N1 == 0 || N2 == 0 )
                BOOST_CHECK_EQUAL( G.nrEdges(), 0 );
            else
                BOOST_CHECK_EQUAL( G.nrEdges(), (N1 <= 2 ? (N1-1) : N1) * N2 + N1 * (N2 <= 2 ? (N2-1) : N2) );
            BOOST_CHECK( G.isConnected() );
            BOOST_CHECK_EQUAL( G.isTree(), (G.nrNodes() <= 2) );
            for( size_t n1 = 0; n1 < G.nrNodes(); n1++ ) {
                bforeach( const Neighbor &n2, G.nb(n1) ) {
                    BOOST_CHECK( G.hasEdge( n1, n2 ) );
                    BOOST_CHECK( G.hasEdge( n2, n1 ) );
                }
                for( size_t n2 = 0; n2 < G.nrNodes(); n2++ )
                    if( G.hasEdge( n1, n2 ) ) {
                        BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ), n2 );
                        BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ), n1 );
                        BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ).iter, G.findNb( n1, n2 ) );
                        BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ).iter, G.findNb( n2, n1 ) );
                    }
            }
            G.checkConsistency();
        }

    // createGraphGrid3D
    for( size_t N1 = 0; N1 < 8; N1++ )
        for( size_t N2 = 0; N2 < 8; N2++ )
            for( size_t N3 = 0; N3 < 8; N3++ ) {
                GraphAL G = createGraphGrid3D( N1, N2, N3, false );
                BOOST_CHECK_EQUAL( G.nrNodes(), N1 * N2 * N3 );
                BOOST_CHECK_EQUAL( G.nrEdges(), (N1 > 0 && N2 > 0 && N3 > 0) ? 3 * (N1-1) * (N2-1) * (N3-1) + 2 * (N1-1) * (N2-1) + 2 * (N1-1) * (N3-1) + 2 *  (N2-1) * (N3-1) + (N1-1) + (N2-1) + (N3-1) : 0 );
                BOOST_CHECK( G.isConnected() );
                BOOST_CHECK_EQUAL( G.isTree(), (G.nrNodes() == 0) || (N1 <= 1 && N2 <= 1) || (N1 <= 1 && N3 <= 1) || (N2 <= 1 && N3 <= 1) );
                for( size_t n1 = 0; n1 < G.nrNodes(); n1++ ) {
                    bforeach( const Neighbor &n2, G.nb(n1) ) {
                        BOOST_CHECK( G.hasEdge( n1, n2 ) );
                        BOOST_CHECK( G.hasEdge( n2, n1 ) );
                    }
                    for( size_t n2 = 0; n2 < G.nrNodes(); n2++ )
                        if( G.hasEdge( n1, n2 ) ) {
                            BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ), n2 );
                            BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ), n1 );
                            BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ).iter, G.findNb( n1, n2 ) );
                            BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ).iter, G.findNb( n2, n1 ) );
                        }
                }
                G.checkConsistency();
                
                G = createGraphGrid3D( N1, N2, N3, true );
                BOOST_CHECK_EQUAL( G.nrNodes(), N1 * N2 * N3 );
                if( N1 == 0 || N2 == 0 || N3 == 0 )
                    BOOST_CHECK_EQUAL( G.nrEdges(), 0 );
                else
                    BOOST_CHECK_EQUAL( G.nrEdges(), (N1 <= 2 ? (N1-1) : N1) * N2 * N3 + N1 * (N2 <= 2 ? (N2-1) : N2) * N3 + N1 * N2 * (N3 <= 2 ? (N3-1) : N3) );
                BOOST_CHECK( G.isConnected() );
                BOOST_CHECK_EQUAL( G.isTree(), (G.nrNodes() <= 2) );
                for( size_t n1 = 0; n1 < G.nrNodes(); n1++ ) {
                    bforeach( const Neighbor &n2, G.nb(n1) ) {
                        BOOST_CHECK( G.hasEdge( n1, n2 ) );
                        BOOST_CHECK( G.hasEdge( n2, n1 ) );
                    }
                    for( size_t n2 = 0; n2 < G.nrNodes(); n2++ )
                        if( G.hasEdge( n1, n2 ) ) {
                            BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ), n2 );
                            BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ), n1 );
                            BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ).iter, G.findNb( n1, n2 ) );
                            BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ).iter, G.findNb( n2, n1 ) );
                        }
                }
                G.checkConsistency();
            }

    // createGraphLoop
    for( size_t N = 0; N < 100; N++ ) {
        GraphAL G = createGraphLoop( N );
        BOOST_CHECK_EQUAL( G.nrNodes(), N );
        if( N == 0 )
            BOOST_CHECK_EQUAL( G.nrEdges(), 0 );
        else if( N <= 2 )
            BOOST_CHECK_EQUAL( G.nrEdges(), N-1 );
        else
            BOOST_CHECK_EQUAL( G.nrEdges(), N );
        BOOST_CHECK( G.isConnected() );
        BOOST_CHECK_EQUAL( G.isTree(), N <= 2 );
        for( size_t n1 = 0; n1 < G.nrNodes(); n1++ ) {
            bforeach( const Neighbor &n2, G.nb(n1) ) {
                BOOST_CHECK( G.hasEdge( n1, n2 ) );
                BOOST_CHECK( G.hasEdge( n2, n1 ) );
            }
            for( size_t n2 = 0; n2 < G.nrNodes(); n2++ )
                if( G.hasEdge( n1, n2 ) ) {
                    BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ), n2 );
                    BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ), n1 );
                    BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ).iter, G.findNb( n1, n2 ) );
                    BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ).iter, G.findNb( n2, n1 ) );
                }
        }
        G.checkConsistency();
    }

    // createGraphTree
    for( size_t N = 0; N < 100; N++ ) {
        GraphAL G = createGraphTree( N );
        BOOST_CHECK_EQUAL( G.nrNodes(), N );
        BOOST_CHECK_EQUAL( G.nrEdges(), N > 0 ? N - 1 : N );
        BOOST_CHECK( G.isConnected() );
        BOOST_CHECK( G.isTree() );
        for( size_t n1 = 0; n1 < G.nrNodes(); n1++ ) {
            bforeach( const Neighbor &n2, G.nb(n1) ) {
                BOOST_CHECK( G.hasEdge( n1, n2 ) );
                BOOST_CHECK( G.hasEdge( n2, n1 ) );
            }
            for( size_t n2 = 0; n2 < G.nrNodes(); n2++ )
                if( G.hasEdge( n1, n2 ) ) {
                    BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ), n2 );
                    BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ), n1 );
                    BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ).iter, G.findNb( n1, n2 ) );
                    BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ).iter, G.findNb( n2, n1 ) );
                }
        }
        G.checkConsistency();
    }

    // createGraphRegular
    for( size_t N = 0; N < 50; N++ ) {
        for( size_t d = 0; d < N && d <= 15; d++ ) {
            if( (N * d) % 2 == 0 ) {
                GraphAL G = createGraphRegular( N, d );
                BOOST_CHECK_EQUAL( G.nrNodes(), N );
                BOOST_CHECK_EQUAL( G.nrEdges(), d * N / 2 );
                for( size_t n1 = 0; n1 < G.nrNodes(); n1++ ) {
                    BOOST_CHECK_EQUAL( G.nb(n1).size(), d );
                    bforeach( const Neighbor &n2, G.nb(n1) ) {
                        BOOST_CHECK( G.hasEdge( n1, n2 ) );
                        BOOST_CHECK( G.hasEdge( n2, n1 ) );
                    }
                    for( size_t n2 = 0; n2 < G.nrNodes(); n2++ )
                        if( G.hasEdge( n1, n2 ) ) {
                            BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ), n2 );
                            BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ), n1 );
                            BOOST_CHECK_EQUAL( G.nb( n1, G.findNb( n1, n2 ) ).iter, G.findNb( n1, n2 ) );
                            BOOST_CHECK_EQUAL( G.nb( n2, G.findNb( n2, n1 ) ).iter, G.findNb( n2, n1 ) );
                        }
                }
                G.checkConsistency();
            }
        }
    }
}


BOOST_AUTO_TEST_CASE( StreamTest ) {
    // check printDot
    GraphAL G( 4 );
    G.addEdge( 0, 1 );
    G.addEdge( 0, 2 );
    G.addEdge( 1, 3 );
    G.addEdge( 2, 3 );
    G.addEdge( 2, 2 );
    G.addEdge( 3, 2 );

    std::stringstream ss;
    std::string s;

    G.printDot( ss );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "graph GraphAL {" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "node[shape=circle,width=0.4,fixedsize=true];" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx2;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx3;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0 -- x1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0 -- x2;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx1 -- x3;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx2 -- x3;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "}" );

    ss << G;
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "graph GraphAL {" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "node[shape=circle,width=0.4,fixedsize=true];" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx2;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx3;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0 -- x1;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx0 -- x2;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx1 -- x3;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tx2 -- x3;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "}" );
}
