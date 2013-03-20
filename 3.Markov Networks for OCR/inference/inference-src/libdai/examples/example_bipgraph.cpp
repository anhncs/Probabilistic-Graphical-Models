/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/bipgraph.h>

using namespace std;
using namespace dai;

int main() {
    // Create a list of edges
    vector<Edge> edges;
    edges.reserve( 5 );
    edges.push_back( Edge(0, 0) );
    edges.push_back( Edge(1, 0) );
    edges.push_back( Edge(2, 0) );
    edges.push_back( Edge(1, 1) );
    edges.push_back( Edge(2, 1) );

    // Create a bipartite graph with 3 nodes of type 1,
    // 2 nodes of type 2 and edge list edges.
    BipartiteGraph G( 3, 2, edges.begin(), edges.end() );

    // Display some information about G
    cout << "G has " << G.nrNodes1() << " nodes of type 1, " << G.nrNodes2() << " nodes of type 2 and " << G.nrEdges() << " edges." << endl << endl;

    // Iterate over all nodes n1 of type 1
    for( size_t n1 = 0; n1 < G.nrNodes1(); n1++ ) {
        cout << "Node " << n1 << " of type 1 has " << G.nb1(n1).size() << " neighbors:" << endl;
        // Iterate over all neighbors n2 of n1
        bforeach( const Neighbor &n2, G.nb1(n1) ) {
            // The n2.iter'th neighbor of n1 is n2:
            DAI_ASSERT( G.nb1(n1)[n2.iter] == n2 );

            // The n2.dual'th neighbor of n2 is n1:
            DAI_ASSERT( G.nb2(n2)[n2.dual] == n1 );

            // n2 can be used as an abbreviation of n2.node:
            DAI_ASSERT( static_cast<size_t>(n2) == n2.node );

            cout << "  the " << n2.iter << "'th neighbor is node " << n2 << " of type 2" << endl;
        }
        cout << endl;
    }
}
