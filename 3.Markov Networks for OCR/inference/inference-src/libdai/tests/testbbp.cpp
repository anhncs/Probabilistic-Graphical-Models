/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <iostream>
#include <dai/alldai.h>
#include <dai/bbp.h>


using namespace dai;
using namespace std;


int main( int argc, char *argv[] ) {
    if ( argc != 2 ) {
        cout << "Usage: " << argv[0] << " <filename.fg>" << endl << endl;
        cout << "Reads factor graph <filename.fg> and verifies" << endl;
        cout << "whether BBP works correctly on it." << endl << endl;
        return 1;
    } else {
        // Read FactorGraph from the file specified by the first command line argument
        FactorGraph fg;
        fg.ReadFromFile(argv[1]);

        // Set some constants
        size_t verbose = 0;
        Real   tol = 1.0e-9;
        size_t maxiter = 10000;
        Real   damping = 0.0;

        // Store the constants in a PropertySet object
        PropertySet opts;
        opts.set("verbose",verbose);  // Verbosity (amount of output generated)
        opts.set("tol",tol);          // Tolerance for convergence
        opts.set("maxiter",maxiter);  // Maximum number of iterations
        opts.set("damping",damping);  // Amount of damping applied

        // Construct a BP (belief propagation) object from the FactorGraph fg
        BP bp(fg, opts("updates",string("SEQFIX"))("logdomain",false));
        bp.recordSentMessages = true;
        bp.init();
        bp.run();

        vector<size_t> state( fg.nrVars(), 0 );

        for( size_t t = 0; t < 45; t++ ) {
            BBP::Properties::UpdateType updates;
            switch( t % 5 ) {
                case BBP::Properties::UpdateType::SEQ_FIX:
                    updates = BBP::Properties::UpdateType::SEQ_FIX;
                    break;
                case BBP::Properties::UpdateType::SEQ_MAX:
                    updates = BBP::Properties::UpdateType::SEQ_MAX;
                    break;
                case BBP::Properties::UpdateType::SEQ_BP_REV:
                    updates = BBP::Properties::UpdateType::SEQ_BP_REV;
                    break;
                case BBP::Properties::UpdateType::SEQ_BP_FWD:
                    updates = BBP::Properties::UpdateType::SEQ_BP_FWD;
                    break;
                case BBP::Properties::UpdateType::PAR:
                    updates = BBP::Properties::UpdateType::PAR;
                    break;
            }
            BBPCostFunction cfn;
            switch( (t / 5) % 9 ) {
                case 0:
                    cfn = BBPCostFunction::CFN_GIBBS_B;
                    break;
                case 1:
                    cfn = BBPCostFunction::CFN_GIBBS_B2;
                    break;
                case 2:
                    cfn = BBPCostFunction::CFN_GIBBS_EXP;
                    break;
                case 3:
                    cfn = BBPCostFunction::CFN_GIBBS_B_FACTOR;
                    break;
                case 4:
                    cfn = BBPCostFunction::CFN_GIBBS_B2_FACTOR;
                    break;
                case 5:
                    cfn = BBPCostFunction::CFN_GIBBS_EXP_FACTOR;
                    break;
                case 6:
                    cfn = BBPCostFunction::CFN_VAR_ENT;
                    break;
                case 7:
                    cfn = BBPCostFunction::CFN_FACTOR_ENT;
                    break;
                case 8:
                    cfn = BBPCostFunction::CFN_BETHE_ENT;
                    break;
            }

            Real h = 1e-4;
            Real result = numericBBPTest( bp, &state, opts("updates",updates), cfn, h );
            cout << "result: " << result << ",\tupdates=" << updates << ", cfn=" << cfn << endl;
        }
    }

    return 0;
}
