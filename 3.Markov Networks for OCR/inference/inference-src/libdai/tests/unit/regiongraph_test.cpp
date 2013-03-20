/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/regiongraph.h>
#include <vector>
#include <strstream>


using namespace dai;


const double tol = 1e-8;


#define BOOST_TEST_MODULE RegionGraphTest


#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>


BOOST_AUTO_TEST_CASE( ConstructorsTest ) {
    Var v0( 0, 2 );
    Var v1( 1, 2 );
    Var v2( 2, 2 );
    VarSet v01( v0, v1 );
    VarSet v02( v0, v2 );
    VarSet v12( v1, v2 );
    RegionGraph G;
    BOOST_CHECK_EQUAL( G.vars(), std::vector<Var>() );
    BOOST_CHECK_EQUAL( G.factors(), std::vector<Factor>() );
    BOOST_CHECK( G.bipGraph() == BipartiteGraph() );
    BOOST_CHECK( G.DAG() == BipartiteGraph() );
    BOOST_CHECK_EQUAL( G.nrORs(), 0 );
    BOOST_CHECK_EQUAL( G.nrIRs(), 0 );

    std::vector<Factor> facs;
    facs.push_back( Factor( v01 ) );
    facs.push_back( Factor( v02 ) );
    facs.push_back( Factor( v12 ) );
    facs.push_back( Factor( v1 ) );
    std::vector<Var> vars;
    vars.push_back( v0 );
    vars.push_back( v1 );
    vars.push_back( v2 );
    BipartiteGraph dag( 3, 3 );
    dag.addEdge( 0, 0 );
    dag.addEdge( 0, 1 );
    dag.addEdge( 1, 0 );
    dag.addEdge( 1, 2 );
    dag.addEdge( 2, 1 );
    dag.addEdge( 2, 2 );
    BipartiteGraph bipgraph( dag );
    bipgraph.addNode2();
    bipgraph.addEdge( 1, 3 );

    FactorGraph fg(facs);
    RegionGraph G1( fg, fg.maximalFactorDomains() );
    BOOST_CHECK_EQUAL( G1.vars(), vars );
    BOOST_CHECK_EQUAL( G1.factors(), facs );
    BOOST_CHECK( G1.bipGraph() == bipgraph );
    BOOST_CHECK( G1.DAG() == dag );
    BOOST_CHECK_EQUAL( G1.nrORs(), 3 );
    BOOST_CHECK_EQUAL( G1.OR(0).c(), 1 );
    BOOST_CHECK_EQUAL( G1.OR(0), facs[0] * facs[3] );
    BOOST_CHECK_EQUAL( G1.OR(1).c(), 1 );
    BOOST_CHECK_EQUAL( G1.OR(1), facs[1] );
    BOOST_CHECK_EQUAL( G1.OR(2).c(), 1 );
    BOOST_CHECK_EQUAL( G1.OR(2), facs[2] );
    BOOST_CHECK_EQUAL( G1.nrIRs(), 3 );
    BOOST_CHECK_EQUAL( G1.IR(0).c(), -1 );
    BOOST_CHECK_EQUAL( G1.IR(0), v0 );
    BOOST_CHECK_EQUAL( G1.IR(1).c(), -1 );
    BOOST_CHECK_EQUAL( G1.IR(1), v1 );
    BOOST_CHECK_EQUAL( G1.IR(2).c(), -1 );
    BOOST_CHECK_EQUAL( G1.IR(2), v2 );
    BOOST_CHECK_EQUAL( G1.fac2OR(0), 0 );
    BOOST_CHECK_EQUAL( G1.fac2OR(1), 1 );
    BOOST_CHECK_EQUAL( G1.fac2OR(2), 2 );
    BOOST_CHECK_EQUAL( G1.fac2OR(3), 0 );
    BOOST_CHECK( G1.checkCountingNumbers() );

    std::vector<VarSet> ors;
    std::vector<Region> irs;
    ors.push_back( v01 | v2 );
    irs.push_back( Region( v01, 0.0 ) );
    irs.push_back( Region( v2, 0.0 ) );
    typedef std::pair<size_t,size_t> edge;
    std::vector<edge> edges;
    edges.push_back( edge( 0, 0 ) );
    edges.push_back( edge( 0, 1 ) );
    BipartiteGraph dag2( 1, 2, edges.begin(), edges.end() );
    RegionGraph G2( fg, ors, irs, edges );
    BOOST_CHECK_EQUAL( G2.vars(), vars );
    BOOST_CHECK_EQUAL( G2.factors(), facs );
    BOOST_CHECK( G2.bipGraph() == bipgraph );
    BOOST_CHECK( G2.DAG() == dag2 );
    BOOST_CHECK_EQUAL( G2.nrORs(), 1 );
    BOOST_CHECK_EQUAL( G2.OR(0).c(), 1 );
    BOOST_CHECK_EQUAL( G2.OR(0), facs[0] * facs[1] * facs[2] * facs[3] );
    BOOST_CHECK_EQUAL( G2.nrIRs(), 2 );
    BOOST_CHECK_EQUAL( G2.IR(0), irs[0] );
    BOOST_CHECK_EQUAL( G2.IR(1), irs[1] );
    BOOST_CHECK_EQUAL( G2.fac2OR(0), 0 );
    BOOST_CHECK_EQUAL( G2.fac2OR(1), 0 );
    BOOST_CHECK_EQUAL( G2.fac2OR(2), 0 );
    BOOST_CHECK_EQUAL( G2.fac2OR(3), 0 );
    BOOST_CHECK( G2.checkCountingNumbers() );

    RegionGraph *G3 = G1.clone();
    BOOST_CHECK_EQUAL( G3->vars(), G1.vars() );
    BOOST_CHECK_EQUAL( G3->factors(), G1.factors() );
    BOOST_CHECK( G3->bipGraph() == bipgraph );
    BOOST_CHECK( G3->DAG() == dag );
    BOOST_CHECK_EQUAL( G3->nrORs(), G1.nrORs() );
    BOOST_CHECK_EQUAL( G3->OR(0), G1.OR(0) );
    BOOST_CHECK_EQUAL( G3->OR(1), G1.OR(1) );
    BOOST_CHECK_EQUAL( G3->OR(2), G1.OR(2) );
    BOOST_CHECK_EQUAL( G3->nrIRs(), G1.nrIRs() );
    BOOST_CHECK_EQUAL( G3->IR(0), G1.IR(0) );
    BOOST_CHECK_EQUAL( G3->IR(1), G1.IR(1) );
    BOOST_CHECK_EQUAL( G3->IR(2), G1.IR(2) );
    BOOST_CHECK_EQUAL( G3->fac2OR(0), G1.fac2OR(0) );
    BOOST_CHECK_EQUAL( G3->fac2OR(1), G1.fac2OR(1) );
    BOOST_CHECK_EQUAL( G3->fac2OR(2), G1.fac2OR(2) );
    BOOST_CHECK_EQUAL( G3->fac2OR(3), G1.fac2OR(3) );
    BOOST_CHECK( G3->checkCountingNumbers() );
    delete G3;

    RegionGraph G4 = G1;
    BOOST_CHECK_EQUAL( G4.vars(), G1.vars() );
    BOOST_CHECK_EQUAL( G4.factors(), G1.factors() );
    BOOST_CHECK( G4.bipGraph() == bipgraph );
    BOOST_CHECK( G4.DAG() == dag );
    BOOST_CHECK_EQUAL( G4.nrORs(), G1.nrORs() );
    BOOST_CHECK_EQUAL( G4.OR(0), G1.OR(0) );
    BOOST_CHECK_EQUAL( G4.OR(1), G1.OR(1) );
    BOOST_CHECK_EQUAL( G4.OR(2), G1.OR(2) );
    BOOST_CHECK_EQUAL( G4.nrIRs(), G1.nrIRs() );
    BOOST_CHECK_EQUAL( G4.IR(0), G1.IR(0) );
    BOOST_CHECK_EQUAL( G4.IR(1), G1.IR(1) );
    BOOST_CHECK_EQUAL( G4.IR(2), G1.IR(2) );
    BOOST_CHECK_EQUAL( G4.fac2OR(0), G1.fac2OR(0) );
    BOOST_CHECK_EQUAL( G4.fac2OR(1), G1.fac2OR(1) );
    BOOST_CHECK_EQUAL( G4.fac2OR(2), G1.fac2OR(2) );
    BOOST_CHECK_EQUAL( G4.fac2OR(3), G1.fac2OR(3) );
    BOOST_CHECK( G4.checkCountingNumbers() );

    RegionGraph G5( G1 );
    BOOST_CHECK_EQUAL( G5.vars(), G1.vars() );
    BOOST_CHECK_EQUAL( G5.factors(), G1.factors() );
    BOOST_CHECK( G5.bipGraph() == bipgraph );
    BOOST_CHECK( G5.DAG() == dag );
    BOOST_CHECK_EQUAL( G5.nrORs(), G1.nrORs() );
    BOOST_CHECK_EQUAL( G5.OR(0), G1.OR(0) );
    BOOST_CHECK_EQUAL( G5.OR(1), G1.OR(1) );
    BOOST_CHECK_EQUAL( G5.OR(2), G1.OR(2) );
    BOOST_CHECK_EQUAL( G5.nrIRs(), G1.nrIRs() );
    BOOST_CHECK_EQUAL( G5.IR(0), G1.IR(0) );
    BOOST_CHECK_EQUAL( G5.IR(1), G1.IR(1) );
    BOOST_CHECK_EQUAL( G5.IR(2), G1.IR(2) );
    BOOST_CHECK_EQUAL( G5.fac2OR(0), G1.fac2OR(0) );
    BOOST_CHECK_EQUAL( G5.fac2OR(1), G1.fac2OR(1) );
    BOOST_CHECK_EQUAL( G5.fac2OR(2), G1.fac2OR(2) );
    BOOST_CHECK_EQUAL( G5.fac2OR(3), G1.fac2OR(3) );
    BOOST_CHECK( G5.checkCountingNumbers() );
}


BOOST_AUTO_TEST_CASE( AccMutTest ) {
    Var v0( 0, 2 );
    Var v1( 1, 2 );
    Var v2( 2, 2 );
    VarSet v01( v0, v1 );
    VarSet v02( v0, v2 );
    VarSet v12( v1, v2 );
    std::vector<Factor> facs;
    facs.push_back( Factor( v01 ) );
    facs.push_back( Factor( v02 ) );
    facs.push_back( Factor( v12 ) );
    facs.push_back( Factor( v1 ) );
    std::vector<Var> vars;
    vars.push_back( v0 );
    vars.push_back( v1 );
    vars.push_back( v2 );
    BipartiteGraph dag( 3, 3 );
    dag.addEdge( 0, 0 );
    dag.addEdge( 0, 1 );
    dag.addEdge( 1, 0 );
    dag.addEdge( 1, 2 );
    dag.addEdge( 2, 1 );
    dag.addEdge( 2, 2 );
    BipartiteGraph bipgraph( dag );
    bipgraph.addNode2();
    bipgraph.addEdge( 1, 3 );

    FactorGraph fg( facs );
    RegionGraph G( fg, fg.maximalFactorDomains() );
    BOOST_CHECK_EQUAL( G.var(0), v0 );
    BOOST_CHECK_EQUAL( G.var(1), v1 );
    BOOST_CHECK_EQUAL( G.var(2), v2 );
    BOOST_CHECK_EQUAL( G.vars(), vars );
    BOOST_CHECK_EQUAL( G.factor(0), facs[0] );
    BOOST_CHECK_EQUAL( G.factor(1), facs[1] );
    BOOST_CHECK_EQUAL( G.factor(2), facs[2] );
    BOOST_CHECK_EQUAL( G.factor(3), facs[3] );
    BOOST_CHECK_EQUAL( G.factors(), facs );
    BOOST_CHECK( G.bipGraph() == bipgraph );
    BOOST_CHECK_EQUAL( G.nbV(0).size(), 2 );
    BOOST_CHECK_EQUAL( G.nbV(0,0), 0 );
    BOOST_CHECK_EQUAL( G.nbV(0,1), 1 );
    BOOST_CHECK_EQUAL( G.nbV(1).size(), 3 );
    BOOST_CHECK_EQUAL( G.nbV(1,0), 0 );
    BOOST_CHECK_EQUAL( G.nbV(1,1), 2 );
    BOOST_CHECK_EQUAL( G.nbV(1,2), 3 );
    BOOST_CHECK_EQUAL( G.nbV(0).size(), 2 );
    BOOST_CHECK_EQUAL( G.nbV(2,0), 1 );
    BOOST_CHECK_EQUAL( G.nbV(2,1), 2 );
    BOOST_CHECK_EQUAL( G.nbF(0).size(), 2 );
    BOOST_CHECK_EQUAL( G.nbF(0,0), 0 );
    BOOST_CHECK_EQUAL( G.nbF(0,1), 1 );
    BOOST_CHECK_EQUAL( G.nbF(1).size(), 2 );
    BOOST_CHECK_EQUAL( G.nbF(1,0), 0 );
    BOOST_CHECK_EQUAL( G.nbF(1,1), 2 );
    BOOST_CHECK_EQUAL( G.nbF(2).size(), 2 );
    BOOST_CHECK_EQUAL( G.nbF(2,0), 1 );
    BOOST_CHECK_EQUAL( G.nbF(2,1), 2 );
    BOOST_CHECK_EQUAL( G.nbF(3).size(), 1 );
    BOOST_CHECK_EQUAL( G.nbF(3,0), 1 );
    BOOST_CHECK( G.DAG() == dag );
    BOOST_CHECK_EQUAL( G.nrORs(), 3 );
    BOOST_CHECK_EQUAL( G.OR(0).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(0).vars(), v01 );
    BOOST_CHECK_EQUAL( G.OR(0), facs[0] * facs[3] );
    BOOST_CHECK_EQUAL( G.OR(1).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(1).vars(), v02 );
    BOOST_CHECK_EQUAL( G.OR(1), facs[1] );
    BOOST_CHECK_EQUAL( G.OR(2).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(2).vars(), v12 );
    BOOST_CHECK_EQUAL( G.OR(2), facs[2] );
    BOOST_CHECK_EQUAL( G.nrIRs(), 3 );
    BOOST_CHECK_EQUAL( G.IR(0).c(), -1 );
    BOOST_CHECK_EQUAL( G.IR(0), v0 );
    BOOST_CHECK_EQUAL( G.IR(1).c(), -1 );
    BOOST_CHECK_EQUAL( G.IR(1), v1 );
    BOOST_CHECK_EQUAL( G.IR(2).c(), -1 );
    BOOST_CHECK_EQUAL( G.IR(2), v2 );
    BOOST_CHECK_EQUAL( G.fac2OR(0), 0 );
    BOOST_CHECK_EQUAL( G.fac2OR(1), 1 );
    BOOST_CHECK_EQUAL( G.fac2OR(2), 2 );
    BOOST_CHECK_EQUAL( G.fac2OR(3), 0 );
    BOOST_CHECK_EQUAL( G.nbOR(0).size(), 2 );
    BOOST_CHECK_EQUAL( G.nbOR(0)[0], 0 );
    BOOST_CHECK_EQUAL( G.nbOR(0)[1], 1 );
    BOOST_CHECK_EQUAL( G.nbOR(1).size(), 2 );
    BOOST_CHECK_EQUAL( G.nbOR(1)[0], 0 );
    BOOST_CHECK_EQUAL( G.nbOR(1)[1], 2 );
    BOOST_CHECK_EQUAL( G.nbOR(2).size(), 2 );
    BOOST_CHECK_EQUAL( G.nbOR(2)[0], 1 );
    BOOST_CHECK_EQUAL( G.nbOR(2)[1], 2 );
    BOOST_CHECK_EQUAL( G.nbIR(0).size(), 2 );
    BOOST_CHECK_EQUAL( G.nbIR(0)[0], 0 );
    BOOST_CHECK_EQUAL( G.nbIR(0)[1], 1 );
    BOOST_CHECK_EQUAL( G.nbIR(1).size(), 2 );
    BOOST_CHECK_EQUAL( G.nbIR(1)[0], 0 );
    BOOST_CHECK_EQUAL( G.nbIR(1)[1], 2 );
    BOOST_CHECK_EQUAL( G.nbIR(2).size(), 2 );
    BOOST_CHECK_EQUAL( G.nbIR(2)[0], 1 );
    BOOST_CHECK_EQUAL( G.nbIR(2)[1], 2 );
}


BOOST_AUTO_TEST_CASE( QueriesTest ) {
    Var v0( 0, 2 );
    Var v1( 1, 2 );
    Var v2( 2, 2 );
    VarSet v01( v0, v1 );
    VarSet v02( v0, v2 );
    VarSet v12( v1, v2 );
    VarSet v012 = v01 | v2;

    RegionGraph G0;
    BOOST_CHECK_EQUAL( G0.nrVars(), 0 );
    BOOST_CHECK_EQUAL( G0.nrFactors(), 0 );
    BOOST_CHECK_EQUAL( G0.nrEdges(), 0 );
    BOOST_CHECK_THROW( G0.findVar( v0 ), Exception );
    BOOST_CHECK_THROW( G0.findVars( v01 ), Exception );
    BOOST_CHECK_THROW( G0.findFactor( v01 ), Exception );
#ifdef DAI_DBEUG
    BOOST_CHECK_THROW( G0.delta( 0 ), Exception );
    BOOST_CHECK_THROW( G0.Delta( 0 ), Exception );
    BOOST_CHECK_THROW( G0.delta( v0 ), Exception );
    BOOST_CHECK_THROW( G0.Delta( v0 ), Exception );
    BOOST_CHECK_THROW( G0.fac2OR( 0 ), Exception );
#endif
    BOOST_CHECK( G0.isConnected() );
    BOOST_CHECK( G0.isTree() );
    BOOST_CHECK( G0.isBinary() );
    BOOST_CHECK( G0.isPairwise() );
    BOOST_CHECK( G0.MarkovGraph() == GraphAL() );
    BOOST_CHECK( G0.bipGraph() == BipartiteGraph() );
    BOOST_CHECK_EQUAL( G0.maximalFactorDomains().size(), 1 );
    BOOST_CHECK( G0.DAG() == BipartiteGraph() );
    BOOST_CHECK( G0.checkCountingNumbers() );
    BOOST_CHECK_EQUAL( G0.nrORs(), 0 );
    BOOST_CHECK_EQUAL( G0.nrIRs(), 0 );

    std::vector<Factor> facs;
    facs.push_back( Factor( v01 ) );
    facs.push_back( Factor( v12 ) );
    facs.push_back( Factor( v1 ) );
    std::vector<Var> vars;
    vars.push_back( v0 );
    vars.push_back( v1 );
    vars.push_back( v2 );
    GraphAL H(3);
    H.addEdge( 0, 1 );
    H.addEdge( 1, 2 );
    BipartiteGraph K(3, 3);
    K.addEdge( 0, 0 );
    K.addEdge( 1, 0 );
    K.addEdge( 1, 1 );
    K.addEdge( 2, 1 );
    K.addEdge( 1, 2 );
    BipartiteGraph dag(2, 1);
    dag.addEdge( 0, 0 );
    dag.addEdge( 1, 0 );

    FactorGraph fg( facs );
    RegionGraph G1( fg, fg.maximalFactorDomains() );
    BOOST_CHECK_EQUAL( G1.nrVars(), 3 );
    BOOST_CHECK_EQUAL( G1.nrFactors(), 3 );
    BOOST_CHECK_EQUAL( G1.nrEdges(), 5 );
    BOOST_CHECK_EQUAL( G1.findVar( v0 ), 0 );
    BOOST_CHECK_EQUAL( G1.findVar( v1 ), 1 );
    BOOST_CHECK_EQUAL( G1.findVar( v2 ), 2 );
    BOOST_CHECK_EQUAL( G1.findVars( v01 ), SmallSet<size_t>( 0, 1 ) );
    BOOST_CHECK_EQUAL( G1.findVars( v02 ), SmallSet<size_t>( 0, 2 ) );
    BOOST_CHECK_EQUAL( G1.findVars( v12 ), SmallSet<size_t>( 1, 2 ) );
    BOOST_CHECK_EQUAL( G1.findFactor( v01 ), 0 );
    BOOST_CHECK_EQUAL( G1.findFactor( v12 ), 1 );
    BOOST_CHECK_EQUAL( G1.findFactor( v1 ), 2 );
    BOOST_CHECK_THROW( G1.findFactor( v02 ), Exception );
    BOOST_CHECK_EQUAL( G1.delta( 0 ), v1 );
    BOOST_CHECK_EQUAL( G1.delta( 1 ), v02 );
    BOOST_CHECK_EQUAL( G1.delta( 2 ), v1 );
    BOOST_CHECK_EQUAL( G1.Delta( 0 ), v01 );
    BOOST_CHECK_EQUAL( G1.Delta( 1 ), v012 );
    BOOST_CHECK_EQUAL( G1.Delta( 2 ), v12 );
    BOOST_CHECK_EQUAL( G1.delta( v0 ), v1 );
    BOOST_CHECK_EQUAL( G1.delta( v1 ), v02 );
    BOOST_CHECK_EQUAL( G1.delta( v2 ), v1 );
    BOOST_CHECK_EQUAL( G1.delta( v01 ), v2 );
    BOOST_CHECK_EQUAL( G1.delta( v02 ), v1 );
    BOOST_CHECK_EQUAL( G1.delta( v12 ), v0 );
    BOOST_CHECK_EQUAL( G1.delta( v012 ), VarSet() );
    BOOST_CHECK_EQUAL( G1.Delta( v0 ), v01 );
    BOOST_CHECK_EQUAL( G1.Delta( v1 ), v012 );
    BOOST_CHECK_EQUAL( G1.Delta( v2 ), v12 );
    BOOST_CHECK_EQUAL( G1.Delta( v01 ), v012 );
    BOOST_CHECK_EQUAL( G1.Delta( v02 ), v012 );
    BOOST_CHECK_EQUAL( G1.Delta( v12 ), v012 );
    BOOST_CHECK_EQUAL( G1.Delta( v012 ), v012 );
    BOOST_CHECK( G1.isConnected() );
    BOOST_CHECK( G1.isTree() );
    BOOST_CHECK( G1.isBinary() );
    BOOST_CHECK( G1.isPairwise() );
    BOOST_CHECK( G1.MarkovGraph() == H );
    BOOST_CHECK( G1.bipGraph() == K );
    BOOST_CHECK_EQUAL( G1.maximalFactorDomains().size(), 2 );
    BOOST_CHECK_EQUAL( G1.maximalFactorDomains()[0], v01 );
    BOOST_CHECK_EQUAL( G1.maximalFactorDomains()[1], v12 );
    BOOST_CHECK( G1.DAG() == dag );
    BOOST_CHECK( G1.checkCountingNumbers() );
    BOOST_CHECK_EQUAL( G1.nrORs(), 2 );
    BOOST_CHECK_EQUAL( G1.OR(0).c(), 1 );
    BOOST_CHECK_EQUAL( G1.OR(0), facs[0] * facs[2] );
    BOOST_CHECK_EQUAL( G1.OR(1).c(), 1 );
    BOOST_CHECK_EQUAL( G1.OR(1), facs[1] );
    BOOST_CHECK_EQUAL( G1.nrIRs(), 1 );
    BOOST_CHECK_EQUAL( G1.IR(0).c(), -1 );
    BOOST_CHECK_EQUAL( G1.IR(0), v1 );
    BOOST_CHECK_EQUAL( G1.fac2OR(0), 0 );
    BOOST_CHECK_EQUAL( G1.fac2OR(1), 1 );
    BOOST_CHECK_EQUAL( G1.fac2OR(2), 0 );

    facs.push_back( Factor( v02 ) );
    H.addEdge( 0, 2 );
    K.addNode2();
    K.addEdge( 0, 3 );
    K.addEdge( 2, 3 );
    dag = BipartiteGraph( 3, 3 );
    dag.addEdge( 0, 0 );
    dag.addEdge( 0, 1 );
    dag.addEdge( 1, 1 );
    dag.addEdge( 1, 2 );
    dag.addEdge( 2, 0 );
    dag.addEdge( 2, 2 );
    fg = FactorGraph( facs );
    RegionGraph G2( fg, fg.maximalFactorDomains() );
    BOOST_CHECK_EQUAL( G2.nrVars(), 3 );
    BOOST_CHECK_EQUAL( G2.nrFactors(), 4 );
    BOOST_CHECK_EQUAL( G2.nrEdges(), 7 );
    BOOST_CHECK_EQUAL( G2.findVar( v0 ), 0 );
    BOOST_CHECK_EQUAL( G2.findVar( v1 ), 1 );
    BOOST_CHECK_EQUAL( G2.findVar( v2 ), 2 );
    BOOST_CHECK_EQUAL( G2.findVars( v01 ), SmallSet<size_t>( 0, 1 ) );
    BOOST_CHECK_EQUAL( G2.findVars( v02 ), SmallSet<size_t>( 0, 2 ) );
    BOOST_CHECK_EQUAL( G2.findVars( v12 ), SmallSet<size_t>( 1, 2 ) );
    BOOST_CHECK_EQUAL( G2.findFactor( v01 ), 0 );
    BOOST_CHECK_EQUAL( G2.findFactor( v12 ), 1 );
    BOOST_CHECK_EQUAL( G2.findFactor( v1 ), 2 );
    BOOST_CHECK_EQUAL( G2.findFactor( v02 ), 3 );
    BOOST_CHECK_EQUAL( G2.delta( 0 ), v12 );
    BOOST_CHECK_EQUAL( G2.delta( 1 ), v02 );
    BOOST_CHECK_EQUAL( G2.delta( 2 ), v01 );
    BOOST_CHECK_EQUAL( G2.Delta( 0 ), v012 );
    BOOST_CHECK_EQUAL( G2.Delta( 1 ), v012 );
    BOOST_CHECK_EQUAL( G2.Delta( 2 ), v012 );
    BOOST_CHECK( G2.isConnected() );
    BOOST_CHECK( !G2.isTree() );
    BOOST_CHECK( G2.isBinary() );
    BOOST_CHECK( G2.isPairwise() );
    BOOST_CHECK( G2.MarkovGraph() == H );
    BOOST_CHECK( G2.bipGraph() == K );
    BOOST_CHECK_EQUAL( G2.maximalFactorDomains().size(), 3 );
    BOOST_CHECK_EQUAL( G2.maximalFactorDomains()[0], v01 );
    BOOST_CHECK_EQUAL( G2.maximalFactorDomains()[1], v12 );
    BOOST_CHECK_EQUAL( G2.maximalFactorDomains()[2], v02 );
    BOOST_CHECK( G2.DAG() == dag );
    BOOST_CHECK( G2.checkCountingNumbers() );
    BOOST_CHECK_EQUAL( G2.nrORs(), 3 );
    BOOST_CHECK_EQUAL( G2.OR(0).c(), 1 );
    BOOST_CHECK_EQUAL( G2.OR(0), facs[0] * facs[2] );
    BOOST_CHECK_EQUAL( G2.OR(1).c(), 1 );
    BOOST_CHECK_EQUAL( G2.OR(1), facs[1] );
    BOOST_CHECK_EQUAL( G2.OR(2).c(), 1 );
    BOOST_CHECK_EQUAL( G2.OR(2), facs[3] );
    BOOST_CHECK_EQUAL( G2.nrIRs(), 3 );
    BOOST_CHECK_EQUAL( G2.IR(0).c(), -1.0 );
    BOOST_CHECK_EQUAL( G2.IR(0), v0 );
    BOOST_CHECK_EQUAL( G2.IR(1).c(), -1.0 );
    BOOST_CHECK_EQUAL( G2.IR(1), v1 );
    BOOST_CHECK_EQUAL( G2.IR(2).c(), -1.0 );
    BOOST_CHECK_EQUAL( G2.IR(2), v2 );
    BOOST_CHECK_EQUAL( G2.fac2OR(0), 0 );
    BOOST_CHECK_EQUAL( G2.fac2OR(1), 1 );
    BOOST_CHECK_EQUAL( G2.fac2OR(2), 0 );
    BOOST_CHECK_EQUAL( G2.fac2OR(3), 2 );

    Var v3( 3, 3 );
    VarSet v03( v0, v3 );
    VarSet v13( v1, v3 );
    VarSet v23( v2, v3 );
    VarSet v013 = v01 | v3;
    VarSet v023 = v02 | v3;
    VarSet v123 = v12 | v3;
    VarSet v0123 = v012 | v3;
    vars.push_back( v3 );
    facs.push_back( Factor( v3 ) );
    H.addNode();
    K.addNode1();
    K.addNode2();
    K.addEdge( 3, 4 );
    dag.addNode1();
    fg = FactorGraph( facs );
    RegionGraph G3( fg, fg.maximalFactorDomains() );
    BOOST_CHECK_EQUAL( G3.nrVars(), 4 );
    BOOST_CHECK_EQUAL( G3.nrFactors(), 5 );
    BOOST_CHECK_EQUAL( G3.nrEdges(), 8 );
    BOOST_CHECK_EQUAL( G3.findVar( v0 ), 0 );
    BOOST_CHECK_EQUAL( G3.findVar( v1 ), 1 );
    BOOST_CHECK_EQUAL( G3.findVar( v2 ), 2 );
    BOOST_CHECK_EQUAL( G3.findVar( v3 ), 3 );
    BOOST_CHECK_EQUAL( G3.findVars( v01 ), SmallSet<size_t>( 0, 1 ) );
    BOOST_CHECK_EQUAL( G3.findVars( v02 ), SmallSet<size_t>( 0, 2 ) );
    BOOST_CHECK_EQUAL( G3.findVars( v12 ), SmallSet<size_t>( 1, 2 ) );
    BOOST_CHECK_EQUAL( G3.findFactor( v01 ), 0 );
    BOOST_CHECK_EQUAL( G3.findFactor( v12 ), 1 );
    BOOST_CHECK_EQUAL( G3.findFactor( v1 ), 2 );
    BOOST_CHECK_EQUAL( G3.findFactor( v02 ), 3 );
    BOOST_CHECK_EQUAL( G3.findFactor( v3 ), 4 );
    BOOST_CHECK_THROW( G3.findFactor( v23 ), Exception );
    BOOST_CHECK_EQUAL( G3.delta( 0 ), v12 );
    BOOST_CHECK_EQUAL( G3.delta( 1 ), v02 );
    BOOST_CHECK_EQUAL( G3.delta( 2 ), v01 );
    BOOST_CHECK_EQUAL( G3.delta( 3 ), VarSet() );
    BOOST_CHECK_EQUAL( G3.Delta( 0 ), v012 );
    BOOST_CHECK_EQUAL( G3.Delta( 1 ), v012 );
    BOOST_CHECK_EQUAL( G3.Delta( 2 ), v012 );
    BOOST_CHECK_EQUAL( G3.Delta( 3 ), v3 );
    BOOST_CHECK( !G3.isConnected() );
    BOOST_CHECK( !G3.isTree() );
    BOOST_CHECK( !G3.isBinary() );
    BOOST_CHECK( G3.isPairwise() );
    BOOST_CHECK( G3.MarkovGraph() == H );
    BOOST_CHECK( G3.bipGraph() == K );
    BOOST_CHECK_EQUAL( G3.maximalFactorDomains().size(), 4 );
    BOOST_CHECK_EQUAL( G3.maximalFactorDomains()[0], v01 );
    BOOST_CHECK_EQUAL( G3.maximalFactorDomains()[1], v12 );
    BOOST_CHECK_EQUAL( G3.maximalFactorDomains()[2], v02 );
    BOOST_CHECK_EQUAL( G3.maximalFactorDomains()[3], v3 );
    BOOST_CHECK( G3.DAG() == dag );
    BOOST_CHECK( G3.checkCountingNumbers() );
    BOOST_CHECK_EQUAL( G3.nrORs(), 4 );
    BOOST_CHECK_EQUAL( G3.OR(0).c(), 1 );
    BOOST_CHECK_EQUAL( G3.OR(0), facs[0] * facs[2] );
    BOOST_CHECK_EQUAL( G3.OR(1).c(), 1 );
    BOOST_CHECK_EQUAL( G3.OR(1), facs[1] );
    BOOST_CHECK_EQUAL( G3.OR(2).c(), 1 );
    BOOST_CHECK_EQUAL( G3.OR(2), facs[3] );
    BOOST_CHECK_EQUAL( G3.OR(3).c(), 1 );
    BOOST_CHECK_EQUAL( G3.OR(3), facs[4] );
    BOOST_CHECK_EQUAL( G3.nrIRs(), 3 );
    BOOST_CHECK_EQUAL( G3.IR(0).c(), -1.0 );
    BOOST_CHECK_EQUAL( G3.IR(0), v0 );
    BOOST_CHECK_EQUAL( G3.IR(1).c(), -1.0 );
    BOOST_CHECK_EQUAL( G3.IR(1), v1 );
    BOOST_CHECK_EQUAL( G3.IR(2).c(), -1.0 );
    BOOST_CHECK_EQUAL( G3.IR(2), v2 );
    BOOST_CHECK_EQUAL( G3.fac2OR(0), 0 );
    BOOST_CHECK_EQUAL( G3.fac2OR(1), 1 );
    BOOST_CHECK_EQUAL( G3.fac2OR(2), 0 );
    BOOST_CHECK_EQUAL( G3.fac2OR(3), 2 );
    BOOST_CHECK_EQUAL( G3.fac2OR(4), 3 );

    facs.push_back( Factor( v123 ) );
    H.addEdge( 1, 3 );
    H.addEdge( 2, 3 );
    K.addNode2();
    K.addEdge( 1, 5 );
    K.addEdge( 2, 5 );
    K.addEdge( 3, 5 );
    dag = BipartiteGraph( 3, 3 );
    dag.addEdge( 0, 0 );
    dag.addEdge( 0, 1 );
    dag.addEdge( 1, 0 );
    dag.addEdge( 1, 2 );
    dag.addEdge( 2, 1 );
    dag.addEdge( 2, 2 );
    fg = FactorGraph( facs );
    RegionGraph G4( fg, fg.maximalFactorDomains() );
    BOOST_CHECK_EQUAL( G4.nrVars(), 4 );
    BOOST_CHECK_EQUAL( G4.nrFactors(), 6 );
    BOOST_CHECK_EQUAL( G4.nrEdges(), 11 );
    BOOST_CHECK_EQUAL( G4.findVar( v0 ), 0 );
    BOOST_CHECK_EQUAL( G4.findVar( v1 ), 1 );
    BOOST_CHECK_EQUAL( G4.findVar( v2 ), 2 );
    BOOST_CHECK_EQUAL( G4.findVar( v3 ), 3 );
    BOOST_CHECK_EQUAL( G4.findVars( v01 ), SmallSet<size_t>( 0, 1 ) );
    BOOST_CHECK_EQUAL( G4.findVars( v02 ), SmallSet<size_t>( 0, 2 ) );
    BOOST_CHECK_EQUAL( G4.findVars( v12 ), SmallSet<size_t>( 1, 2 ) );
    BOOST_CHECK_EQUAL( G4.findFactor( v01 ), 0 );
    BOOST_CHECK_EQUAL( G4.findFactor( v12 ), 1 );
    BOOST_CHECK_EQUAL( G4.findFactor( v1 ), 2 );
    BOOST_CHECK_EQUAL( G4.findFactor( v02 ), 3 );
    BOOST_CHECK_EQUAL( G4.findFactor( v3 ), 4 );
    BOOST_CHECK_EQUAL( G4.findFactor( v123 ), 5 );
    BOOST_CHECK_THROW( G4.findFactor( v23 ), Exception );
    BOOST_CHECK_EQUAL( G4.delta( 0 ), v12 );
    BOOST_CHECK_EQUAL( G4.delta( 1 ), v023 );
    BOOST_CHECK_EQUAL( G4.delta( 2 ), v013 );
    BOOST_CHECK_EQUAL( G4.delta( 3 ), v12 );
    BOOST_CHECK_EQUAL( G4.Delta( 0 ), v012 );
    BOOST_CHECK_EQUAL( G4.Delta( 1 ), v0123 );
    BOOST_CHECK_EQUAL( G4.Delta( 2 ), v0123 );
    BOOST_CHECK_EQUAL( G4.Delta( 3 ), v123 );
    BOOST_CHECK( G4.isConnected() );
    BOOST_CHECK( !G4.isTree() );
    BOOST_CHECK( !G4.isBinary() );
    BOOST_CHECK( !G4.isPairwise() );
    BOOST_CHECK( G4.MarkovGraph() == H );
    BOOST_CHECK( G4.bipGraph() == K );
    BOOST_CHECK_EQUAL( G4.maximalFactorDomains().size(), 3 );
    BOOST_CHECK_EQUAL( G4.maximalFactorDomains()[0], v01 );
    BOOST_CHECK_EQUAL( G4.maximalFactorDomains()[1], v02 );
    BOOST_CHECK_EQUAL( G4.maximalFactorDomains()[2], v123 );
    BOOST_CHECK( G4.DAG() == dag );
    BOOST_CHECK( G4.checkCountingNumbers() );
    BOOST_CHECK_EQUAL( G4.nrORs(), 3 );
    BOOST_CHECK_EQUAL( G4.OR(0).c(), 1 );
    BOOST_CHECK_EQUAL( G4.OR(0), facs[0] * facs[2] );
    BOOST_CHECK_EQUAL( G4.OR(1).c(), 1 );
    BOOST_CHECK_EQUAL( G4.OR(1), facs[3] );
    BOOST_CHECK_EQUAL( G4.OR(2).c(), 1 );
    BOOST_CHECK_EQUAL( G4.OR(2), facs[1] * facs[4] * facs[5] );
    BOOST_CHECK_EQUAL( G4.nrIRs(), 3 );
    BOOST_CHECK_EQUAL( G4.IR(0).c(), -1.0 );
    BOOST_CHECK_EQUAL( G4.IR(0), v0 );
    BOOST_CHECK_EQUAL( G4.IR(1).c(), -1.0 );
    BOOST_CHECK_EQUAL( G4.IR(1), v1 );
    BOOST_CHECK_EQUAL( G4.IR(2).c(), -1.0 );
    BOOST_CHECK_EQUAL( G4.IR(2), v2 );
    BOOST_CHECK_EQUAL( G4.fac2OR(0), 0 );
    BOOST_CHECK_EQUAL( G4.fac2OR(1), 2 );
    BOOST_CHECK_EQUAL( G4.fac2OR(2), 0 );
    BOOST_CHECK_EQUAL( G4.fac2OR(3), 1 );
    BOOST_CHECK_EQUAL( G4.fac2OR(4), 2 );
    BOOST_CHECK_EQUAL( G4.fac2OR(5), 2 );
}


BOOST_AUTO_TEST_CASE( BackupRestoreTest ) {
    Var v0( 0, 2 );
    Var v1( 1, 2 );
    Var v2( 2, 2 );
    VarSet v01( v0, v1 );
    VarSet v02( v0, v2 );
    VarSet v12( v1, v2 );
    VarSet v012 = v01 | v2;

    std::vector<Factor> facs;
    facs.push_back( Factor( v01 ) );
    facs.push_back( Factor( v12 ) );
    facs.push_back( Factor( v1 ) );
    std::vector<Var> vars;
    vars.push_back( v0 );
    vars.push_back( v1 );
    vars.push_back( v2 );

    FactorGraph fg( facs );
    RegionGraph G( fg, fg.maximalFactorDomains() );
    RegionGraph Gorg( G );
    BOOST_CHECK_EQUAL( G.OR(0).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );

    BOOST_CHECK_THROW( G.setFactor( 0, Factor( v0 ), false ), Exception );
    G.setFactor( 0, Factor( v01, 2.0 ), false );
    BOOST_CHECK_EQUAL( G.OR(0).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
    BOOST_CHECK_THROW( G.restoreFactor( 0 ), Exception );
    G.setFactor( 0, Factor( v01, 3.0 ), true );
    BOOST_CHECK_EQUAL( G.OR(0).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
    G.restoreFactor( 0 );
    BOOST_CHECK_EQUAL( G.factor( 0 )[0], 2.0 );
    BOOST_CHECK_EQUAL( G.OR(0).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
    G.setFactor( 0, Gorg.factor( 0 ), false );
    BOOST_CHECK_EQUAL( G.OR(0).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
    G.backupFactor( 0 );
    BOOST_CHECK_EQUAL( G.factor(0), Gorg.factor(0) );
    G.setFactor( 0, Factor( v01, 2.0 ), false );
    BOOST_CHECK_EQUAL( G.OR(0).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
    BOOST_CHECK_EQUAL( G.factor( 0 )[0], 2.0 );
    G.restoreFactor( 0 );
    BOOST_CHECK_EQUAL( G.factor(0), Gorg.factor(0) );
    BOOST_CHECK_EQUAL( G.OR(0).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1).c(), 1 );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );

    std::map<size_t, Factor> fs;
    fs[0] = Factor( v01, 3.0 );
    fs[2] = Factor( v1, 2.0 );
    G.setFactors( fs, false );
    BOOST_CHECK_EQUAL( G.factor(0), fs[0] );
    BOOST_CHECK_EQUAL( G.factor(2), fs[2] );
    BOOST_CHECK_EQUAL( G.factor(1), Gorg.factor(1) );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
    G.restoreFactors();
    BOOST_CHECK_EQUAL( G.factor(0), fs[0] );
    BOOST_CHECK_EQUAL( G.factor(2), fs[2] );
    BOOST_CHECK_EQUAL( G.factor(1), Gorg.factor(1) );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
    G = Gorg;
    BOOST_CHECK_EQUAL( G.factor(0), Gorg.factor(0) );
    BOOST_CHECK_EQUAL( G.factor(1), Gorg.factor(1) );
    BOOST_CHECK_EQUAL( G.factor(2), Gorg.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
    G.setFactors( fs, true );
    BOOST_CHECK_EQUAL( G.factor(0), fs[0] );
    BOOST_CHECK_EQUAL( G.factor(2), fs[2] );
    BOOST_CHECK_EQUAL( G.factor(1), Gorg.factor(1) );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
    G.restoreFactors();
    BOOST_CHECK_EQUAL( G.factor(0), Gorg.factor(0) );
    BOOST_CHECK_EQUAL( G.factor(1), Gorg.factor(1) );
    BOOST_CHECK_EQUAL( G.factor(2), Gorg.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
    std::set<size_t> fsind;
    fsind.insert( 0 );
    fsind.insert( 2 );
    G.backupFactors( fsind );
    G.setFactors( fs, false );
    BOOST_CHECK_EQUAL( G.factor(0), fs[0] );
    BOOST_CHECK_EQUAL( G.factor(2), fs[2] );
    BOOST_CHECK_EQUAL( G.factor(1), Gorg.factor(1) );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
    G.restoreFactors();
    BOOST_CHECK_EQUAL( G.factor(0), Gorg.factor(0) );
    BOOST_CHECK_EQUAL( G.factor(1), Gorg.factor(1) );
    BOOST_CHECK_EQUAL( G.factor(2), Gorg.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );

    G.backupFactors( v2 );
    G.setFactor( 1, Factor(v12, 5.0) );
    BOOST_CHECK_EQUAL( G.factor(0), Gorg.factor(0) );
    BOOST_CHECK_EQUAL( G.factor(1), Factor(v12, 5.0) );
    BOOST_CHECK_EQUAL( G.factor(2), Gorg.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
    G.restoreFactors( v2 );
    BOOST_CHECK_EQUAL( G.factor(0), Gorg.factor(0) );
    BOOST_CHECK_EQUAL( G.factor(1), Gorg.factor(1) );
    BOOST_CHECK_EQUAL( G.factor(2), Gorg.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );

    G.backupFactors( v1 );
    fs[1] = Factor( v12, 5.0 );
    G.setFactors( fs, false );
    BOOST_CHECK_EQUAL( G.factor(0), fs[0] );
    BOOST_CHECK_EQUAL( G.factor(1), fs[1] );
    BOOST_CHECK_EQUAL( G.factor(2), fs[2] );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
    G.restoreFactors();
    BOOST_CHECK_EQUAL( G.factor(0), Gorg.factor(0) );
    BOOST_CHECK_EQUAL( G.factor(1), Gorg.factor(1) );
    BOOST_CHECK_EQUAL( G.factor(2), Gorg.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
    G.setFactors( fs, true );
    BOOST_CHECK_EQUAL( G.factor(0), fs[0] );
    BOOST_CHECK_EQUAL( G.factor(1), fs[1] );
    BOOST_CHECK_EQUAL( G.factor(2), fs[2] );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
    G.restoreFactors( v1 );
    BOOST_CHECK_EQUAL( G.factor(0), Gorg.factor(0) );
    BOOST_CHECK_EQUAL( G.factor(1), Gorg.factor(1) );
    BOOST_CHECK_EQUAL( G.factor(2), Gorg.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(0), G.factor(0) * G.factor(2) );
    BOOST_CHECK_EQUAL( G.OR(1), G.factor(1) );
}


BOOST_AUTO_TEST_CASE( TransformationsTest ) {
    Var v0( 0, 2 );
    Var v1( 1, 2 );
    Var v2( 2, 2 );
    VarSet v01( v0, v1 );
    VarSet v02( v0, v2 );
    VarSet v12( v1, v2 );
    VarSet v012 = v01 | v2;

    std::vector<Factor> facs;
    facs.push_back( Factor( v01 ).randomize() );
    facs.push_back( Factor( v12 ).randomize() );
    facs.push_back( Factor( v1 ).randomize() );
    std::vector<Var> vars;
    vars.push_back( v0 );
    vars.push_back( v1 );
    vars.push_back( v2 );

    FactorGraph fg( facs );
    RegionGraph G( fg, fg.maximalFactorDomains() );

    FactorGraph Gsmall = G.maximalFactors();
    BOOST_CHECK_EQUAL( Gsmall.nrVars(), 3 );
    BOOST_CHECK_EQUAL( Gsmall.nrFactors(), 2 );
    BOOST_CHECK_EQUAL( Gsmall.factor( 0 ), G.factor( 0 ) * G.factor( 2 ) );
    BOOST_CHECK_EQUAL( Gsmall.factor( 1 ), G.factor( 1 ) );

    size_t i = 0;
    for( size_t x = 0; x < 2; x++ ) {
        FactorGraph Gcl = G.clamped( i, x );
        BOOST_CHECK_EQUAL( Gcl.nrVars(), 3 );
        BOOST_CHECK_EQUAL( Gcl.nrFactors(), 3 );
        BOOST_CHECK_EQUAL( Gcl.factor(0), createFactorDelta(vars[i], x) );
        BOOST_CHECK_EQUAL( Gcl.factor(1), G.factor(0).slice(vars[i], x) * G.factor(2) );
        BOOST_CHECK_EQUAL( Gcl.factor(2), G.factor(1) );
    }
    i = 1;
    for( size_t x = 0; x < 2; x++ ) {
        FactorGraph Gcl = G.clamped( i, x );
        BOOST_CHECK_EQUAL( Gcl.nrVars(), 3 );
        BOOST_CHECK_EQUAL( Gcl.nrFactors(), 3 );
        BOOST_CHECK_EQUAL( Gcl.factor(0), createFactorDelta(vars[i], x) * G.factor(2).slice(vars[i],x) );
        BOOST_CHECK_EQUAL( Gcl.factor(1), G.factor(0).slice(vars[i], x) );
        BOOST_CHECK_EQUAL( Gcl.factor(2), G.factor(1).slice(vars[i], x) );
    }
    i = 2;
    for( size_t x = 0; x < 2; x++ ) {
        FactorGraph Gcl = G.clamped( i, x );
        BOOST_CHECK_EQUAL( Gcl.nrVars(), 3 );
        BOOST_CHECK_EQUAL( Gcl.nrFactors(), 3 );
        BOOST_CHECK_EQUAL( Gcl.factor(0), createFactorDelta(vars[i], x) );
        BOOST_CHECK_EQUAL( Gcl.factor(1), G.factor(0) );
        BOOST_CHECK_EQUAL( Gcl.factor(2), G.factor(1).slice(vars[i], x) * G.factor(2) );
    }
}


BOOST_AUTO_TEST_CASE( OperationsTest ) {
    Var v0( 0, 2 );
    Var v1( 1, 2 );
    Var v2( 2, 2 );
    VarSet v01( v0, v1 );
    VarSet v02( v0, v2 );
    VarSet v12( v1, v2 );
    VarSet v012 = v01 | v2;

    std::vector<Factor> facs;
    facs.push_back( Factor( v01 ).randomize() );
    facs.push_back( Factor( v12 ).randomize() );
    facs.push_back( Factor( v1 ).randomize() );
    std::vector<Var> vars;
    vars.push_back( v0 );
    vars.push_back( v1 );
    vars.push_back( v2 );

    FactorGraph fg( facs );
    RegionGraph G( fg, fg.maximalFactorDomains() );

    // clamp
    RegionGraph Gcl = G;
    for( size_t i = 0; i < 3; i++ )
        for( size_t x = 0; x < 2; x++ ) {
            Gcl.clamp( i, x, true );
            Factor delta = createFactorDelta( vars[i], x );
            BOOST_CHECK_EQUAL( Gcl.nrVars(), 3 );
            BOOST_CHECK_EQUAL( Gcl.nrFactors(), 3 );
            for( size_t j = 0; j < 3; j++ )
                if( G.factor(j).vars().contains( vars[i] ) )
                    BOOST_CHECK_EQUAL( Gcl.factor(j), G.factor(j) * delta );
                else
                    BOOST_CHECK_EQUAL( Gcl.factor(j), G.factor(j) );
            BOOST_CHECK_EQUAL( Gcl.OR(0), Gcl.factor(0) * Gcl.factor(2) );
            BOOST_CHECK_EQUAL( Gcl.OR(1), Gcl.factor(1) );

            Gcl.restoreFactors();
            for( size_t j = 0; j < 3; j++ )
                BOOST_CHECK_EQUAL( Gcl.factor(j), G.factor(j) );
            BOOST_CHECK_EQUAL( Gcl.OR(0), Gcl.factor(0) * Gcl.factor(2) );
            BOOST_CHECK_EQUAL( Gcl.OR(1), Gcl.factor(1) );
        }

    // clampVar
    for( size_t i = 0; i < 3; i++ )
        for( size_t x = 0; x < 2; x++ ) {
            Gcl.clampVar( i, std::vector<size_t>(1, x), true );
            Factor delta = createFactorDelta( vars[i], x );
            BOOST_CHECK_EQUAL( Gcl.nrVars(), 3 );
            BOOST_CHECK_EQUAL( Gcl.nrFactors(), 3 );
            for( size_t j = 0; j < 3; j++ )
                if( G.factor(j).vars().contains( vars[i] ) )
                    BOOST_CHECK_EQUAL( Gcl.factor(j), G.factor(j) * delta );
                else
                    BOOST_CHECK_EQUAL( Gcl.factor(j), G.factor(j) );
            BOOST_CHECK_EQUAL( Gcl.OR(0), Gcl.factor(0) * Gcl.factor(2) );
            BOOST_CHECK_EQUAL( Gcl.OR(1), Gcl.factor(1) );

            Gcl.restoreFactors();
            for( size_t j = 0; j < 3; j++ )
                BOOST_CHECK_EQUAL( Gcl.factor(j), G.factor(j) );
            BOOST_CHECK_EQUAL( Gcl.OR(0), Gcl.factor(0) * Gcl.factor(2) );
            BOOST_CHECK_EQUAL( Gcl.OR(1), Gcl.factor(1) );
        }
    for( size_t i = 0; i < 3; i++ )
        for( size_t x = 0; x < 2; x++ ) {
            Gcl.clampVar( i, std::vector<size_t>(), true );
            BOOST_CHECK_EQUAL( Gcl.nrVars(), 3 );
            BOOST_CHECK_EQUAL( Gcl.nrFactors(), 3 );
            for( size_t j = 0; j < 3; j++ )
                if( G.factor(j).vars().contains( vars[i] ) )
                    BOOST_CHECK_EQUAL( Gcl.factor(j), G.factor(j) * 0.0 );
                else
                    BOOST_CHECK_EQUAL( Gcl.factor(j), G.factor(j) );
            BOOST_CHECK_EQUAL( Gcl.OR(0), Gcl.factor(0) * Gcl.factor(2) );
            BOOST_CHECK_EQUAL( Gcl.OR(1), Gcl.factor(1) );

            Gcl.restoreFactors();
            for( size_t j = 0; j < 3; j++ )
                BOOST_CHECK_EQUAL( Gcl.factor(j), G.factor(j) );
            BOOST_CHECK_EQUAL( Gcl.OR(0), Gcl.factor(0) * Gcl.factor(2) );
            BOOST_CHECK_EQUAL( Gcl.OR(1), Gcl.factor(1) );
        }
    std::vector<size_t> both;
    both.push_back( 0 );
    both.push_back( 1 );
    for( size_t i = 0; i < 3; i++ )
        for( size_t x = 0; x < 2; x++ ) {
            Gcl.clampVar( i, both, true );
            BOOST_CHECK_EQUAL( Gcl.nrVars(), 3 );
            BOOST_CHECK_EQUAL( Gcl.nrFactors(), 3 );
            for( size_t j = 0; j < 3; j++ )
                BOOST_CHECK_EQUAL( Gcl.factor(j), G.factor(j) );
            BOOST_CHECK_EQUAL( Gcl.OR(0), Gcl.factor(0) * Gcl.factor(2) );
            BOOST_CHECK_EQUAL( Gcl.OR(1), Gcl.factor(1) );
            Gcl.restoreFactors();
            BOOST_CHECK_EQUAL( Gcl.OR(0), Gcl.factor(0) * Gcl.factor(2) );
            BOOST_CHECK_EQUAL( Gcl.OR(1), Gcl.factor(1) );
        }

    // clampFactor
    for( size_t x = 0; x < 4; x++ ) {
        Gcl.clampFactor( 0, std::vector<size_t>(1,x), true );
        Factor mask( v01, 0.0 );
        mask.set( x, 1.0 );
        BOOST_CHECK_EQUAL( Gcl.factor(0), G.factor(0) * mask );
        BOOST_CHECK_EQUAL( Gcl.factor(1), G.factor(1) );
        BOOST_CHECK_EQUAL( Gcl.factor(2), G.factor(2) );
        BOOST_CHECK_EQUAL( Gcl.OR(0), Gcl.factor(0) * Gcl.factor(2) );
        BOOST_CHECK_EQUAL( Gcl.OR(1), Gcl.factor(1) );
        Gcl.restoreFactor( 0 );
        BOOST_CHECK_EQUAL( Gcl.OR(0), Gcl.factor(0) * Gcl.factor(2) );
        BOOST_CHECK_EQUAL( Gcl.OR(1), Gcl.factor(1) );
    }
    for( size_t x = 0; x < 4; x++ ) {
        Gcl.clampFactor( 1, std::vector<size_t>(1,x), true );
        Factor mask( v12, 0.0 );
        mask.set( x, 1.0 );
        BOOST_CHECK_EQUAL( Gcl.factor(0), G.factor(0) );
        BOOST_CHECK_EQUAL( Gcl.factor(1), G.factor(1) * mask );
        BOOST_CHECK_EQUAL( Gcl.factor(2), G.factor(2) );
        BOOST_CHECK_EQUAL( Gcl.OR(0), Gcl.factor(0) * Gcl.factor(2) );
        BOOST_CHECK_EQUAL( Gcl.OR(1), Gcl.factor(1) );
        Gcl.restoreFactor( 1 );
        BOOST_CHECK_EQUAL( Gcl.OR(0), Gcl.factor(0) * Gcl.factor(2) );
        BOOST_CHECK_EQUAL( Gcl.OR(1), Gcl.factor(1) );
    }
    for( size_t x = 0; x < 2; x++ ) {
        Gcl.clampFactor( 2, std::vector<size_t>(1,x), true );
        Factor mask( v1, 0.0 );
        mask.set( x, 1.0 );
        BOOST_CHECK_EQUAL( Gcl.factor(0), G.factor(0) );
        BOOST_CHECK_EQUAL( Gcl.factor(1), G.factor(1) );
        BOOST_CHECK_EQUAL( Gcl.factor(2), G.factor(2) * mask );
        BOOST_CHECK_EQUAL( Gcl.OR(0), Gcl.factor(0) * Gcl.factor(2) );
        BOOST_CHECK_EQUAL( Gcl.OR(1), Gcl.factor(1) );
        Gcl.restoreFactors();
        BOOST_CHECK_EQUAL( Gcl.OR(0), Gcl.factor(0) * Gcl.factor(2) );
        BOOST_CHECK_EQUAL( Gcl.OR(1), Gcl.factor(1) );
    }

    // makeCavity
    RegionGraph Gcav( G );
    Gcav.makeCavity( 0, true );
    BOOST_CHECK_EQUAL( Gcav.factor(0), Factor( v01, 1.0 ) );
    BOOST_CHECK_EQUAL( Gcav.factor(1), G.factor(1) );
    BOOST_CHECK_EQUAL( Gcav.factor(2), G.factor(2) );
    BOOST_CHECK_EQUAL( Gcav.OR(0), Gcav.factor(0) * Gcav.factor(2) );
    BOOST_CHECK_EQUAL( Gcav.OR(1), Gcav.factor(1) );
    Gcav.restoreFactors();
    BOOST_CHECK_EQUAL( Gcav.OR(0), Gcav.factor(0) * Gcav.factor(2) );
    BOOST_CHECK_EQUAL( Gcav.OR(1), Gcav.factor(1) );
    Gcav.makeCavity( 1, true );
    BOOST_CHECK_EQUAL( Gcav.factor(0), Factor( v01, 1.0 ) );
    BOOST_CHECK_EQUAL( Gcav.factor(1), Factor( v12, 1.0 ) );
    BOOST_CHECK_EQUAL( Gcav.factor(2), Factor( v1, 1.0 ) );
    BOOST_CHECK_EQUAL( Gcav.OR(0), Gcav.factor(0) * Gcav.factor(2) );
    BOOST_CHECK_EQUAL( Gcav.OR(1), Gcav.factor(1) );
    Gcav.restoreFactors();
    BOOST_CHECK_EQUAL( Gcav.OR(0), Gcav.factor(0) * Gcav.factor(2) );
    BOOST_CHECK_EQUAL( Gcav.OR(1), Gcav.factor(1) );
    Gcav.makeCavity( 2, true );
    BOOST_CHECK_EQUAL( Gcav.factor(0), G.factor(0) );
    BOOST_CHECK_EQUAL( Gcav.factor(1), Factor( v12, 1.0 ) );
    BOOST_CHECK_EQUAL( Gcav.factor(2), G.factor(2) );
    BOOST_CHECK_EQUAL( Gcav.OR(0), Gcav.factor(0) * Gcav.factor(2) );
    BOOST_CHECK_EQUAL( Gcav.OR(1), Gcav.factor(1) );
    Gcav.restoreFactors();
    BOOST_CHECK_EQUAL( Gcav.OR(0), Gcav.factor(0) * Gcav.factor(2) );
    BOOST_CHECK_EQUAL( Gcav.OR(1), Gcav.factor(1) );
}


BOOST_AUTO_TEST_CASE( IOTest ) {
    Var v0( 0, 2 );
    Var v1( 1, 2 );
    Var v2( 2, 2 );
    VarSet v01( v0, v1 );
    VarSet v02( v0, v2 );
    VarSet v12( v1, v2 );
    VarSet v012 = v01 | v2;

    std::vector<Factor> facs;
    facs.push_back( Factor( v01, 1.0 ) );
    facs.push_back( Factor( v12, 2.0 ) );
    facs.push_back( Factor( v1, 3.0 ) );
    std::vector<Var> vars;
    vars.push_back( v0 );
    vars.push_back( v1 );
    vars.push_back( v2 );

    FactorGraph fg( facs );
    RegionGraph G( fg, fg.maximalFactorDomains() );
    BOOST_CHECK_THROW( G.WriteToFile( "regiongraph_test.fg" ), Exception );
    BOOST_CHECK_THROW( G.ReadFromFile( "regiongraph_test.fg" ), Exception );
    BOOST_CHECK_THROW( G.printDot( std::cout ), Exception );

    std::stringstream ss;
    std::string s;
    ss << G;
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "digraph RegionGraph {" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "node[shape=box];" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\ta0 [label=\"a0: {x0, x1}, c=1\"];" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\ta1 [label=\"a1: {x1, x2}, c=1\"];" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "node[shape=ellipse];" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\tb0 [label=\"b0: {x1}, c=-1\"];" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\ta0 -> b0;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "\ta1 -> b0;" );
    std::getline( ss, s ); BOOST_CHECK_EQUAL( s, "}" );
}
