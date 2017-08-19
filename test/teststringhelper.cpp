// Copyright Hugh Perkins 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>

#include "stringhelper.h"

#include "gtest/gtest.h"
#include "test/gtest_supp.h"

using namespace std;

TEST( teststringhelper, split ) {
    wstring mystring = L"MP10";
    vector<wstring> splitString = split( mystring, L"MP" );
    EXPECT_EQ( L"", splitString[0] );
    EXPECT_EQ( L"10", splitString[1] );
}

TEST( teststringhelper, split2 ) {
    wstring mystring = L"MP10MPMP54";
    vector<wstring> splitString = split( mystring, L"MP" );
    EXPECT_EQ( L"", splitString[0] );
    EXPECT_EQ( L"10", splitString[1] );
    EXPECT_EQ( L"", splitString[2] );
    EXPECT_EQ( L"54", splitString[3] );
}

TEST( teststringhelper, split3 ) {
    wstring mystring = L"42MP10MPMP54";
    vector<wstring> splitString = split( mystring, L"MP" );
    EXPECT_EQ( L"42", splitString[0] );
    EXPECT_EQ( L"10", splitString[1] );
    EXPECT_EQ( L"", splitString[2] );
    EXPECT_EQ( L"54", splitString[3] );
}

TEST( teststringhelper, tolower ) {
    wstring mystring = L"3fAfef4FAD";
    EXPECT_EQ( L"3fafef4fad", toLower( mystring ) );
}

TEST( teststringhelper, replace ) {
    wstring mystring = L"hellonewworld";
    EXPECT_EQ( L"hellocoolworld", replace( mystring, L"new", L"cool" ) );
}

TEST( teststringhelper, replaceglobal ) {
    wstring mystring = L"";
    mystring = L"hello world";
    EXPECT_EQ( L"one world", replaceGlobal( mystring, L"hello", L"one" ) );

    mystring = L"hello hello";
    EXPECT_EQ( L"one one", replaceGlobal( mystring, L"hello", L"one" ) );

    mystring = L"hello hello hello";
    EXPECT_EQ( L"one one one", replaceGlobal( mystring, L"hello", L"one" ) );

    mystring = L"hellohellohello";
    EXPECT_EQ( L"oneoneone", replaceGlobal( mystring, L"hello", L"one" ) );

    mystring = L"hellonewwohellorldhellohellohello";
    EXPECT_EQ( L"onenewwoonerldoneoneone", replaceGlobal( mystring, L"hello", L"one" ) );
}

TEST( teststringhelper, strcpy_safe ) {
    wchar_t const*source = L"hello123";
    wchar_t dest[1024];
    memset( dest, 99, 1024 ); // ie, not zero :-)
    strcpy_safe( dest, source, 100 );
    wstring target = wstring(dest);
    EXPECT_EQ( L"hello123", target );
    EXPECT_EQ( 0, dest[8] );
    EXPECT_EQ( 99, dest[9] );
    EXPECT_EQ( '3', dest[7] );

    memset( dest, 99, 1024 ); // ie, not zero :-)
    strcpy_safe( dest, source, 3 );
    target = wstring(dest);
    EXPECT_EQ( L"hel", target );
    EXPECT_EQ( 0, dest[3] );
    EXPECT_EQ( 99, dest[4] );
    EXPECT_EQ( 'l', dest[2] );
}

