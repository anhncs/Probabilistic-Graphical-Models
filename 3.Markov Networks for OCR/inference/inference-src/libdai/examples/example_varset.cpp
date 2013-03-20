/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/varset.h>
#include <dai/index.h>
#include <iostream>

using namespace std;
using namespace dai;

int main() {
    Var x0(0, 2);   // Define binary variable x0 (with label 0)
    Var x1(1, 3);   // Define ternary variable x1 (with label 1)

    // Define set X = {x0, x1}
    VarSet X; // empty
    X |= x1;  // X = {x1}
    X |= x0;  // X = {x1, x0}
    cout << "X = " << X << endl << endl; // Note that the elements of X are ordered according to their labels

    // Output some information about x0, x1 and X
    cout << "Var " << x0 << " has " << x0.states() << " states (possible values)." << endl;
    cout << "Var " << x1 << " has " << x1.states() << " states." << endl << endl;
    cout << "VarSet " << X << " has " << X.nrStates() << " states (joint assignments of its variables)." << endl << endl;

    cout << "States of VarSets correspond to states of their constituent Vars:" << endl;
    cout << "  state of x0:   state of x1:   (linear) state of X:" << endl;
    for( size_t s1 = 0; s1 < x1.states(); s1++ ) // for all states s1 of x1
        for( size_t s0 = 0; s0 < x0.states(); s0++ ) { // for all states s0 of x0
            // store s0 and s1 in a map "states"
            map<Var,size_t> states;
            states[x0] = s0;
            states[x1] = s1;

            // output states of x0, x1 and corresponding state of X
            cout << "    " << s0 << "              " << s1 << "              " << calcLinearState(X,states) << endl;

            // calcState() is the inverse of calcLinearState()
            DAI_ASSERT( calcState(X, calcLinearState(X, states)) == states );
        }

    cout << endl << "And vice versa:" << endl;
    cout << "  state of x0:   state of x1:   (linear) state of X:" << endl;
    for( size_t S = 0; S < X.nrStates(); S++ ) { // for all (joint) states of X
        // calculate states of x0 and x1 corresponding to state S of X
        map<Var,size_t> states = calcState(X,S);

        // output state of X and corresponding states of x0, x1
        cout << "    " << states[x0] << "              " << states[x1] << "              " << S << endl;

        // calcLinearState() is the inverse of calcState()
        DAI_ASSERT( calcLinearState(X, calcState(X,S)) == S );
    }

    cout << endl << "Iterating over all joint states using the State class:" << endl;
    cout << "  state of x0:   state of x1:   (linear) state of X:   state of X (as a map):" << endl;
    for( State S(X); S.valid(); S++ ) {
        // output state of X and corresponding states of x0, x1
        cout << "    " << S(x0) << "              " << S(x1) << "              " << S << "                      " << S.get() << endl;
    }

    return 0;
}
