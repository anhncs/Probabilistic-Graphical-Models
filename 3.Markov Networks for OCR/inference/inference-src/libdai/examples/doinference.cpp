/*  This file was adapted from libDAI's example.cpp, for Probabilistic Graphical Models.
 *
 *  libDAI is licensed under the BSD 2-clause license (also known as the FreeBSD license).
 *  This file is distributed without any warranty. 
 *  See the file COPYING for more details.
 *
 */


#include <iostream>
#include <map>
#include <dai/alldai.h>  // Include main libDAI header files
#include <cstring>

using namespace dai;
using namespace std;


int main( int argc, char *argv[] ) {
    if ( argc != 3 ) {
        cout << "Usage: " << argv[0] << " <filename.fg> [map|pd]" << endl << endl;
        cout << "Reads factor graph <filename.fg> and runs" << endl;
        cout << "map: Junction tree MAP" << endl;
        cout << "pd : LBP and posterior decoding" << endl << endl;
        return 1;
    } else {
        // Redirect cerr to inf.log
        ofstream errlog("inf.log");
        //streambuf* orig_cerr = cerr.rdbuf();
        cerr.rdbuf(errlog.rdbuf());

        // Read FactorGraph from the file specified by the first command line argument
        FactorGraph fg;
        fg.ReadFromFile(argv[1]);

        // Set some constants
        size_t maxiter = 10000;
        Real   tol = 1e-9;
        size_t verb = 1;

        // Store the constants in a PropertySet object
        PropertySet opts;
        opts.set("maxiter",maxiter);  // Maximum number of iterations
        opts.set("tol",tol);          // Tolerance for convergence
        opts.set("verbose",verb);     // Verbosity (amount of output generated)

        if (strcmp(argv[2], "map") == 0) {
            // Construct another JTree (junction tree) object that is used to calculate
            // the joint configuration of variables that has maximum probability (MAP state)
            JTree jtmap( fg, opts("updates",string("HUGIN"))("inference",string("MAXPROD")) );
            // Initialize junction tree algorithm
            jtmap.init();
            // Run junction tree algorithm
            jtmap.run();
            // Calculate joint state of all variables that has maximum probability
            vector<size_t> jtmapstate = jtmap.findMaximum();

            /*
            // Report exact MAP variable marginals
            cout << "Exact MAP variable marginals:" << endl;
            for( size_t i = 0; i < fg.nrVars(); i++ )
                cout << jtmap.belief(fg.var(i)) << endl;
            */

            // Report exact MAP joint state
            cerr << "Exact MAP state (log score = " << fg.logScore( jtmapstate ) << "):" << endl;
            cout << fg.nrVars() << endl;
            for( size_t i = 0; i < jtmapstate.size(); i++ )
                cout << fg.var(i).label() << " " << jtmapstate[i] + 1 << endl; // +1 because in MATLAB assignments start at 1
        } else if (strcmp(argv[2], "pd") == 0) {
        
            // Construct a BP (belief propagation) object from the FactorGraph fg
            // using the parameters specified by opts and two additional properties,
            // specifying the type of updates the BP algorithm should perform and
            // whether they should be done in the real or in the logdomain
            BP bp(fg, opts("updates",string("SEQMAX"))("logdomain",true));
            // Initialize belief propagation algorithm
            bp.init();
            // Run belief propagation algorithm
            bp.run();

            // Report variable marginals for fg, calculated by the belief propagation algorithm
            cerr << "LBP posterior decoding (highest prob assignment in marginal):" << endl;
            cout << fg.nrVars() << endl;
            for( size_t i = 0; i < fg.nrVars(); i++ ) {// iterate over all variables in fg
                //cout << bp.belief(fg.var(i)) << endl; // display the belief of bp for that variable
                Factor marginal = bp.belief(fg.var(i));
                Real maxprob = marginal.max();
                for (size_t j = 0; j < marginal.nrStates(); j++) {
                  if (marginal[j] == maxprob) {
                    cout << fg.var(i).label() << " " << j + 1 << endl; // +1 because in MATLAB assignments start at 1
                  }
                }
            }
        } else {
          cerr << "Invalid inference algorithm specified." << endl;
          return 1;
        }
    }

    return 0;
}
