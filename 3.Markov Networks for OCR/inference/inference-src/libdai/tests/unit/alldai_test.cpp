/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/alldai.h>
#include <strstream>
#include <fstream>


using namespace dai;


const double tol = 1e-8;


#define BOOST_TEST_MODULE DAIAlgTest


#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( newInfAlgTest ) {
    Var v0( 0, 2 );
    Var v1( 1, 2 );
    Var v2( 2, 2 );
    Var v3( 3, 2 );
    VarSet v01( v0, v1 );
    VarSet v02( v0, v2 );
    VarSet v03( v0, v3 );
    VarSet v12( v1, v2 );
    VarSet v13( v1, v3 );
    VarSet v23( v2, v3 );
    std::vector<Factor> facs;
    facs.push_back( createFactorIsing( v0, v1, 1.0 ) );
    facs.push_back( createFactorIsing( v1, v2, 1.0 ) );
    facs.push_back( createFactorIsing( v2, v3, 1.0 ) );
    facs.push_back( createFactorIsing( v3, v0, 1.0 ) );
    facs.push_back( createFactorIsing( v0, -1.0 ) );
    facs.push_back( createFactorIsing( v1, -1.0 ) );
    facs.push_back( createFactorIsing( v2, -1.0 ) );
    facs.push_back( createFactorIsing( v3, 1.0 ) );
    Factor joint = facs[0] * facs[1] * facs[2] * facs[3] * facs[4] * facs[5] * facs[6] * facs[7];
    FactorGraph fg( facs );
    VarSet vs = v01;

    InfAlg* ia = newInfAlg( "EXACT", fg, PropertySet()("verbose",(size_t)0) );
    ia->init();
    ia->run();
    BOOST_CHECK( dist( ia->belief( v01 ), joint.marginal( vs ), DISTTV ) < tol );
    BOOST_CHECK( dist( calcMarginal( *ia, vs, true ), joint.marginal( vs ), DISTTV ) < tol );
    delete ia;

    ia = newInfAlgFromString( "EXACT[verbose=0]", fg );
    ia->init();
    ia->run();
    BOOST_CHECK( dist( ia->belief( v01 ), joint.marginal( vs ), DISTTV ) < tol );
    BOOST_CHECK( dist( calcMarginal( *ia, vs, true ), joint.marginal( vs ), DISTTV ) < tol );
    delete ia;

    std::map<std::string, std::string> aliases;
    aliases["alias"] = "EXACT[verbose=1]";
    ia = newInfAlgFromString( "alias[verbose=0]", fg, aliases );
    ia->init();
    ia->run();
    BOOST_CHECK( dist( ia->belief( v01 ), joint.marginal( vs ), DISTTV ) < tol );
    BOOST_CHECK( dist( calcMarginal( *ia, vs, true ), joint.marginal( vs ), DISTTV ) < tol );
    delete ia;
}


BOOST_AUTO_TEST_CASE( parseTest ) {
    std::pair<std::string, PropertySet> nameProps = parseNameProperties( "name" );
    BOOST_CHECK_EQUAL( nameProps.first, "name" );
    BOOST_CHECK_EQUAL( nameProps.second.size(), 0 );

    nameProps = parseNameProperties( "name[]" );
    BOOST_CHECK_EQUAL( nameProps.first, "name" );
    BOOST_CHECK_EQUAL( nameProps.second.size(), 0 );

    nameProps = parseNameProperties( "name[key1=value,key2=0.5]" );
    BOOST_CHECK_EQUAL( nameProps.first, "name" );
    BOOST_CHECK_EQUAL( nameProps.second.size(), 2 );
    BOOST_CHECK( nameProps.second.hasKey( "key1" ) );
    BOOST_CHECK( nameProps.second.hasKey( "key2" ) );
    BOOST_CHECK_EQUAL( nameProps.second.getAs<std::string>("key1"), "value" );
    BOOST_CHECK_EQUAL( nameProps.second.getAs<std::string>("key2"), "0.5" );

    std::map<std::string, std::string> aliases;
    aliases["alias"] = "name[key1=other]";

    nameProps = parseNameProperties( "alias", aliases );
    BOOST_CHECK_EQUAL( nameProps.first, "name" );
    BOOST_CHECK_EQUAL( nameProps.second.size(), 1 );
    BOOST_CHECK( nameProps.second.hasKey( "key1" ) );
    BOOST_CHECK_EQUAL( nameProps.second.getAs<std::string>("key1"), "other" );

    nameProps = parseNameProperties( "alias[]", aliases );
    BOOST_CHECK_EQUAL( nameProps.first, "name" );
    BOOST_CHECK_EQUAL( nameProps.second.size(), 1 );
    BOOST_CHECK( nameProps.second.hasKey( "key1" ) );
    BOOST_CHECK_EQUAL( nameProps.second.getAs<std::string>("key1"), "other" );

    nameProps = parseNameProperties( "alias[key1=value,key2=0.5]", aliases );
    BOOST_CHECK_EQUAL( nameProps.first, "name" );
    BOOST_CHECK_EQUAL( nameProps.second.size(), 2 );
    BOOST_CHECK( nameProps.second.hasKey( "key1" ) );
    BOOST_CHECK( nameProps.second.hasKey( "key2" ) );
    BOOST_CHECK_EQUAL( nameProps.second.getAs<std::string>("key1"), "value" );
    BOOST_CHECK_EQUAL( nameProps.second.getAs<std::string>("key2"), "0.5" );
}


BOOST_AUTO_TEST_CASE( readAliasFileTest ) {
    std::ofstream outfile;
    std::string filename( "alldai_test.aliases" );
    outfile.open( filename.c_str() );
    if( outfile.is_open() ) {
        outfile << "alias:\tname[key1=other]" << std::endl;
        outfile.close();
    } else
        DAI_THROWE(CANNOT_WRITE_FILE,"Cannot write to file " + std::string(filename));

    std::map<std::string, std::string> aliases;
    aliases["alias"] = "name[key1=other]";

    std::map<std::string, std::string> aliases2 = readAliasesFile( filename );
    BOOST_CHECK( aliases == aliases2 );
}
