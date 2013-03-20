/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <iostream>
#include <map>
#include <dai/alldai.h>  // Include main libDAI header file
#include <dai/jtree.h>
#include <dai/bp.h>
#include <dai/decmap.h>


using namespace dai;
using namespace std;


int main( int argc, char *argv[] ) {
    if ( argc != 2 && argc != 3 ) {
        cout << "Usage: " << argv[0] << " <filename.fg> [maxstates]" << endl << endl;
        cout << "Reads factor graph <filename.fg> and runs" << endl;
        cout << "Belief Propagation, Max-Product and JunctionTree on it." << endl;
        cout << "JunctionTree is only run if a junction tree is found with" << endl;
        cout << "total number of states less than <maxstates> (where 0 means unlimited)." << endl << endl;
        return 1;
    } else {
        // Report inference algorithms built into libDAI
        cout << "Builtin inference algorithms: " << builtinInfAlgNames() << endl << endl;

        // Read FactorGraph from the file specified by the first command line argument
        FactorGraph fg;
        fg.ReadFromFile(argv[1]);
        size_t maxstates = 1000000;
        if( argc == 3 )
            maxstates = fromString<size_t>( argv[2] );

        // Set some constants
        size_t maxiter = 10000;
        Real   tol = 1e-9;
        size_t verb = 1;

        // Store the constants in a PropertySet object
        PropertySet opts;
        opts.set("maxiter",maxiter);  // Maximum number of iterations
        opts.set("tol",tol);          // Tolerance for convergence
        opts.set("verbose",verb);     // Verbosity (amount of output generated)

        // Bound treewidth for junctiontree
        bool do_jt = true;
        try {
            boundTreewidth(fg, &eliminationCost_MinFill, maxstates );
        } catch( Exception &e ) {
            if( e.getCode() == Exception::OUT_OF_MEMORY ) {
                do_jt = false;
                cout << "Skipping junction tree (need more than " << maxstates << " states)." << endl;
            }
            else
                throw;
        }

        JTree jt, jtmap;
        vector<size_t> jtmapstate;
        if( do_jt ) {
            // Construct a JTree (junction tree) object from the FactorGraph fg
            // using the parameters specified by opts and an additional property
            // that specifies the type of updates the JTree algorithm should perform
            jt = JTree( fg, opts("updates",string("HUGIN")) );
            // Initialize junction tree algorithm
            jt.init();
            // Run junction tree algorithm
            jt.run();

            // Construct another JTree (junction tree) object that is used to calculate
            // the joint configuration of variables that has maximum probability (MAP state)
            jtmap = JTree( fg, opts("updates",string("HUGIN"))("inference",string("MAXPROD")) );
            // Initialize junction tree algorithm
            jtmap.init();
            // Run junction tree algorithm
            jtmap.run();
            // Calculate joint state of all variables that has maximum probability
            jtmapstate = jtmap.findMaximum();
        }

        // Construct a BP (belief propagation) object from the FactorGraph fg
        // using the parameters specified by opts and two additional properties,
        // specifying the type of updates the BP algorithm should perform and
        // whether they should be done in the real or in the logdomain
        BP bp(fg, opts("updates",string("SEQRND"))("logdomain",false));
        // Initialize belief propagation algorithm
        bp.init();
        // Run belief propagation algorithm
        bp.run();

        // Construct a BP (belief propagation) object from the FactorGraph fg
        // using the parameters specified by opts and two additional properties,
        // specifying the type of updates the BP algorithm should perform and
        // whether they should be done in the real or in the logdomain
        //
        // Note that inference is set to MAXPROD, which means that the object
        // will perform the max-product algorithm instead of the sum-product algorithm
        BP mp(fg, opts("updates",string("SEQRND"))("logdomain",false)("inference",string("MAXPROD"))("damping",string("0.1")));
        // Initialize max-product algorithm
        mp.init();
        // Run max-product algorithm
        mp.run();
        // Calculate joint state of all variables that has maximum probability
        // based on the max-product result
        vector<size_t> mpstate = mp.findMaximum();

        // Construct a decimation algorithm object from the FactorGraph fg
        // using the parameters specified by opts and three additional properties,
        // specifying that the decimation algorithm should use the max-product
        // algorithm and should completely reinitalize its state at every step
        DecMAP decmap(fg, opts("reinit",true)("ianame",string("BP"))("iaopts",string("[damping=0.1,inference=MAXPROD,logdomain=0,maxiter=1000,tol=1e-9,updates=SEQRND,verbose=1]")) );
        decmap.init();
        decmap.run();
        vector<size_t> decmapstate = decmap.findMaximum();

        if( do_jt ) {
            // Report variable marginals for fg, calculated by the junction tree algorithm
            cout << "Exact variable marginals:" << endl;
            for( size_t i = 0; i < fg.nrVars(); i++ ) // iterate over all variables in fg
                cout << jt.belief(fg.var(i)) << endl; // display the "belief" of jt for that variable
        }

        // Report variable marginals for fg, calculated by the belief propagation algorithm
        cout << "Approximate (loopy belief propagation) variable marginals:" << endl;
        for( size_t i = 0; i < fg.nrVars(); i++ ) // iterate over all variables in fg
            cout << bp.belief(fg.var(i)) << endl; // display the belief of bp for that variable

        if( do_jt ) {
            // Report factor marginals for fg, calculated by the junction tree algorithm
            cout << "Exact factor marginals:" << endl;
            for( size_t I = 0; I < fg.nrFactors(); I++ ) // iterate over all factors in fg
                cout << jt.belief(fg.factor(I).vars()) << endl;  // display the "belief" of jt for the variables in that factor
        }

        // Report factor marginals for fg, calculated by the belief propagation algorithm
        cout << "Approximate (loopy belief propagation) factor marginals:" << endl;
        for( size_t I = 0; I < fg.nrFactors(); I++ ) // iterate over all factors in fg
            cout << bp.belief(fg.factor(I).vars()) << endl; // display the belief of bp for the variables in that factor

        if( do_jt ) {
            // Report log partition sum (normalizing constant) of fg, calculated by the junction tree algorithm
            cout << "Exact log partition sum: " << jt.logZ() << endl;
        }

        // Report log partition sum of fg, approximated by the belief propagation algorithm
        cout << "Approximate (loopy belief propagation) log partition sum: " << bp.logZ() << endl;

        if( do_jt ) {
            // Report exact MAP variable marginals
            cout << "Exact MAP variable marginals:" << endl;
            for( size_t i = 0; i < fg.nrVars(); i++ )
                cout << jtmap.belief(fg.var(i)) << endl;
        }

        // Report max-product variable marginals
        cout << "Approximate (max-product) MAP variable marginals:" << endl;
        for( size_t i = 0; i < fg.nrVars(); i++ )
            cout << mp.belief(fg.var(i)) << endl;

        if( do_jt ) {
            // Report exact MAP factor marginals
            cout << "Exact MAP factor marginals:" << endl;
            for( size_t I = 0; I < fg.nrFactors(); I++ )
                cout << jtmap.belief(fg.factor(I).vars()) << " == " << jtmap.beliefF(I) << endl;
        }

        // Report max-product factor marginals
        cout << "Approximate (max-product) MAP factor marginals:" << endl;
        for( size_t I = 0; I < fg.nrFactors(); I++ )
            cout << mp.belief(fg.factor(I).vars()) << " == " << mp.beliefF(I) << endl;

        if( do_jt ) {
            // Report exact MAP joint state
            cout << "Exact MAP state (log score = " << fg.logScore( jtmapstate ) << "):" << endl;
            for( size_t i = 0; i < jtmapstate.size(); i++ )
                cout << fg.var(i) << ": " << jtmapstate[i] << endl;
        }

        // Report max-product MAP joint state
        cout << "Approximate (max-product) MAP state (log score = " << fg.logScore( mpstate ) << "):" << endl;
        for( size_t i = 0; i < mpstate.size(); i++ )
            cout << fg.var(i) << ": " << mpstate[i] << endl;

        // Report DecMAP joint state
        cout << "Approximate DecMAP state (log score = " << fg.logScore( decmapstate ) << "):" << endl;
        for( size_t i = 0; i < decmapstate.size(); i++ )
            cout << fg.var(i) << ": " << decmapstate[i] << endl;
    }

    return 0;
}
