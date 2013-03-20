/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/varset.h>
#include <dai/index.h>
#include <iostream>
#include <iomanip>

using namespace std;
using namespace dai;

int main() {
    Var x0(0, 2);   // Define binary  variable x0 (with label 0)
    Var x1(1, 3);   // Define ternary variable x1 (with label 1)
    Var x2(2, 2);   // Define binary  variable x2 (with label 2)

    // Define vector V = (x1, x2, x0)
    vector<Var> V;      // Define a vector of variables
    V.push_back( x1 );  // V[0] = x1;
    V.push_back( x2 );  // V[1] = x2;
    V.push_back( x0 );  // V[2] = x0;
    cout << "V = " << V << endl; // Note that the elements of X are not necessarily ordered according to their labels

    // Define set X = {x0, x1, x2}
    VarSet X; // empty
    X |= x2;  // X = {x2}
    X |= x0;  // X = {x0, x2}
    X |= x1;  // X = {x0, x1, x2}
    cout << "X = " << X << endl; // Note that the elements of X are ordered according to their labels

    cout << "Note that the ordering of the variables in X is the canonical ordering" << endl;
    cout << "(ascendingly according to their labels) but the ordering in V is different." << endl << endl;

    // N = number of variables in V (and X)
    size_t N = V.size();

    // Define a Permute object based on the variables in V
    Permute sigma(V);
    // Each Var in V corresponds with a dimension in a multi-dimensional array.
    // The permutation sigma permutes these dimensions from the canonical ordering
    // (sorted ascendingly on the label of the variable, i.e., the same ordering as
    // in X) into the ordering these variables have in V.
    cout << "The permutation between both variable orderings is sigma = " << sigma.sigma() << ", or more verbosely:" << endl;
    for( size_t n = 0; n < N; n++ )
        cout << "  sigma[" << n << "] = " << sigma[n] << endl;
    cout << "This means that variable V[sigma[n]] should correspond with the n'th variable in X (for n=0,...," << (N-1) << ")...";
    // Check whether the permutation works as advertised
    VarSet::const_iterator X_n = X.begin();
    for( size_t n = 0; n < N; n++, X_n++ )
        DAI_ASSERT( V[sigma[n]] == *X_n );
    cout << "OK. " << endl << endl;

    // Iterate over the joint states of the variables, according to the ordering in V
    cout << "The states of the variables x0,x1,x2 are, according to the ordering in V:" << endl;
    cout << "SV:  x0:  x1:  x2:" << endl;
    std::vector<size_t> ranges;
    for( size_t i = 0; i < V.size(); i++ )
        ranges.push_back( V[i].states() );
    for( multifor SV(ranges); SV.valid(); ++SV )
        cout << setw(2) << (size_t)SV << "    " << SV[sigma[0]] << "    " << SV[sigma[1]] << "    " << SV[sigma[2]] << endl;
    cout << endl;

    // Iterate over the joint states of the variables, according to the canonical ordering in X
    cout << "The states of the variables x0,x1,x2 are, according to the canonical ordering in X:" << endl;
    cout << "SX:  x0:  x1:  x2:" << endl;
    for( State SX(X); SX.valid(); SX++ )
        cout << setw(2) << SX << "    " << SX(x0) << "    " << SX(x1) << "    " << SX(x2) << endl;
    cout << endl;

    // The main functionality of the Permute object is to calculate the induced permutation of linear indices of joint states
    cout << "The permutation sigma induces the following permutation of linear indices of joint states:" << endl;
    cout << "SV:  SX:" << endl;
    for( size_t li = 0; li < X.nrStates(); li++ )
        cout << setw(2) << li << "   " << setw(2) << sigma.convertLinearIndex( li ) << endl;

    return 0;
}
