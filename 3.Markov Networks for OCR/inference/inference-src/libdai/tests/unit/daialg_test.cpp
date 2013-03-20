/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/daialg.h>
#include <dai/alldai.h>
#include <strstream>


using namespace dai;


const double tol = 1e-8;


#define BOOST_TEST_MODULE DAIAlgTest


#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( calcMarginalTest ) {
    Var v0( 0, 2 );
    Var v1( 1, 2 );
    Var v2( 2, 2 );
    Var v3( 3, 2 );
    VarSet v01( v0, v1 );
    VarSet v02( v0, v2 );
    VarSet v03( v0, v3 );
    VarSet v12( v1, v2 );
    VarSet v13( v1, v3 );
    VarSet v23( v2, v3 );
    std::vector<Factor> facs;
    facs.push_back( createFactorIsing( v0, v1, 1.0 ) );
    facs.push_back( createFactorIsing( v1, v2, 1.0 ) );
    facs.push_back( createFactorIsing( v2, v3, 1.0 ) );
    facs.push_back( createFactorIsing( v3, v0, 1.0 ) );
    facs.push_back( createFactorIsing( v0, -1.0 ) );
    facs.push_back( createFactorIsing( v1, -1.0 ) );
    facs.push_back( createFactorIsing( v2, -1.0 ) );
    facs.push_back( createFactorIsing( v3, 1.0 ) );
    Factor joint = facs[0] * facs[1] * facs[2] * facs[3] * facs[4] * facs[5] * facs[6] * facs[7];
    FactorGraph fg( facs );
    ExactInf ei( fg, PropertySet()("verbose",(size_t)0) );
    ei.init();
    ei.run();
    VarSet vs;

    vs = v0;        BOOST_CHECK( dist( calcMarginal( ei, vs, false ), joint.marginal( vs ), DISTTV ) < tol );
    vs = v1;        BOOST_CHECK( dist( calcMarginal( ei, vs, false ), joint.marginal( vs ), DISTTV ) < tol );
    vs = v2;        BOOST_CHECK( dist( calcMarginal( ei, vs, false ), joint.marginal( vs ), DISTTV ) < tol );
    vs = v3;        BOOST_CHECK( dist( calcMarginal( ei, vs, false ), joint.marginal( vs ), DISTTV ) < tol );
    vs = v01;       BOOST_CHECK( dist( calcMarginal( ei, vs, false ), joint.marginal( vs ), DISTTV ) < tol );
    vs = v02;       BOOST_CHECK( dist( calcMarginal( ei, vs, false ), joint.marginal( vs ), DISTTV ) < tol );
    vs = v03;       BOOST_CHECK( dist( calcMarginal( ei, vs, false ), joint.marginal( vs ), DISTTV ) < tol );
    vs = v12;       BOOST_CHECK( dist( calcMarginal( ei, vs, false ), joint.marginal( vs ), DISTTV ) < tol );
    vs = v13;       BOOST_CHECK( dist( calcMarginal( ei, vs, false ), joint.marginal( vs ), DISTTV ) < tol );
    vs = v23;       BOOST_CHECK( dist( calcMarginal( ei, vs, false ), joint.marginal( vs ), DISTTV ) < tol );
    vs = v01 | v2;  BOOST_CHECK( dist( calcMarginal( ei, vs, false ), joint.marginal( vs ), DISTTV ) < tol );
    vs = v01 | v3;  BOOST_CHECK( dist( calcMarginal( ei, vs, false ), joint.marginal( vs ), DISTTV ) < tol );
    vs = v02 | v3;  BOOST_CHECK( dist( calcMarginal( ei, vs, false ), joint.marginal( vs ), DISTTV ) < tol );
    vs = v12 | v3;  BOOST_CHECK( dist( calcMarginal( ei, vs, false ), joint.marginal( vs ), DISTTV ) < tol );
    vs = v01 | v23; BOOST_CHECK( dist( calcMarginal( ei, vs, false ), joint.marginal( vs ), DISTTV ) < tol );
}


BOOST_AUTO_TEST_CASE( calcPairBeliefsTest ) {
    Var v0( 0, 2 );
    Var v1( 1, 2 );
    Var v2( 2, 2 );
    Var v3( 3, 2 );
    VarSet v01( v0, v1 );
    VarSet v02( v0, v2 );
    VarSet v03( v0, v3 );
    VarSet v12( v1, v2 );
    VarSet v13( v1, v3 );
    VarSet v23( v2, v3 );
    std::vector<Factor> facs;
    facs.push_back( createFactorIsing( v0, v1, 1.0 ) );
    facs.push_back( createFactorIsing( v1, v2, 1.0 ) );
    facs.push_back( createFactorIsing( v2, v3, 1.0 ) );
    facs.push_back( createFactorIsing( v3, v0, 1.0 ) );
    facs.push_back( createFactorIsing( v0, -1.0 ) );
    facs.push_back( createFactorIsing( v1, -1.0 ) );
    facs.push_back( createFactorIsing( v2, -1.0 ) );
    facs.push_back( createFactorIsing( v3, 1.0 ) );
    Factor joint = facs[0] * facs[1] * facs[2] * facs[3] * facs[4] * facs[5] * facs[6] * facs[7];
    FactorGraph fg( facs );
    ExactInf ei( fg, PropertySet()("verbose",(size_t)0) );
    ei.init();
    ei.run();
    VarSet vs;

    std::vector<Factor> pb = calcPairBeliefs( ei, v01 | v23, false, false );
    BOOST_CHECK( dist( pb[0], joint.marginal( v01 ), DISTTV ) < tol );
    BOOST_CHECK( dist( pb[1], joint.marginal( v02 ), DISTTV ) < tol );
    BOOST_CHECK( dist( pb[2], joint.marginal( v03 ), DISTTV ) < tol );
    BOOST_CHECK( dist( pb[3], joint.marginal( v12 ), DISTTV ) < tol );
    BOOST_CHECK( dist( pb[4], joint.marginal( v13 ), DISTTV ) < tol );
    BOOST_CHECK( dist( pb[5], joint.marginal( v23 ), DISTTV ) < tol );

    pb = calcPairBeliefs( ei, v01 | v23, false, true );
    BOOST_CHECK( dist( pb[0], joint.marginal( v01 ), DISTTV ) < tol );
    BOOST_CHECK( dist( pb[1], joint.marginal( v02 ), DISTTV ) < tol );
    BOOST_CHECK( dist( pb[2], joint.marginal( v03 ), DISTTV ) < tol );
    BOOST_CHECK( dist( pb[3], joint.marginal( v12 ), DISTTV ) < tol );
    BOOST_CHECK( dist( pb[4], joint.marginal( v13 ), DISTTV ) < tol );
    BOOST_CHECK( dist( pb[5], joint.marginal( v23 ), DISTTV ) < tol );
}
