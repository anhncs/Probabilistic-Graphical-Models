/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <iostream>
#include <fstream>
#include <ostream>
#include <cstdlib>
#include <dai/alldai.h>
#include <dai/io.h>


using namespace std;
using namespace dai;


// Type for storing a joint state of all variables
typedef std::vector<size_t> stateVec;


struct PRbest {
    Real value;
    Real maxdiff;
    bool ready;
    PRbest() : value(0.0), maxdiff(INFINITY), ready(false) {}
};

struct MARbest {
    vector<Factor> beliefs;
    Real maxdiff;
    bool ready;
    MARbest() : beliefs(), maxdiff(INFINITY), ready(false) {}
};

struct MPEbest {
    Real value;
    stateVec state;
    bool ready;
    MPEbest() : value(-INFINITY), state(), ready(false) {}
};


int main( int argc, char *argv[] ) {
    if ( argc != 5 ) {
        cout << "This program is part of libDAI - http://www.libdai.org/" << endl << endl;
        cout << "It is one of the winning solvers that participated in the" << endl;
        cout << "UAI 2010 Approximate Inference Challenge" << endl;
        cout << "(see http://www.cs.huji.ac.il/project/UAI10/)" << endl << endl;
        cout << "Usage: " << argv[0] << " <filename.uai> <filename.uai.evid> <seed> <task>" << endl << endl;
        return 1;
    } else {
        double starttic = toc();

        size_t verbose = 1;    // verbosity
        size_t ia_verbose = 0; // verbosity of inference algorithms
        bool surgery = true;   // change factor graph structure based on evidence
        if( verbose )
            cout << "Solver:               " << argv[0] << endl;

        // set random seed
        size_t seed = fromString<size_t>( argv[3] );
        rnd_seed( seed );
        if( verbose )
            cout << "Seed:                 " << seed << endl;

        // check whether the task is valid
        string task( argv[4] );
        if( task != string("PR") && task != string("MPE") && task != string("MAR") )
            DAI_THROWE(RUNTIME_ERROR,"Unknown task");
        if( verbose )
            cout << "Task:                 " << task << endl;

        // output other command line options
        if( verbose ) {
            cout << "Factorgraph filename: " << argv[1] << endl;
            cout << "Evidence filename:    " << argv[2] << endl;
        }

        // get time and memory limits
        char *buf = getenv( "UAI_TIME" );
        double UAI_time = INFINITY;
        if( buf != NULL )
            UAI_time = fromString<double>( buf );
        buf = getenv( "UAI_MEMORY" );
        size_t UAI_memory = 0;
        if( buf != NULL ) {
            UAI_memory = fromString<double>( buf ) * 1024 * 1024 * 1024;
        }
        if( verbose ) {
            cout << "Time limit:           " << UAI_time << endl;
            cout << "Memory limit:         " << UAI_memory << endl;
        }

        // build output file name
        vector<string> pathComponents = tokenizeString( string(argv[1]), true, "/" );
        string outfile = pathComponents.back() + "." + task;
        if( verbose )
            cout << "Output filename:      " << outfile << endl;

        // open output stream
        ofstream os;
        os.open( outfile.c_str() );
        if( !os.is_open() )
            DAI_THROWE(CANNOT_WRITE_FILE,"Cannot write to file " + outfile);
        if( verbose )
            cout << "Opened output stream" << endl;

        // read factor graph
        vector<Var> vars;
        vector<Factor> facs0;
        vector<Permute> permutations;
        if( verbose )
            cout << "Reading factor graph..." << endl;
        ReadUaiAieFactorGraphFile( argv[1], verbose, vars, facs0, permutations );
        if( verbose )
            cout << "Successfully read factor graph" << endl;

        // check if it could be a grid
        bool couldBeGrid = true;
        FactorGraph fg0( facs0.begin(), facs0.end(), vars.begin(), vars.end(), facs0.size(), vars.size() );
        for( size_t i = 0; i < fg0.nrVars(); i++ )
            if( fg0.delta(i).size() > 4 ) {
                couldBeGrid = false;
                break;
            }
        if( couldBeGrid )
            for( size_t I = 0; I < fg0.nrFactors(); I++ )
                if( fg0.factor(I).vars().size() > 2 ) {
                    couldBeGrid = false;
                    break;
                }
        if( verbose ) {
            if( couldBeGrid )
                cout << "This could be a grid!" << endl;
            else
                cout << "This cannot be a grid!" << endl;
        }

        // read evidence
        if( verbose )
            cout << "Reading evidence..." << endl;
        vector<map<size_t,size_t> > evid = ReadUaiAieEvidenceFile( argv[2], verbose );
        if( verbose )
            cout << "Successfully read " << evid.size() << " evidence cases" << endl;

        // write output header
        if( verbose )
            cout << "  Writing header to file..." << endl;
        os << task << endl;

        // construct clamped factor graphs
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
        }

        // variables for storing best results so far
        vector<PRbest> bestPR( evid.size() );
        vector<MARbest> bestMAR( evid.size() );
        vector<MPEbest> bestMPE( evid.size() );
        for( size_t ev = 0; ev < evid.size(); ev++ )
            bestMPE[ev].state = stateVec( fgs[ev].nrVars(), 0 );
        vector<size_t> ev2go;
        ev2go.reserve( evid.size() );
        for( size_t ev = 0; ev < evid.size(); ev++ )
            ev2go.push_back( ev );

        // solve inference problems
        if( verbose )
            cout << "Solving inference problems..." << endl;
        bool first = true;
        size_t nrsolvers = 3;
        vector<size_t> nrsubsolvers( nrsolvers );
        nrsubsolvers[0] = 2;
        nrsubsolvers[1] = 1;
        nrsubsolvers[2] = 1;
        double MPEdamping = 0.49;
        // for each (sub)solver
        for( size_t solver = 0; solver < nrsolvers; solver++ ) {
            if( verbose )
                cout << "  Solver " << solver << endl;

            // for each evidence case
            size_t subsolver = 0;
            for( long _ev = 0; _ev < (long)ev2go.size(); ) {
                bool improved = false;
                size_t ev = ev2go[_ev];
                if( verbose )
                    cout << "    Evidence case " << ev << ", subsolver = " << subsolver << "..." << endl;

                // construct inference algorithm on clamped factor graph
                if( verbose )
                    cout << "      Constructing inference algorithm..." << endl;
                InfAlg *ia = NULL;
                double tic = toc();
                bool failed = false;
                try {
                    // construct
                    if( solver == 0 ) { // the quick one
                        double remtime = (UAI_time - (toc() - starttic)) * 0.9;
                        if( remtime < 1.0 )
                            remtime = 1.0 ;
                        double maxtime = remtime / (ev2go.size() - _ev);
                        if( verbose ) {
                            cout << "      Past time:     " << (toc() - starttic) << endl;
                            cout << "      Remaining time:" << remtime << endl;
                            cout << "      Allotted time: " << maxtime << endl;
                        }
                        string maxtimestr;
                        if( maxtime != INFINITY )
                            maxtimestr = ",maxtime=" + toString(maxtime);
                        // quick and dirty...
                        if( task == "MPE" )
                            ia = newInfAlgFromString( "BP[inference=MAXPROD,updates=SEQRND,logdomain=" + toString(subsolver) + ",tol=1e-9,maxiter=10000" + maxtimestr + ",damping=0.1,verbose=" + toString(ia_verbose) + "]", fgs[ev] );
                        else {
                            if( couldBeGrid )
                                ia = newInfAlgFromString( "HAK[doubleloop=1,clusters=LOOP,init=UNIFORM,loopdepth=4,tol=1e-9,maxiter=10000" + maxtimestr + ",verbose=" + toString(ia_verbose) + "]", fgs[ev] );
                            else
                                ia = newInfAlgFromString( "BP[inference=SUMPROD,updates=SEQRND,logdomain=" + toString(subsolver) + ",tol=1e-9,maxiter=10000" + maxtimestr + ",damping=0.0,verbose=" + toString(ia_verbose) + "]", fgs[ev] );
                        }
                    } else if( solver == 1 ) { // the exact one
                        string maxmemstr;
                        if( UAI_memory != 0 )
                            maxmemstr = ",maxmem=" + toString(UAI_memory);
                        if( task == "MPE" )
                            ia = newInfAlgFromString( "JTREE[inference=MAXPROD,updates=HUGIN" + maxmemstr + ",verbose=" + toString(ia_verbose) + "]", fgs[ev] );
                        else
                            ia = newInfAlgFromString( "JTREE[inference=SUMPROD,updates=HUGIN" + maxmemstr + ",verbose=" + toString(ia_verbose) + "]", fgs[ev] );
                    } else if( solver == 2 ) { // the decent one
                        double remtime = (UAI_time - (toc() - starttic));
                        if( remtime < 1.0 )
                            remtime = 1.0;
                        double maxtime = 0.95 * remtime / (ev2go.size() - _ev);
                        if( verbose ) {
                            cout << "      Past time:     " << (toc() - starttic) << endl;
                            cout << "      Remaining time:" << remtime << endl;
                            cout << "      Allotted time: " << maxtime << endl;
                        }
                        if( task == "MPE" )
                            maxtime /= fgs[ev].nrVars();
                        string maxtimestr;
                        if( maxtime != INFINITY )
                            maxtimestr = ",maxtime=" + toString(maxtime);
                        if( task == "MPE" )
                            ia = newInfAlgFromString( "DECMAP[ianame=BP,iaopts=[inference=MAXPROD,updates=SEQRND,logdomain=1,tol=1e-9,maxiter=10000" + maxtimestr + ",damping=" + toString(MPEdamping) + ",verbose=0],reinit=1,verbose=" + toString(ia_verbose) + "]", fgs[ev] );
                        else {
                            if( couldBeGrid )
                                ia = newInfAlgFromString( "HAK[doubleloop=1,clusters=LOOP,init=UNIFORM,loopdepth=4,tol=1e-9" + maxtimestr + ",maxiter=100000,verbose=" + toString(ia_verbose) + "]", fgs[ev] );
                            else {
                                if( task == "PR" )
                                    ia = newInfAlgFromString( "HAK[doubleloop=1,clusters=MIN,init=UNIFORM,tol=1e-9" + maxtimestr + ",maxiter=100000,verbose=" + toString(ia_verbose) + "]", fgs[ev] );
                                else
                                    ia = newInfAlgFromString( "GIBBS[maxiter=1000000000,burnin=1000,restart=10000000" + maxtimestr + ",verbose=" + toString(ia_verbose) + "]", fgs[ev] );
                            }
                        }
                    }

                    // initialize
                    if( verbose )
                        cout << "      Initializing inference algorithm..." << endl;
                    ia->init();
                    // run
                    if( verbose )
                        cout << "      Running inference algorithm..." << endl;
                    ia->run();
                    if( verbose )
                        cout << "      Inference algorithm finished..." << endl;
                } catch( Exception &e ) {
                    failed = true;
                    if( verbose ) {
                        cout << "      Inference algorithm failed...!" << endl;
                        cout << "      Exception: " << e.what() << endl;
                    }
                }

                if( verbose )
                    cout << "      Used time:             " << toc() - tic << endl;
                if( !failed && verbose ) {
                    try {
                        cout << "      Number of iterations:  " << ia->Iterations() << endl;
                    } catch( Exception &e ) {
                        cout << "      Number of iterations:   N/A" << endl;
                    }
                    try {
                        cout << "      Final maxdiff:         " << ia->maxDiff() << endl;
                    } catch( Exception &e ) {
                        cout << "      Final maxdiff:          N/A" << endl;
                    }
                }

                // update results for inference task
                if( !failed ) {
                    if( task == "PR" ) {
                        PRbest cur;

                        // calculate PR value
                        cur.value = ia->logZ() / dai::log((Real)10.0);

                        // get maxdiff
                        try {
                            cur.maxdiff = ia->maxDiff();
                        } catch( Exception &e ) {
                            cur.maxdiff = 1e-9;
                        }

                        // only update if this run has converged
                        if( ((cur.maxdiff <= 1e-9) || (cur.maxdiff <= bestPR[ev].maxdiff)) && !dai::isnan(cur.value) ) {
                            // if this was exact inference, we are ready
                            if( solver == 1 ) {
                                ev2go.erase( ev2go.begin() + _ev );
                                _ev--;
                                cur.ready = true;
                            }

                            if( verbose )
                                cout << "    Replacing best PR value so far (" << bestPR[ev].value << ") with new value " << cur.value << endl;
                            bestPR[ev] = cur;
                            improved = true;
                        } else {
                            if( verbose )
                                cout << "    Discarding PR value " << cur.value << endl;
                        }
                    } else if( task == "MAR" ) {
                        MARbest cur;

                        // get variable beliefs
                        bool hasnans = false;
                        cur.beliefs.reserve( fgs[ev].nrVars() );
                        for( size_t i = 0; i < fgs[ev].nrVars(); i++ ) {
                            cur.beliefs.push_back( ia->beliefV(i) );
                            if( cur.beliefs.back().hasNaNs() )
                                hasnans = true;
                        }

                        // get maxdiff
                        try {
                            cur.maxdiff = ia->maxDiff();
                        } catch( Exception &e ) {
                            cur.maxdiff = 1e-9;
                        }

                        // only update if this run has converged
                        if( ((cur.maxdiff <= 1e-9) || (cur.maxdiff <= bestMAR[ev].maxdiff)) && !hasnans ) {
                            // if this was exact inference, we are ready
                            if( solver == 1 ) {
                                ev2go.erase( ev2go.begin() + _ev );
                                _ev--;
                                cur.ready = true;
                            }

                            if( verbose )
                                cout << "    Replacing best beliefs so far with new beliefs" << endl;
                            bestMAR[ev] = cur;
                            improved = true;
                        } else {
                            if( verbose )
                                cout << "    Discarding beliefs" << endl;
                        }
                    } else if( task == "MPE" ) {
                        MPEbest cur;

                        // calculate MPE state
                        cur.state = ia->findMaximum();

                        // calculate MPE value
                        cur.value = fgs[ev].logScore( cur.state );

                        // update best MPE state and value
                        if( cur.value > bestMPE[ev].value && !dai::isnan(cur.value) ) {
                            // if this was exact inference, we are ready
                            if( solver == 1 ) {
                                ev2go.erase( ev2go.begin() + _ev );
                                _ev--;
                                cur.ready = true;
                            }

                            if( verbose )
                                cout << "    Replacing best MPE value so far (" << bestMPE[ev].value << ") with new value " << cur.value << endl;
                            bestMPE[ev] = cur;
                            improved = true;
                        } else {
                            if( verbose )
                                cout << "    New MPE value " << cur.value << " not better than best one so far " << bestMPE[ev].value << endl;
                        }
                    }
                }

                // remove inference algorithm
                if( verbose )
                    cout << "    Cleaning up..." << endl;
                if( !failed )
                    delete ia;

                // write current best output to stream
                if( improved ) {
                    if( verbose )
                        cout << "    Writing output..." << endl;
                    if( first )
                        first = false;
                    else
                        os << "-BEGIN-" << endl;
                    os << evid.size() << endl;
                    for( size_t ev = 0; ev < evid.size(); ev++ ) {
                        if( task == "PR" ) {
                            // output probability of evidence
                            os << bestPR[ev].value << endl;
                        } else if( task == "MAR" ) {
                            // output variable marginals
                            os << bestMAR[ev].beliefs.size() << " ";
                            for( size_t i = 0; i < bestMAR[ev].beliefs.size(); i++ ) {
                                os << bestMAR[ev].beliefs[i].nrStates() << " ";
                                for( size_t s = 0; s < bestMAR[ev].beliefs[i].nrStates(); s++ )
                                    os << bestMAR[ev].beliefs[i][s] << " ";
                            }
                            os << endl;
                        } else if( task == "MPE" ) {
                            // output MPE state
                            os << fgs[ev].nrVars() << " ";
                            for( size_t i = 0; i < fgs[ev].nrVars(); i++ )
                                os << bestMPE[ev].state[i] << " ";
                            os << endl;
                        }
                    }
                    os.flush();
                }

                if( verbose )
                    cout << "    Done..." << endl;

                if( !improved )
                    subsolver++;
                if( improved || subsolver >= nrsubsolvers[solver] || couldBeGrid ) {
                    subsolver = 0;
                    _ev++;
                }
            }

            if( task == "MPE" && solver == 2 && (toc() - starttic) < UAI_time && MPEdamping != 0.0 ) {
                MPEdamping /= 2.0;
                solver--;  // repeat this one
            }
            if( ev2go.size() == 0 )
                break;
        }

        // close output file
        if( verbose )
            cout << "Closing output file..." << endl;
        os.close();
        
        if( verbose )
            cout << "Done!" << endl;
    }

    return 0;
}
