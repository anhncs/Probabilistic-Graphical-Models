/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <boost/program_options.hpp>
#include <dai/factorgraph.h>
#include <dai/weightedgraph.h>
#include <dai/util.h>
#include <dai/graph.h>
#include <dai/enum.h>
#include <dai/properties.h>


using namespace std;
using namespace dai;
namespace po = boost::program_options;


/// Possible factor types
DAI_ENUM(FactorType,ISINGGAUSS,ISINGUNIFORM,EXPGAUSS,POTTS);


/// Creates a factor graph from a pairwise interactions graph
/** \param G  Graph describing interactions between variables
 *  \param ft Type of factors to use for interactions
 *  \param states Number of states of the variables
 *  \param props Additional properties for generating the interactions
 */
FactorGraph createFG( const GraphAL &G, FactorType ft, size_t states, const PropertySet &props ) {
    size_t N = G.nrNodes();

    if( states > 2 )
        DAI_ASSERT( ft != FactorType::ISINGGAUSS && ft != FactorType::ISINGUNIFORM );

    // Get inverse temperature
    Real beta = 1.0;
    if( ft != FactorType::ISINGGAUSS && ft != FactorType::ISINGUNIFORM ) 
        beta = props.getAs<Real>("beta");

    // Get properties for Ising factors
    Real mean_h = 0.0;
    Real sigma_h = 0.0;
    Real mean_J = 0.0;
    Real sigma_J = 0.0;
    if( ft == FactorType::ISINGGAUSS ) {
        mean_h = props.getAs<Real>("mean_th");
        sigma_h = props.getAs<Real>("sigma_th");
        mean_J = props.getAs<Real>("mean_w");
        sigma_J = props.getAs<Real>("sigma_w");
    }
    Real min_h = 0.0;
    Real min_J = 0.0;
    Real max_h = 0.0;
    Real max_J = 0.0;
    if( ft == FactorType::ISINGUNIFORM ) {
        min_h = props.getAs<Real>("min_th");
        min_J = props.getAs<Real>("min_w");
        max_h = props.getAs<Real>("max_th");
        max_J = props.getAs<Real>("max_w");
    }
    
    // Create variables
    vector<Var> vars;
    vars.reserve( N );
    for( size_t i = 0; i < N; i++ )
        vars.push_back( Var( i, states ) );

    // Create factors
    vector<Factor> factors;
    factors.reserve( G.nrEdges() + N );
    // Pairwise factors
    for( size_t i = 0; i < N; i++ )
        bforeach( const Neighbor &j, G.nb(i) )
            if( i < j ) {
                if( ft == FactorType::POTTS )
                    factors.push_back( createFactorPotts( vars[i], vars[j], beta ) );
                else if( ft == FactorType::EXPGAUSS )
                    factors.push_back( createFactorExpGauss( VarSet( vars[i], vars[j] ), beta ) );
                else if( ft == FactorType::ISINGGAUSS ) {
                    Real J = rnd_stdnormal() * sigma_J + mean_J;
                    factors.push_back( createFactorIsing( vars[i], vars[j], J ) );
                } else if( ft == FactorType::ISINGUNIFORM ) {
                    Real J = min_J + rnd_uniform() * (max_J - min_J);
                    factors.push_back( createFactorIsing( vars[i], vars[j], J ) );
                }
            }
    // Unary factors
    if( ft == FactorType::ISINGGAUSS )
        for( size_t i = 0; i < N; i++ ) {
            Real h = rnd_stdnormal() * sigma_h + mean_h;
            factors.push_back( createFactorIsing( vars[i], h ) );
        }
    else if( ft == FactorType::ISINGUNIFORM )
        for( size_t i = 0; i < N; i++ ) {
            Real h = min_h + rnd_uniform() * (max_h - min_h);
            factors.push_back( createFactorIsing( vars[i], h ) );
        }

    return FactorGraph( factors.begin(), factors.end(), vars.begin(), vars.end(), factors.size(), vars.size() );
}


/// Return a random factor graph with higher-order interactions
/** \param N number of variables
 *  \param M number of factors
 *  \param k number of variables that each factor depends on
 *  \param beta standard-deviation of Gaussian log-factor entries
 */
FactorGraph createHOIFG( size_t N, size_t M, size_t k, Real beta ) {
    vector<Var> vars;
    vector<Factor> factors;

    vars.reserve(N);
    for( size_t i = 0; i < N; i++ )
        vars.push_back(Var(i,2));

    for( size_t I = 0; I < M; I++ ) {
        VarSet vars;
        while( vars.size() < k ) {
            do {
                size_t newind = (size_t)(N * rnd_uniform());
                Var newvar = Var(newind, 2);
                if( !vars.contains( newvar ) ) {
                    vars |= newvar;
                    break;
                }
            } while( 1 );
        }
        factors.push_back( createFactorExpGauss( vars, beta ) );
    }

    return FactorGraph( factors.begin(), factors.end(), vars.begin(), vars.end(), factors.size(), vars.size() );
}


/// Creates a regular random bipartite graph
/** \param N1 = number of nodes of type 1
 *  \param d1 = size of neighborhoods of nodes of type 1
 *  \param N2 = number of nodes of type 2
 *  \param d2 = size of neighborhoods of nodes of type 2
 *  \note asserts that N1 * d1 == N2 * d2
 */
BipartiteGraph createRandomBipartiteGraph( size_t N1, size_t N2, size_t d1, size_t d2 ) {
    BipartiteGraph G;

    DAI_ASSERT( N1 * d1 == N2 * d2 );

    // build lists of degree-repeated vertex numbers
    std::vector<size_t> stubs1( N1*d1, 0 );
    for( size_t n1 = 0; n1 < N1; n1++ )
        for( size_t t = 0; t < d1; t++ )
            stubs1[n1*d1 + t] = n1;

    // build lists of degree-repeated vertex numbers
    std::vector<size_t> stubs2( N2*d2, 0 );
    for( size_t n2 = 0; n2 < N2; n2++ )
        for( size_t t = 0; t < d2; t++ )
            stubs2[n2*d2 + t] = n2;

    // shuffle lists
    random_shuffle( stubs1.begin(), stubs1.end(), rnd );
    random_shuffle( stubs2.begin(), stubs2.end(), rnd );

    // add edges
    vector<Edge> edges;
    edges.reserve( N1*d1 );
    for( size_t e = 0; e < N1*d1; e++ )
        edges.push_back( Edge(stubs1[e], stubs2[e]) );

    // finish construction
    G.construct( N1, N2, edges.begin(), edges.end() );

    return G;
}


/// Returns x**n % p, assuming p is prime
int powmod (int x, int n, int p) {
    int y = 1;
    for( int m = 0; m < n; m++ )
        y = (x * y) % p;
    return y;
}


/// Returns order of x in GF(p) with p prime
size_t order (int x, int p) {
    x = x % p;
    DAI_ASSERT( x != 0 );
    size_t n = 0;
    size_t prod = 1;
    do {
        prod = (prod * x) % p;
        n++;
    } while( prod != 1 );
    return n;
}


/// Returns whether n is a prime number
bool isPrime (size_t n) {
    bool result = true;
    for( size_t k = 2; (k < n) && result; k++ )
        if( n % k == 0 )
            result = false;
    return result;
}


/// Constructs a regular LDPC graph with N=6, j=2, K=4, k=3
BipartiteGraph createSmallLDPCGraph() {
    BipartiteGraph G;
    size_t N=4, j=3, K=4; // k=3;

    vector<Edge> edges;
    edges.reserve( N*j );
    edges.push_back( Edge(0,0) ); edges.push_back( Edge(1,0) ); edges.push_back( Edge(2,0) );
    edges.push_back( Edge(0,1) ); edges.push_back( Edge(1,1) ); edges.push_back( Edge(3,1) );
    edges.push_back( Edge(0,2) ); edges.push_back( Edge(2,2) ); edges.push_back( Edge(3,2) );
    edges.push_back( Edge(1,3) ); edges.push_back( Edge(2,3) ); edges.push_back( Edge(3,3) );

    // finish construction
    G.construct( N, K, edges.begin(), edges.end() );

    return G;
}


/// Creates group-structured LDPC code
/** Use construction described in "A Class of Group-Structured LDPC Codes"
 *  by R. M. Tanner, D. Sridhara and T. Fuja
 *  Proceedings of ICSTA, 2001
 *
 *  Example parameters: (p,j,k) = (31,3,5)
 *                      (p,j,k) = (37,3,4)
 *                      (p,j,k) = (7,2,4)
 *                      (p,j,k) = (29,2,4)
 *
 *  j and k must be divisors of p-1
 */
BipartiteGraph createGroupStructuredLDPCGraph( size_t p, size_t j, size_t k ) {
    BipartiteGraph G;

    size_t n = j;
    size_t N = p * k;
    size_t K = p * j;

    size_t a, b;
    for( a = 2; a < p; a++ )
        if( order(a,p) == k )
            break;
    DAI_ASSERT( a != p );
    for( b = 2; b < p; b++ )
        if( order(b,p) == j )
            break;
    DAI_ASSERT( b != p );
    // cout << "# order(a=" << a << ") = " << order(a,p) << endl;
    // cout << "# order(b=" << b << ") = " << order(b,p) << endl;

    DAI_ASSERT( N * n == K * k );

    vector<Edge> edges;
    edges.reserve( N * n );

    for( size_t s = 0; s < j; s++ )
        for( size_t t = 0; t < k; t++ ) {
            size_t P = (powmod(b,s,p) * powmod(a,t,p)) % p;
            for( size_t m = 0; m < p; m++ )
                edges.push_back( Edge(t*p + m, s*p + ((m + P) % p)) );
        }

    // finish construction
    G.construct( N, K, edges.begin(), edges.end() );

    return G;
}


// Constructs a parity check table
void createParityCheck( Real *result, size_t n, Real eps ) {
    size_t N = 1 << n;
    for( size_t i = 0; i < N; i++ ) {
        size_t c = 0;
        for( size_t t = 0; t < n; t++ )
            if( i & (1 << t) )
                c ^= 1;
        if( c )
            result[i] = eps;
        else
            result[i] = 1.0 - eps;
    }
    return;
}


/// Predefined names of various factor graph types
const char *FULL_TYPE        = "FULL";
const char *DREG_TYPE        = "DREG";
const char *LOOP_TYPE        = "LOOP";
const char *TREE_TYPE        = "TREE";
const char *GRID_TYPE        = "GRID";
const char *GRID3D_TYPE      = "GRID3D";
const char *HOI_TYPE         = "HOI";
const char *LDPC_TYPE        = "LDPC";


/// Possible LDPC structures
DAI_ENUM(LDPCType,SMALL,GROUP,RANDOM);


/// Main function
int main( int argc, char *argv[] ) {
    try {
        // Variables for storing command line arguments
        size_t seed;
        size_t states = 2;
        string type;
        size_t d, N, K, k, j, n1, n2, n3, prime;
        bool periodic = false;
        FactorType ft;
        LDPCType ldpc;
        Real beta, sigma_w, sigma_th, mean_w, mean_th, min_w, min_th, max_w, max_th, noise;

        // Declare the supported options.
        po::options_description opts("General command line options");
        opts.add_options()
            ("help",     "produce help message")
            ("seed",     po::value<size_t>(&seed),   "random number seed (tries to read from /dev/urandom if not specified)")
            ("states",   po::value<size_t>(&states), "number of states of each variable (default=2 for binary variables)")
        ;

        // Graph structure options
        po::options_description opts_graph("Options for specifying graph structure");
        opts_graph.add_options()
            ("type",     po::value<string>(&type),   "factor graph type (one of 'FULL', 'DREG', 'LOOP', 'TREE', 'GRID', 'GRID3D', 'HOI', 'LDPC')")
            ("d",        po::value<size_t>(&d),      "variable connectivity (only for type=='DREG');\n\t<d><N> should be even")
            ("N",        po::value<size_t>(&N),      "number of variables (not for type=='GRID','GRID3D')")
            ("n1",       po::value<size_t>(&n1),     "width of grid (only for type=='GRID','GRID3D')")
            ("n2",       po::value<size_t>(&n2),     "height of grid (only for type=='GRID','GRID3D')")
            ("n3",       po::value<size_t>(&n3),     "length of grid (only for type=='GRID3D')")
            ("periodic", po::value<bool>(&periodic), "periodic grid? (only for type=='GRID','GRID3D'; default=0)")
            ("K",        po::value<size_t>(&K),      "number of factors (only for type=='HOI','LDPC')")
            ("k",        po::value<size_t>(&k),      "number of variables per factor (only for type=='HOI','LDPC')")
        ;

        // Factor options
        po::options_description opts_factors("Options for specifying factors");
        opts_factors.add_options()
            ("factors",  po::value<FactorType>(&ft), "factor type (one of 'EXPGAUSS','POTTS','ISINGGAUSS','ISINGUNIFORM')")
            ("beta",     po::value<Real>(&beta),     "inverse temperature (ignored for factors=='ISINGGAUSS','ISINGUNIFORM')")
            ("mean_w",   po::value<Real>(&mean_w),   "mean of pairwise interactions w_{ij} (only for factors=='ISINGGAUSS')")
            ("mean_th",  po::value<Real>(&mean_th),  "mean of unary interactions th_i (only for factors=='ISINGGAUSS')")
            ("sigma_w",  po::value<Real>(&sigma_w),  "stddev of pairwise interactions w_{ij} (only for factors=='ISINGGAUSS')")
            ("sigma_th", po::value<Real>(&sigma_th), "stddev of unary interactions th_i (only for factors=='ISINGGAUSS'")
            ("min_w",    po::value<Real>(&min_w),    "minimum of pairwise interactions w_{ij} (only for factors=='ISINGUNIFORM')")
            ("min_th",   po::value<Real>(&min_th),   "minimum of unary interactions th_i (only for factors=='ISINGUNIFORM')")
            ("max_w",    po::value<Real>(&max_w),    "maximum of pairwise interactions w_{ij} (only for factors=='ISINGUNIFORM')")
            ("max_th",   po::value<Real>(&max_th),   "maximum of unary interactions th_i (only for factors=='ISINGUNIFORM')")
        ;

        // LDPC options
        po::options_description opts_ldpc("Options for specifying LDPC code factor graphs");
        opts_ldpc.add_options()
            ("ldpc",     po::value<LDPCType>(&ldpc), "type of LDPC code (one of 'SMALL','GROUP','RANDOM')")
            ("j",        po::value<size_t>(&j),      "number of parity checks per bit (only for type=='LDPC')")
            ("noise",    po::value<Real>(&noise),    "bitflip probability for binary symmetric channel (only for type=='LDPC')")
            ("prime",    po::value<size_t>(&prime),  "prime number for construction of LDPC code (only for type=='LDPC' with ldpc='GROUP'))")
        ;

        // All options
        opts.add(opts_graph).add(opts_factors).add(opts_ldpc);

        // Parse command line arguments
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, opts), vm);
        po::notify(vm);

        // Display help message if necessary
        if( vm.count("help") || !vm.count("type") ) {
            cout << "This program is part of libDAI - http://www.libdai.org/" << endl << endl;
            cout << "Usage: ./createfg [options]" << endl << endl;
            cout << "Creates a factor graph according to the specified options." << endl << endl;
            
            cout << endl << opts << endl;
            
            cout << "The following factor graph types with pairwise interactions can be created:" << endl;
            cout << "\t'FULL':   fully connected graph of <N> variables" << endl;
            cout << "\t'DREG':   random regular graph of <N> variables where each variable is connected with <d> others" << endl;
            cout << "\t'LOOP':   a single loop of <N> variables" << endl;
            cout << "\t'TREE':   random tree-structured (acyclic, connected) graph of <N> variables" << endl;
            cout << "\t'GRID':   2D grid of <n1>x<n2> variables" << endl;
            cout << "\t'GRID3D': 3D grid of <n1>x<n2>x<n3> variables" << endl;
            cout << "The following higher-order interactions factor graphs can be created:" << endl;
            cout << "\t'HOI':    random factor graph consisting of <N> variables and <K> factors," << endl;
            cout << "\t          each factor being an interaction of <k> variables." << endl;
            cout << "The following LDPC code factor graphs can be created:" << endl;
            cout << "\t'LDPC':   simulates LDPC decoding problem, using an LDPC code of <N> bits and <K>" << endl;
            cout << "\t          parity checks, with <k> bits per check and <j> checks per bit, transmitted" << endl;
            cout << "\t          on a binary symmetric channel with probability <noise> of flipping a bit." << endl;
            cout << "\t          The transmitted codeword has all bits set to zero. The argument 'ldpc'" << endl;
            cout << "\t          determines how the LDPC code is constructed: either using a group structure," << endl;
            cout << "\t          or randomly, or a fixed small code with (N,K,k,j) = (4,4,3,3)." << endl << endl;

            cout << "For all types except type=='LDPC', the factors have to be specified as well." << endl << endl;

            cout << "EXPGAUSS factors (the default) are created by drawing all log-factor entries" << endl;
            cout << "independently from a Gaussian with mean 0 and standard deviation <beta>." << endl << endl;
            
            cout << "In case of pairwise interactions, one can also choose POTTS factors, for which" << endl;
            cout << "the log-factors are simply delta functions multiplied by the strength <beta>." << endl << endl;

            cout << "For pairwise interactions and binary variables, one can also use ISINGGAUSS factors." << endl;
            cout << "Here variables x1...xN are assumed to be +1/-1--valued, and unary interactions" << endl;
            cout << "are of the form exp(th*xi) with th drawn from a Gaussian distribution with mean" << endl;
            cout << "<mean_th> and standard deviation <sigma_th>, and pairwise interactions are of the" << endl;
            cout << "form exp(w*xi*xj) with w drawn from a Gaussian distribution with mean <mean_w>" << endl;
            cout << "and standard deviation <sigma_w>." << endl;
            cout << "Alternatively, one can use ISINGUNIFORM factors: here th is drawn from a uniform" << endl;
            cout << "distribution on [<min_th>, <max_th>), and w is drawn from a uniform distribution" << endl;
            cout << "on [<min_w>, <max_w>)." << endl;
            return 1;
        }

        // Set default number of states
        if( !vm.count("states") )
            states = 2;

        // Set default factor type
        if( !vm.count("factors") )
            ft = FactorType::EXPGAUSS;
        // Check validness of factor type
        if( ft == FactorType::POTTS )
            if( type == HOI_TYPE )
                throw "For factors=='POTTS', interactions should be pairwise (type!='HOI')";
        if( ft == FactorType::ISINGGAUSS )
            if( ((states != 2) || (type == HOI_TYPE)) )
                throw "For factors=='ISINGGAUSS', variables should be binary (states==2) and interactions should be pairwise (type!='HOI')";
        if( ft == FactorType::ISINGUNIFORM )
            if( ((states != 2) || (type == HOI_TYPE)) )
                throw "For factors=='ISINGUNIFORM', variables should be binary (states==2) and interactions should be pairwise (type!='HOI')";

        // Read random seed
        if( !vm.count("seed") ) {
            ifstream infile;
            bool success;
            infile.open( "/dev/urandom" );
            success = infile.is_open();
            if( success ) {
                infile.read( (char *)&seed, sizeof(size_t) / sizeof(char) );
                success = infile.good();
                infile.close();
            }
            if( !success )
                throw "Please specify random number seed.";
        }
        rnd_seed( seed );

        // Set default periodicity
        if( !vm.count("periodic") )
            periodic = false;

        // Store some options in a PropertySet object
        PropertySet options;
        if( vm.count("mean_th") )
            options.set("mean_th", mean_th);
        if( vm.count("sigma_th") )
            options.set("sigma_th", sigma_th);
        if( vm.count("mean_w") )
            options.set("mean_w", mean_w);
        if( vm.count("sigma_w") )
            options.set("sigma_w", sigma_w);
        if( vm.count("beta") )
            options.set("beta", beta);
        if( vm.count("min_w") )
            options.set("min_w", min_w);
        if( vm.count("min_th") )
            options.set("min_th", min_th);
        if( vm.count("max_w") )
            options.set("max_w", max_w);
        if( vm.count("max_th") )
            options.set("max_th", max_th);

        // Output some comments
        cout << "# Factor graph made by " << argv[0] << endl;
        cout << "# type = " << type << endl;
        cout << "# states = " << states << endl;
        cout << "# factor type = " << ft << endl;

        // The factor graph to be constructed
        FactorGraph fg;

#define NEED_ARG(name, desc) do { if(!vm.count(name)) throw "Please specify " desc " with --" name; } while(0);

        if( type == FULL_TYPE || type == DREG_TYPE || type == LOOP_TYPE || type == TREE_TYPE || type == GRID_TYPE || type == GRID3D_TYPE ) {
            // Pairwise interactions

            // Check command line options
            if( type == GRID_TYPE ) {
                NEED_ARG("n1", "width of grid");
                NEED_ARG("n2", "height of grid");
                N = n1 * n2;
            } else if( type == GRID3D_TYPE ) {
                NEED_ARG("n1", "width of grid");
                NEED_ARG("n2", "height of grid");
                NEED_ARG("n3", "depth of grid");
                N = n1 * n2 * n3;
            } else
                NEED_ARG("N", "number of variables");

            if( ft == FactorType::ISINGGAUSS ) {
                NEED_ARG("mean_w", "mean of pairwise interactions");
                NEED_ARG("mean_th", "mean of unary interactions");
                NEED_ARG("sigma_w", "stddev of pairwise interactions");
                NEED_ARG("sigma_th", "stddev of unary interactions");
            } else if( ft == FactorType::ISINGUNIFORM ) {
                NEED_ARG("min_w", "minimum of pairwise interactions");
                NEED_ARG("min_th", "minimum of unary interactions");
                NEED_ARG("max_w", "maximum of pairwise interactions");
                NEED_ARG("max_th", "maximum of unary interactions");
            } else
                NEED_ARG("beta", "stddev of log-factor entries");

            if( type == DREG_TYPE )
                NEED_ARG("d", "connectivity (number of neighboring variables of each variable)");

            // Build pairwise interaction graph
            GraphAL G;
            if( type == FULL_TYPE )
                G = createGraphFull( N );
            else if( type == DREG_TYPE )
                G = createGraphRegular( N, d );
            else if( type == LOOP_TYPE )
                G = createGraphLoop( N );
            else if( type == TREE_TYPE )
                G = createGraphTree( N );
            else if( type == GRID_TYPE )
                G = createGraphGrid( n1, n2, periodic );
            else if( type == GRID3D_TYPE )
                G = createGraphGrid3D( n1, n2, n3, periodic );

            // Construct factor graph from pairwise interaction graph
            fg = createFG( G, ft, states, options );

            // Output some additional comments
            if( type == GRID_TYPE || type == GRID3D_TYPE ) {
                cout << "# n1 = " << n1 << endl;
                cout << "# n2 = " << n2 << endl;
                if( type == GRID3D_TYPE )
                    cout << "# n3 = " << n3 << endl;
            }
            if( type == DREG_TYPE )
                cout << "# d = " << d << endl;
            cout << "# options = " << options << endl;
        } else if( type == HOI_TYPE ) {
            // Higher order interactions

            // Check command line arguments
            NEED_ARG("N", "number of variables");
            NEED_ARG("K", "number of factors");
            NEED_ARG("k", "number of variables per factor");
            NEED_ARG("beta", "stddev of log-factor entries");

            // Create higher-order interactions factor graph
            do {
                fg = createHOIFG( N, K, k, beta );
            } while( !fg.isConnected() );

            // Output some additional comments
            cout << "# K = " << K << endl;
            cout << "# k = " << k << endl;
            cout << "# beta = " << beta << endl;
        } else if( type == LDPC_TYPE ) {
            // LDPC codes

            // Check command line arguments
            NEED_ARG("ldpc", "type of LDPC code");
            NEED_ARG("noise", "bitflip probability for binary symmetric channel");

            // Check more command line arguments (seperately for each LDPC type)
            if( ldpc == LDPCType::RANDOM ) {
                NEED_ARG("N", "number of variables");
                NEED_ARG("K", "number of factors");
                NEED_ARG("k", "number of variables per factor");
                NEED_ARG("j", "number of parity checks per bit");
                if( N * j != K * k )
                    throw "Parameters should satisfy N * j == K * k";
            } else if( ldpc == LDPCType::GROUP ) {
                NEED_ARG("prime", "prime number");
                NEED_ARG("k", "number of variables per factor");
                NEED_ARG("j", "number of parity checks per bit");

                if( !isPrime(prime) )
                    throw "Parameter <prime> should be prime";
                if( !((prime-1) % j == 0 ) )
                    throw "Parameters should satisfy (prime-1) % j == 0";
                if( !((prime-1) % k == 0 ) )
                    throw "Parameters should satisfy (prime-1) % k == 0";

                N = prime * k;
                K = prime * j;
            } else if( ldpc == LDPCType::SMALL ) {
                N = 4;
                K = 4;
                j = 3;
                k = 3;
            }

            // Output some additional comments
            cout << "# N = " << N << endl;
            cout << "# K = " << K << endl;
            cout << "# j = " << j << endl;
            cout << "# k = " << k << endl;
            if( ldpc == LDPCType::GROUP )
                cout << "# prime = " << prime << endl;
            cout << "# noise = " << noise << endl;

            // Construct likelihood and paritycheck factors
            Real likelihood[4] = {1.0 - noise, noise, noise, 1.0 - noise};
            Real *paritycheck = new Real[1 << k];
            createParityCheck(paritycheck, k, 0.0);

            // Create LDPC structure
            BipartiteGraph ldpcG;
            bool regular;
            do {
                if( ldpc == LDPCType::GROUP )
                    ldpcG = createGroupStructuredLDPCGraph( prime, j, k );
                else if( ldpc == LDPCType::RANDOM )
                    ldpcG = createRandomBipartiteGraph( N, K, j, k );
                else if( ldpc == LDPCType::SMALL )
                    ldpcG = createSmallLDPCGraph();

                regular = true;
                for( size_t i = 0; i < N; i++ )
                    if( ldpcG.nb1(i).size() != j )
                        regular = false;
                for( size_t I = 0; I < K; I++ )
                    if( ldpcG.nb2(I).size() != k )
                        regular = false;
            } while( !regular && !ldpcG.isConnected() );

            // Convert to FactorGraph
            vector<Factor> factors;
            for( size_t I = 0; I < K; I++ ) {
                VarSet vs;
                for( size_t _i = 0; _i < k; _i++ ) {
                    size_t i = ldpcG.nb2(I)[_i];
                    vs |= Var( i, 2 );
                }
                factors.push_back( Factor( vs, paritycheck ) );
            }
            delete paritycheck;

            // Generate noise vector
            vector<char> noisebits(N,0);
            size_t bitflips = 0;
            for( size_t i = 0; i < N; i++ ) {
                if( rnd_uniform() < noise ) {
                    noisebits[i] = 1;
                    bitflips++;
                }
            }
            cout << "# bitflips = " << bitflips << endl;

            // Simulate transmission of all-zero codeword
            vector<char> input(N,0);
            vector<char> output(N,0);
            for( size_t i = 0; i < N; i++ )
                output[i] = (input[i] + noisebits[i]) & 1;

            // Add likelihoods
            for( size_t i = 0; i < N; i++ )
               factors.push_back( Factor(Var(i,2), likelihood + output[i]*2) );

            // Construct Factor Graph
            fg = FactorGraph( factors );
        } else
            throw "Invalid type";

        // Output additional comments
        cout << "# N = " << fg.nrVars() << endl;
        cout << "# seed = " << seed << endl;

        // Output factor graph
        cout << fg;
    } catch( const char *e ) {
        /// Display error message
        cerr << "Error: " << e << endl;
        return 1;
    }

    return 0;
}
