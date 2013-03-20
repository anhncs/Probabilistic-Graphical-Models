/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/properties.h>
#include <strstream>


using namespace dai;


#define BOOST_TEST_MODULE PropertiesTest


#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( PropertyTest ) {
    std::stringstream str1, str2, str3, str4, str5, str6, str7, str8;
    std::string s;

    Property p;
    p.first = "key";

    p.second = (int)-5;
    str1 << p;
    str1 >> s;
    BOOST_CHECK_EQUAL( s, "key=-5" );

    p.second = (size_t)5;
    str2 << p;
    str2 >> s;
    BOOST_CHECK_EQUAL( s, "key=5" );

    p.second = std::string("value");
    str3 << p;
    str3 >> s;
    BOOST_CHECK_EQUAL( s, "key=value" );

    p.second = 3.141;
    str4 << p;
    str4 >> s;
    BOOST_CHECK_EQUAL( s, "key=3.141" );

    p.second = (long double)3.141;
    str5 << p;
    str5 >> s;
    BOOST_CHECK_EQUAL( s, "key=3.141" );

    p.second = false;
    str6 << p;
    str6 >> s;
    BOOST_CHECK_EQUAL( s, "key=0" );

    p.second = PropertySet()("prop2",(size_t)5)("prop1",std::string("hi"));
    str7 << p;
    str7 >> s;
    BOOST_CHECK_EQUAL( s, "key=[prop1=hi,prop2=5]" );

    p.second = std::vector<int>();
    BOOST_CHECK_THROW( str7 << p, Exception );
}


BOOST_AUTO_TEST_CASE( ConstructorsTest ) {
    PropertySet x;
    PropertySet y("[key1=val1,key2=5,key3=[key3a=val,key3b=7.0],key4=1.0]");
    PropertySet z = PropertySet()("key1",std::string("val1"))("key2",5)("key3",PropertySet("[key3a=val,key3b=7.0]"))("key4",1.0);

    BOOST_CHECK( !x.hasKey("key") );
    BOOST_CHECK( !y.hasKey("key") );
    BOOST_CHECK( !z.hasKey("key") );
    BOOST_CHECK( y.hasKey("key1") );
    BOOST_CHECK( y.hasKey("key2") );
    BOOST_CHECK( y.hasKey("key3") );
    BOOST_CHECK( !y.hasKey("key3a") );
    BOOST_CHECK( !y.hasKey("key3b") );
    BOOST_CHECK( y.hasKey("key4") );
    BOOST_CHECK( z.hasKey("key1") );
    BOOST_CHECK( z.hasKey("key2") );
    BOOST_CHECK( z.hasKey("key3") );
    BOOST_CHECK( !z.hasKey("key3a") );
    BOOST_CHECK( !z.hasKey("key3b") );
    BOOST_CHECK( z.hasKey("key4") );

    BOOST_CHECK_EQUAL( x.size(), 0 );
    BOOST_CHECK_EQUAL( y.size(), 4 );
    BOOST_CHECK_EQUAL( z.size(), 4 );
    std::set<std::string> keys;
    keys.insert( "key1" );
    keys.insert( "key2" );
    keys.insert( "key3" );
    keys.insert( "key4" );
    BOOST_CHECK( y.keys() == keys );
    BOOST_CHECK( z.keys() == keys );

    BOOST_CHECK_THROW( x.get( "key" ), Exception );
    BOOST_CHECK_THROW( y.get( "key" ), Exception );
    BOOST_CHECK_THROW( z.get( "key" ), Exception );
    BOOST_CHECK_THROW( x.getAs<std::string>( "key" ), Exception );
    BOOST_CHECK_THROW( y.getAs<int>( "key" ), Exception );
    BOOST_CHECK_THROW( z.getAs<double>( "key" ), Exception );
    BOOST_CHECK_THROW( x.getStringAs<int>( "key" ), Exception );
    BOOST_CHECK_THROW( y.getStringAs<int>( "key" ), Exception );
    BOOST_CHECK_THROW( z.getStringAs<int>( "key" ), Exception );

    BOOST_CHECK_EQUAL( boost::any_cast<std::string>(y.get( "key1" )), std::string("val1") );
    BOOST_CHECK_EQUAL( boost::any_cast<std::string>(y.get( "key2" )), std::string("5") );
    BOOST_CHECK_EQUAL( boost::any_cast<std::string>(y.get( "key3" )), std::string("[key3a=val,key3b=7.0]") );
    BOOST_CHECK_EQUAL( boost::any_cast<std::string>(y.get( "key4" )), std::string("1.0") );
    BOOST_CHECK_EQUAL( y.getAs<std::string>( "key1" ), std::string("val1") );
    BOOST_CHECK_EQUAL( y.getAs<std::string>( "key2" ), std::string("5") );
    BOOST_CHECK_EQUAL( y.getAs<std::string>( "key3" ), std::string("[key3a=val,key3b=7.0]") );
    BOOST_CHECK_EQUAL( y.getAs<std::string>( "key4" ), std::string("1.0") );
    BOOST_CHECK_EQUAL( y.getStringAs<std::string>( "key1" ), std::string("val1") );
    BOOST_CHECK_EQUAL( y.getStringAs<size_t>( "key2" ), 5 );
    BOOST_CHECK_EQUAL( y.getStringAs<int>( "key2" ), 5 );
    BOOST_CHECK_EQUAL( y.getStringAs<double>( "key4" ), 1.0 );
    PropertySet key3val = y.getStringAs<PropertySet>( "key3" );
    BOOST_CHECK_EQUAL( key3val.size(), 2 );
    BOOST_CHECK( key3val.hasKey( "key3a" ) );
    BOOST_CHECK( key3val.hasKey( "key3b" ) );
    BOOST_CHECK_EQUAL( key3val.getStringAs<std::string>( "key3a" ), std::string("val") );
    BOOST_CHECK_EQUAL( key3val.getStringAs<double>( "key3b" ), 7.0 );
    BOOST_CHECK_THROW( y.getAs<int>( "key2" ), Exception );
    BOOST_CHECK_THROW( y.getStringAs<int>( "key4" ), Exception );

    BOOST_CHECK_EQUAL( boost::any_cast<std::string>(z.get( "key1" )), std::string("val1") );
    BOOST_CHECK_EQUAL( boost::any_cast<int>(z.get( "key2" )), 5 );
    BOOST_CHECK_EQUAL( boost::any_cast<PropertySet>(z.get( "key3" )).size(), 2 );
    BOOST_CHECK_EQUAL( boost::any_cast<double>(z.get( "key4" )), 1.0 );
    BOOST_CHECK_EQUAL( z.getAs<std::string>( "key1" ), std::string("val1") );
    BOOST_CHECK_EQUAL( z.getAs<int>( "key2" ), 5 );
    BOOST_CHECK_EQUAL( z.getAs<PropertySet>( "key3" ).size(), 2 );
    BOOST_CHECK_EQUAL( z.getAs<double>( "key4" ), 1.0 );
    BOOST_CHECK_EQUAL( z.getStringAs<std::string>( "key1" ), std::string("val1") );
    BOOST_CHECK_EQUAL( z.getStringAs<int>( "key2" ), 5 );
    BOOST_CHECK_EQUAL( z.getStringAs<double>( "key4" ), 1.0 );
    key3val = z.getAs<PropertySet>( "key3" );
    BOOST_CHECK_EQUAL( key3val.size(), 2 );
    BOOST_CHECK( key3val.hasKey( "key3a" ) );
    BOOST_CHECK( key3val.hasKey( "key3b" ) );
    BOOST_CHECK_EQUAL( key3val.getStringAs<std::string>( "key3a" ), std::string("val") );
    BOOST_CHECK_EQUAL( key3val.getStringAs<double>( "key3b" ), 7.0 );
    BOOST_CHECK_THROW( z.getAs<size_t>( "key2" ), Exception );
    BOOST_CHECK_THROW( z.getStringAs<size_t>( "key4" ), Exception );
}


BOOST_AUTO_TEST_CASE( SetTest ) {
    PropertySet x;
    PropertySet y("[key1=val1,key2=5]");

    x.set( "key1", 5 );
    BOOST_CHECK( x.hasKey( "key1" ) );
    BOOST_CHECK( !x.hasKey( "key2" ) );
    BOOST_CHECK_EQUAL( x.getAs<int>( "key1" ), 5 );
    BOOST_CHECK_THROW( x.getAs<double>( "key1" ), Exception );
    BOOST_CHECK_THROW( x.getAs<int>( "key2" ), Exception );

    x.set( y );
    BOOST_CHECK( x.hasKey( "key1" ) );
    BOOST_CHECK( x.hasKey( "key2" ) );
    BOOST_CHECK( !x.hasKey( "key" ) );
    BOOST_CHECK_EQUAL( x.getAs<std::string>( "key1" ), std::string("val1") );
    BOOST_CHECK_EQUAL( x.getAs<std::string>( "key2" ), std::string("5") );
    x.setAsString<int>( "key1", 5 );
    BOOST_CHECK_EQUAL( x.getAs<std::string>( "key1" ), std::string("5") );
    x.convertTo<size_t>( "key1" );
    BOOST_CHECK_EQUAL( x.getAs<size_t>( "key1" ), 5 );
    x.setAsString( "key1", -5 );
    BOOST_CHECK_EQUAL( x.getAs<std::string>( "key1" ), std::string("-5") );
    x.convertTo<int>( "key1" );
    BOOST_CHECK_EQUAL( x.getAs<int>( "key1" ), -5 );
    x.setAsString( "key1", 1.234 );
    BOOST_CHECK_EQUAL( x.getAs<std::string>( "key1" ), std::string("1.234") );
    BOOST_CHECK_THROW( x.convertTo<int>( "key1" ), Exception );
    x.convertTo<double>( "key1" );
    BOOST_CHECK_EQUAL( x.getAs<double>( "key1" ), 1.234 );
    x.setAsString( "key1", "val1");
    BOOST_CHECK_EQUAL( x.getAs<std::string>( "key1" ), std::string("val1") );
    BOOST_CHECK_THROW( x.convertTo<int>( "key1" ), Exception );
    x.convertTo<std::string>( "key1" );
    BOOST_CHECK_EQUAL( x.getAs<std::string>( "key1" ), std::string("val1") );

    BOOST_CHECK_EQUAL( x.erase( "key1" ), 1 );
    BOOST_CHECK( !x.hasKey( "key1" ) );
}


BOOST_AUTO_TEST_CASE( StreamTest ) {
    std::stringstream ss1, ss2, ss3;
    std::string s;

    PropertySet z = PropertySet()("key1",std::string("val1"))("key2",5)("key3",PropertySet("[key3a=val,key3b=7.0]"))("key4",1.0);
    ss1 << z;
    ss1 >> s;
    BOOST_CHECK_EQUAL( s, std::string("[key1=val1,key2=5,key3=[key3a=val,key3b=7.0],key4=1]") );
    PropertySet y;
    ss2 << z;
    ss2 >> y;
    BOOST_CHECK( y.hasKey( "key1" ) );
    BOOST_CHECK( y.hasKey( "key2" ) );
    BOOST_CHECK( y.hasKey( "key3" ) );
    BOOST_CHECK( y.hasKey( "key4" ) );
    BOOST_CHECK_EQUAL( y.getAs<std::string>( "key1" ), std::string("val1") );
    BOOST_CHECK_EQUAL( y.getAs<std::string>( "key2" ), std::string("5") );
    BOOST_CHECK_EQUAL( y.getAs<std::string>( "key3" ), std::string("[key3a=val,key3b=7.0]") );
    BOOST_CHECK_EQUAL( y.getAs<std::string>( "key4" ), std::string("1") );

    z.set( "key5", std::vector<int>() );
    BOOST_CHECK_THROW( ss1 << z, Exception );

    y.clear();
    BOOST_CHECK_EQUAL( y.size(), 0 );
    ss3 << "[]";
    ss3 >> y;
    BOOST_CHECK_EQUAL( y.size(), 0 );
    ss3 << "";
    BOOST_CHECK_THROW( ss3 >> y, Exception );
    ss3 << "[key1 = val1]";
    BOOST_CHECK_THROW( ss3 >> y, Exception );
    ss3 << "[key1=val1";
    BOOST_CHECK_THROW( ss3 >> y, Exception );
}
