// Copyright Hugh Perkins 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <wchar.h>
#include "stringhelper.h"

#include "Jinja2CppLight.h"

using namespace std;

namespace
{
    const std::wstring JINJA2_TRUE = L"True";
    const std::wstring JINJA2_FALSE = L"False";
    const std::wstring JINJA2_NOT = L"not";
}

namespace Jinja2CppLight {

#undef VIRTUAL
#define VIRTUAL
#undef STATIC
#define STATIC

Template::Template( std::wstring sourceCode ) :
    sourceCode( sourceCode ) {
    root = new Root();
//    cout << "template::Template root: "  << root << endl;
}    

STATIC bool Template::isNumber( std::wstring astring, int *p_value ) {
    wistringstream in( astring );
    int value;
    if( in >> value && in.eof() ) {
        *p_value = value;
        return true;
    }
    return false;
}
VIRTUAL Template::~Template() {
    for( map< wstring, Value * >::iterator it = valueByName.begin(); it != valueByName.end(); it++ ) {
        delete it->second;
    }
    valueByName.clear();
    delete root;
}
Template &Template::setValue( std::wstring name, int value ) {
    IntValue *intValue = new IntValue( value );
    valueByName[ name ] = intValue;
    return *this;
}
Template &Template::setValue( std::wstring name, float value ) {
    FloatValue *floatValue = new FloatValue( value );
    valueByName[ name ] = floatValue;
    return *this;
}
Template &Template::setValue( std::wstring name, std::wstring value ) {
    StringValue *floatValue = new StringValue( value );
    valueByName[ name ] = floatValue;
    return *this;
}
std::wstring Template::render() {
//    cout << "tempalte::render root=" << root << endl;
    size_t finalPos = eatSection(0, root );
    wcout << finalPos << L" vs " << sourceCode.length() << endl;
    if( finalPos != sourceCode.length() ) {
        root->print(L"");
        throw render_error(L"some sourcecode found at end: " + sourceCode.substr( finalPos ) );
    }
//    cout << "tempalte::render root=" << root << endl;
    root->print(L"");
//    cout << "tempalte::render root=" << root << endl;
    return root->render(valueByName);
}

void Template::print(ControlSection *section) {
    section->print(L"");
}

// pos should point to the first character that has sourcecode inside the control section controlSection
// return value should be first character of the control section end part (ie first char of {% endfor %} type bit)
int Template::eatSection( int pos, ControlSection *controlSection ) {
//    int pos = 0;
//    vector<string> tokenStack;
//    string updatedString = "";
    while( true ) {
//        cout << "pos: " << pos << endl;
        size_t controlChangeBegin = sourceCode.find( L"{%", pos );
//        cout << "controlChangeBegin: " << controlChangeBegin << endl;
        if( controlChangeBegin == string::npos ) {
            //updatedString += doSubstitutions( sourceCode.substr( pos ), valueByName );
            Code *code = new Code();
            code->startPos = pos;
            code->endPos = sourceCode.length();
//            code->templateCode = sourceCode.substr( pos, sourceCode.length() - pos );
            code->templateCode = sourceCode.substr( code->startPos, code->endPos - code->startPos );
            controlSection->sections.push_back( code );
            return sourceCode.length();
        } else {
            size_t controlChangeEnd = sourceCode.find( L"%}", controlChangeBegin );
            if( controlChangeEnd == string::npos ) {
                throw render_error( L"control section unterminated: " + sourceCode.substr( controlChangeBegin, 40 ) );
            }
            wstring controlChange = trim( sourceCode.substr( controlChangeBegin + 2, controlChangeEnd - controlChangeBegin - 2 ) );
            vector<wstring> splitControlChange = split( controlChange, L" " );
            if( splitControlChange[0] == L"endfor" || splitControlChange[0] == L"endif") {
                if( splitControlChange.size() != 1 ) {
                    throw render_error(L"control section {% " + controlChange + L" unrecognized" );
                }
                Code *code = new Code();
                code->startPos = pos;
                code->endPos = controlChangeBegin;
                code->templateCode = sourceCode.substr( code->startPos, code->endPos - code->startPos );
                controlSection->sections.push_back( code );
                return controlChangeBegin;
//                if( tokenStack.size() == 0 ) {
//                    throw render_error("control section {% " + controlChange + " unexpected: no current control stack items" );
//                }
//                if( tokenStack[ tokenStack.size() - 1 ] != "for" ) {
//                    throw render_error("control section {% " + controlChange + " unexpected: current last control stack item is: " + tokenStack[ tokenStack.size() - 1 ] );
//                }
//                cout << "token stack old size: " << tokenStack.size() << endl;
//                tokenStack.erase( tokenStack.end() - 1, tokenStack.end() - 1 );
//                string varToRemove = varNameStack[ (int)tokenStack.size() - 1 ];
//                valueByName.erase( varToRemove );
//                varNameStack.erase( tokenStack.end() - 1, tokenStack.end() - 1 );
//                cout << "token stack new size: " << tokenStack.size() << endl;
            } else if( splitControlChange[0] == L"for" ) {
                Code *code = new Code();
                code->startPos = pos;
                code->endPos = controlChangeBegin;
                code->templateCode = sourceCode.substr( code->startPos, code->endPos - code->startPos );
                controlSection->sections.push_back( code );

                wstring varname = splitControlChange[1];
                if( splitControlChange[2] != L"in" ) {
                    throw render_error(L"control section {% " + controlChange + L" unexpected: second word should be 'in'" );
                }
                wstring rangeString = L"";
                for( int i = 3; i < (int)splitControlChange.size(); i++ ) {
                    rangeString += splitControlChange[i];
                }
                rangeString = replaceGlobal( rangeString, L" ", L"" );
                vector<wstring> splitRangeString = split( rangeString, L"(" );
                if( splitRangeString[0] != L"range" ) {
                    throw render_error(L"control section {% " + controlChange + L" unexpected: third word should start with 'range'" );
                }
                if( splitRangeString.size() != 2 ) {
                    throw render_error(L"control section " + controlChange + L" unexpected: should be in format 'range(somevar)' or 'range(somenumber)'" );
                }
                wstring name = split( splitRangeString[1],L")" )[0];
//                cout << "for range name: " << name << endl;
                int endValue;
                if( isNumber( name, &endValue ) ) {
                } else {
                    if( valueByName.find( name ) != valueByName.end() ) {
                        IntValue *intValue = dynamic_cast< IntValue * >( valueByName[ name ] );
                        if( intValue == 0 ) {
                            throw render_error(L"for loop range var " + name + L" must be an int (but it's not)");
                        }
                        endValue = intValue->value;
                    } else {
                        throw render_error(L"for loop range var " + name + L" not recognized");
                    }                    
                }
                int beginValue = 0; // default for now...
//                cout << "for loop start=" << beginValue << " end=" << endValue << endl;
                ForSection *forSection = new ForSection();
                forSection->startPos = controlChangeEnd + 2;
                forSection->loopStart = beginValue;
                forSection->loopEnd = endValue;
                forSection->varName = varname;
                pos = eatSection( controlChangeEnd + 2, forSection );
                controlSection->sections.push_back(forSection);
                size_t controlEndEndPos = sourceCode.find(L"%}", pos );
                if( controlEndEndPos == string::npos ) {
                    throw render_error(L"No control end section found at: " + sourceCode.substr(pos ) );
                }
                wstring controlEnd = sourceCode.substr( pos, controlEndEndPos - pos + 2 );
                wstring controlEndNorm = replaceGlobal( controlEnd, L" ", L"" );
                if( controlEndNorm != L"{%endfor%}" ) {
                    throw render_error(L"No control end section found, expected '{% endfor %}', got '" + controlEnd + L"'" );
                }
                forSection->endPos = controlEndEndPos + 2;
                pos = controlEndEndPos + 2;
//                tokenStack.push_back("for");
//                varNameStack.push_back(name);
            } else if (splitControlChange[0] == L"if") {
                Code *code = new Code();
                code->startPos = pos;
                code->endPos = controlChangeBegin;
                code->templateCode = sourceCode.substr(code->startPos, code->endPos - code->startPos);
                controlSection->sections.push_back(code);
                const wstring word = splitControlChange[1];
                if (JINJA2_TRUE == word)  {
                    ;
                } else if (JINJA2_FALSE == word) {
                    ;
                }
                else if (JINJA2_NOT == word) {
                    ;
                }
                else {
                    ;
                }
                IfSection* ifSection = new IfSection(controlChange);

                pos = eatSection(controlChangeEnd + 2, ifSection);
                controlSection->sections.push_back(ifSection);
                size_t controlEndEndPos = sourceCode.find(L"%}", pos);
                if (controlEndEndPos == string::npos) {
                    throw render_error(L"No control end of any section found at: " + sourceCode.substr(pos));
                }
                wstring controlEnd = sourceCode.substr(pos, controlEndEndPos - pos + 2);
                wstring controlEndNorm = replaceGlobal(controlEnd, L" ", L"");
                if (controlEndNorm != L"{%endif%}") {
                    throw render_error(L"No control end section found, expected '{% endif %}', got '" + controlEnd + L"'");
                }
                //forSection->endPos = controlEndEndPos + 2;
                pos = controlEndEndPos + 2;

            } else {
                throw render_error(L"control section {% " + controlChange + L" unexpected" );
            }
        }
    }

//    vector<string> controlSplit = split( sourceCode, "{%" );
////    int startI = 1;
////    if( controlSplit.substr(0,2) == "{%" ) {
////        startI = 0;
////    }
//    string updatedString = "";
//    for( int i = 0; i < (int)controlSplit.size(); i++ ) {
//        if( controlSplit[i].substr(0,2) == "{%" ) {
//            vector<string> splitControlPair = split(controlSplit[i], "%}" );
//            string controlString = splitControlPair[0];
//        } else {
//            updatedString += doSubstitutions( controlSplit[i], valueByName );
//        }
//    }
////    string templatedString = doSubstitutions( sourceCode, valueByName );
//    return updatedString;
}
STATIC std::wstring Template::doSubstitutions( std::wstring sourceCode, std::map< std::wstring, Value *> valueByName ) {
    int startI = 1;
    if( sourceCode.substr(0,2) == L"{{" ) {
        startI = 0;
    }
    wstring templatedString = L"";
    vector<wstring> splitSource = split( sourceCode, L"{{" );
    if( startI == 1 ) {
        templatedString = splitSource[0];
    }
    for( size_t i = startI; i < splitSource.size(); i++ ) {
        if (0 == i && splitSource.size() > 1 && splitSource[i].empty()) // Ignoring initial empty section if there are other sections.
        {
            continue;
        }

        vector<wstring> thisSplit = split( splitSource[i], L"}}" );
        wstring name = trim( thisSplit[0] );
//        cout << "name: " << name << endl;
        if( valueByName.find( name ) == valueByName.end() ) {
            throw render_error( L"name " + name + L" not defined" );
        }
        Value *value = valueByName[ name ];
        templatedString += value->render();
        if( thisSplit.size() > 0 ) {
            templatedString += thisSplit[1];
        }
    }
    return templatedString;
}

void IfSection::parseIfCondition(const std::wstring& expression) {
    const std::vector<std::wstring> splittedExpression = split(expression, L" ");
    if (splittedExpression.empty() || splittedExpression[0] != L"if") {
        throw render_error(L"if statement expected.");
    }

    std::size_t expressionIndex = 1;
    if (splittedExpression.size() < expressionIndex + 1) {
        throw render_error(L"Any expression expected after if statement.");
    }
    m_isNegation = (JINJA2_NOT == splittedExpression[expressionIndex]);
    expressionIndex += (m_isNegation) ? 1 : 0;
    if (splittedExpression.size() < expressionIndex + 1) {
        if (!m_isNegation)
            throw render_error(L"Any expression expected after if statement.");
        else
            throw render_error(L"Any expression expected after if not statement.");
    }
    m_variableName = splittedExpression[expressionIndex];
    if (splittedExpression.size() > expressionIndex + 1) {
        throw render_error(std::wstring(L"Unexpected expression after variable name: ") + splittedExpression[expressionIndex + 1]);
    }
}

bool IfSection::computeExpression(const std::map< std::wstring, Value *> &valueByName) const {
    if (JINJA2_TRUE == m_variableName) {
        return true ^ m_isNegation;
    }
    else if (JINJA2_FALSE == m_variableName) {
        return false ^ m_isNegation;
    }
    else {
        const bool valueExists = valueByName.count(m_variableName) > 0;
        if (!valueExists) {
            return false ^ m_isNegation;
        }
        return valueByName.at(m_variableName)->isTrue() ^ m_isNegation;
    }
}

}


