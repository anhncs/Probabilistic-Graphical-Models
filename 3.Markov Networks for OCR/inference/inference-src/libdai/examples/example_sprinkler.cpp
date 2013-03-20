/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/factorgraph.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace dai;

int main() {
    // This example program illustrates how to construct a factorgraph
    // by means of the sprinkler network example discussed at
    // http://www.cs.ubc.ca/~murphyk/Bayes/bnintro.html

    Var C(0, 2);  // Define binary variable Cloudy (with label 0)
    Var S(1, 2);  // Define binary variable Sprinkler (with label 1)
    Var R(2, 2);  // Define binary variable Rain (with label 2)
    Var W(3, 2);  // Define binary variable Wetgrass (with label 3)

    // Define probability distribution for C
    Factor P_C( C );
    P_C.set(0, 0.5);   // C = 0
    P_C.set(1, 0.5);   // C = 1

    // Define conditional probability of S given C
    Factor P_S_given_C( VarSet( S, C ) );
    P_S_given_C.set(0, 0.5);   // C = 0, S = 0
    P_S_given_C.set(1, 0.9);   // C = 1, S = 0
    P_S_given_C.set(2, 0.5);   // C = 0, S = 1
    P_S_given_C.set(3, 0.1);   // C = 1, S = 1

    // Define conditional probability of R given C
    Factor P_R_given_C( VarSet( R, C ) );
    P_R_given_C.set(0, 0.8);   // C = 0, R = 0
    P_R_given_C.set(1, 0.2);   // C = 1, R = 0
    P_R_given_C.set(2, 0.2);   // C = 0, R = 1
    P_R_given_C.set(3, 0.8);   // C = 1, R = 1

    // Define conditional probability of W given S and R
    Factor P_W_given_S_R( VarSet( S, R ) | W );
    P_W_given_S_R.set(0, 1.0);  // S = 0, R = 0, W = 0
    P_W_given_S_R.set(1, 0.1);  // S = 1, R = 0, W = 0
    P_W_given_S_R.set(2, 0.1);  // S = 0, R = 1, W = 0
    P_W_given_S_R.set(3, 0.01); // S = 1, R = 1, W = 0
    P_W_given_S_R.set(4, 0.0);  // S = 0, R = 0, W = 1
    P_W_given_S_R.set(5, 0.9);  // S = 1, R = 0, W = 1
    P_W_given_S_R.set(6, 0.9);  // S = 0, R = 1, W = 1
    P_W_given_S_R.set(7, 0.99); // S = 1, R = 1, W = 1

    // Build factor graph consisting of those four factors
    vector<Factor> SprinklerFactors;
    SprinklerFactors.push_back( P_C );
    SprinklerFactors.push_back( P_R_given_C );
    SprinklerFactors.push_back( P_S_given_C );
    SprinklerFactors.push_back( P_W_given_S_R );
    FactorGraph SprinklerNetwork( SprinklerFactors );

    // Write factorgraph to a file
    SprinklerNetwork.WriteToFile( "sprinkler.fg" );
    cout << "Sprinkler network written to sprinkler.fg" << endl;

    // Output some information about the factorgraph
    cout << SprinklerNetwork.nrVars() << " variables" << endl;
    cout << SprinklerNetwork.nrFactors() << " factors" << endl;

    // Calculate joint probability of all four variables
    Factor P;
    for( size_t I = 0; I < SprinklerNetwork.nrFactors(); I++ )
        P *= SprinklerNetwork.factor( I );
    // P.normalize();  // Not necessary: a Bayesian network is already normalized by definition

    // Calculate some probabilities
    Real denom = P.marginal( W )[1];
    cout << "P(W=1) = " << denom << endl;
    cout << "P(S=1 | W=1) = " << P.marginal( VarSet( S, W ) )[3] / denom << endl;
    cout << "P(R=1 | W=1) = " << P.marginal( VarSet( R, W ) )[3] / denom << endl;

    return 0;
}
