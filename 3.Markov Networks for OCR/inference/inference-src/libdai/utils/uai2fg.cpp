/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <dai/alldai.h>
#include <dai/io.h>


using namespace std;
using namespace dai;


int main( int argc, char *argv[] ) {
    if ( argc != 4 ) {
        cout << "This program is part of libDAI - http://www.libdai.org/" << endl << endl;
        cout << "Usage: ./uai2fg <basename> <surgery> <verbose>" << endl << endl;
        cout << "Converts input files in the UAI 2006/2008/2010 approximate inference evaluation format" << endl;
        cout << "(see http://graphmod.ics.uci.edu/uai08/ and http://www.cs.huji.ac.il/project/UAI10/)" << endl;
        cout << "to the libDAI factor graph format." << endl << endl;
        cout << "Reads factor graph <basename.uai> and evidence <basename.uai.evid>" << endl;
        cout << "and writes the resulting clamped factor graphs to <basename.X.fg>," << endl;
        cout << "where X=0,1,2,... enumerates the different evidence cases." << endl << endl;
        cout << "If surgery!=0, uses surgery on the factor graph (recommended)," << endl;
        cout << "otherwise, just adds delta factors to the factor graph." << endl;
        return 1;
    } else {
        string basename( argv[1] );
        bool surgery = fromString<size_t>( argv[2] );
        size_t verbose = fromString<size_t>( argv[3] );
        string uainame = basename + ".uai";
        string evidname = uainame + ".evid";

        // output command line options
        if( verbose ) {
            cout << "Factorgraph filename: " << uainame << endl;
            cout << "Evidence filename:    " << evidname << endl;
            cout << "Output basename:      " << basename << endl;
            cout << "Surgery:              " << surgery << endl;
        }

        // read factor graph
        vector<Var> vars;
        vector<Factor> facs0;
        vector<Permute> permutations;
        if( verbose )
            cout << "Reading factor graph..." << endl;
        ReadUaiAieFactorGraphFile( uainame.c_str(), verbose, vars, facs0, permutations );
        if( verbose )
            cout << "Successfully read factor graph" << endl;

        // read evidence
        if( verbose )
            cout << "Reading evidence..." << endl;
        vector<map<size_t,size_t> > evid = ReadUaiAieEvidenceFile( evidname.c_str(), verbose );
        if( verbose )
            cout << "Successfully read " << evid.size() << " evidence cases" << endl;

        // build output file names
        vector<string> fgnames;
        fgnames.reserve( evid.size() );
        for( size_t ev = 0; ev < evid.size(); ev++ )
            fgnames.push_back( basename + '.' + toString(ev) + ".fg" );

        // construct clamped factor graphs which reflect observed evidence cases
        if( verbose )
            cout << "Constructing clamped factor graphs..." << endl;
        vector<FactorGraph> fgs;
        fgs.reserve( evid.size() );
        for( size_t ev = 0; ev < evid.size(); ev++ ) {
            if( verbose )
                cout << "  Evidence case " << ev << "..." << endl;
            // copy vector of factors
            vector<Factor> facs( facs0 );

            // change factor graph to reflect observed evidence
            if( verbose )
                cout << "    Applying evidence..." << endl;
            if( surgery ) {
                // replace factors with clamped variables with slices
                for( size_t I = 0; I < facs.size(); I++ ) {
                    for( map<size_t,size_t>::const_iterator e = evid[ev].begin(); e != evid[ev].end(); e++ ) {
                        if( facs[I].vars() >> vars[e->first] ) {
                            if( verbose >= 2 )
                                cout << "      Clamping " << e->first << " to value " << e->second << " in factor " << I << " = " << facs[I].vars() << endl;
                            facs[I] = facs[I].slice( vars[e->first], e->second );
                            if( verbose >= 2 )
                                cout << "      ...remaining vars: " << facs[I].vars() << endl;
                        }
                    }
                }
                // remove empty factors
                Real logZcorr = 0.0;
                for( vector<Factor>::iterator I = facs.begin(); I != facs.end(); )
                    if( I->vars().size() == 0 ) {
                        logZcorr += std::log( (Real)(*I)[0] );
                        I = facs.erase( I );
                    } else
                        I++;
                // multiply with logZcorr constant
                if( facs.size() == 0 )
                    facs.push_back( Factor( VarSet(), std::exp(logZcorr) ) );
                else
                    facs.front() *= std::exp(logZcorr);
            }
            // add delta factors corresponding to observed variable values
            for( map<size_t,size_t>::const_iterator e = evid[ev].begin(); e != evid[ev].end(); e++ )
                facs.push_back( createFactorDelta( vars[e->first], e->second ) );

            // construct clamped factor graph
            if( verbose )
                cout << "    Constructing factor graph..." << endl;
            fgs.push_back( FactorGraph( facs.begin(), facs.end(), vars.begin(), vars.end(), facs.size(), vars.size() ) );

            // write it to a file
            if( verbose )
                cout << "    Saving factor graph as " << fgnames[ev] << "..." << endl;
            fgs[ev].WriteToFile( fgnames[ev].c_str() );
        }

        if( verbose )
            cout << "Done!" << endl;
    }

    return 0;
}
