// Copyright Hugh Perkins 2014, 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <wchar.h>
// #include "ClConvolveDllExport.h"

class IHasToString {
public:
    virtual std::wstring toString() = 0;
};

//std::wstring toString( IHasToString *val ); // { // not terribly efficient, but works...
//   std::ostringstream myostringstream;
//   myostringstream << val->toString();
//   return myostringstream.str();
//}

template<typename T>
std::wstring toString(T val ) { // not terribly efficient, but works...
   std::wostringstream myostringstream;
   myostringstream << val;
   return myostringstream.str();
}

std::vector<std::wstring> split(const std::wstring &str, const std::wstring &separator = L" " );
std::wstring trim( const std::wstring &target );

inline float atof( std::wstring stringvalue ) {
   return (float)std::wcstof(stringvalue.c_str(),nullptr);
}
inline int atoi( std::wstring stringvalue ) {
   return std::stoi(stringvalue.c_str());
}

// returns empty string if off the end of the number of available tokens
inline std::wstring getToken( std::wstring targetstring, int tokenIndexFromZero, std::wstring separator = L" " ) {
   std::vector<std::wstring> splitstring = split( targetstring, separator );
   if( tokenIndexFromZero < (int)splitstring.size() ) {
      return splitstring[tokenIndexFromZero];
   } else {
      return L"";
   }
}

std::wstring replace( std::wstring targetString, std::wstring oldValue, std::wstring newValue );
std::wstring replaceGlobal( std::wstring targetString, std::wstring oldValue, std::wstring newValue );

std::wstring toLower(std::wstring in );

void strcpy_safe( wchar_t *destination, wchar_t const*source, int maxLength );

