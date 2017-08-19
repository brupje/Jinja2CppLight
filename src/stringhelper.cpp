// Copyright Hugh Perkins 2014, 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include <string>
#include <vector>
#include <sstream>
using namespace std;

#include "stringhelper.h"

vector<wstring> split(const wstring &str, const wstring &separator ) {
	vector<wstring> splitstring;
	int start = 0;
	int npos = (int)str.find(separator);
	while (npos != (int)str.npos ) {
		splitstring.push_back( str.substr(start, npos-start) );
		start = npos + (int)separator.length();
		npos = (int)str.find(separator, start);
	}
	splitstring.push_back( str.substr( start ) );
    return splitstring;
}

wstring trim( const wstring &target ) {

   int origlen = (int)target.size();
   int startpos = -1;
   for( int i = 0; i < origlen; i++ ) {
      if( target[i] != ' ' && target[i] != '\r' && target[i] != '\n' ) {
         startpos = i;
         break;
      }
   }
   int endpos = -1;
   for( int i = origlen - 1; i >= 0; i-- ) {
      if( target[i] != ' ' && target[i] != '\r' && target[i] != '\n' ) {
         endpos = i;
         break;
      }      
   }
   if( startpos == -1 || endpos == -1 ) {
      return L"";
   }
   return target.substr(startpos, endpos-startpos + 1 );
}

wstring replace( wstring targetString, wstring oldValue, wstring newValue ) {
    size_t pos = targetString.find( oldValue );
    if( pos == string::npos ) {
        return targetString;
    }
    return targetString.replace( pos, oldValue.length(), newValue );
}
wstring replaceGlobal( wstring targetString, wstring oldValue, wstring newValue ) {
    int pos = 0;
    wstring resultString = L"";
    size_t targetPos = targetString.find( oldValue, pos );
    while( targetPos != string::npos ) {
        wstring preOld = targetString.substr( pos, targetPos - pos );
        resultString += preOld + newValue;
        pos = targetPos + oldValue.length();
        targetPos = targetString.find( oldValue, pos );
    }
    resultString += targetString.substr(pos);
    return resultString;
}

std::wstring toLower(std::wstring in ) {
     int len = static_cast<int>( in.size() );
     wchar_t *buffer = new wchar_t[len + 1];
     for( int i = 0; i < len; i++ ) {
        wchar_t thischar = in[i];
        thischar = tolower(thischar);
        buffer[i] = thischar;
    }
    buffer[len] = 0;
    std::wstring result = std::wstring(buffer);
    delete[] buffer;
    return result;
}

void strcpy_safe( wchar_t *destination, wchar_t const*source, int maxLength ) {
    int i = 0;
    for( i = 0; i < maxLength; i++ ) {
        destination[i] = source[i];
        if( source[i] == 0 ) {
            break;
        }
    }
    destination[i] = 0;
}


