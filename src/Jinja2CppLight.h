// Copyright Hugh Perkins 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.

// the intent here is to create a templates library that:
// - is based on Jinja2 syntax
// - doesn't depend on boost, qt, etc ...

// for now, will handle:
// - variable substitution, ie {{myvar}}
// - for loops, ie {% for i in range(myvar) %}

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <locale>

#define VIRTUAL virtual
#define STATIC static

namespace Jinja2CppLight {



class render_error : public std::runtime_error {
public:
    render_error(const std::wstring &msg ) : runtime_error("Error!"), message(msg) {};
    ~render_error() throw(){};

    std::wstring get_message() { return message; }

private:
    std::wstring message;
};

class Value {
public:
    virtual ~Value() {}
    virtual std::wstring render() = 0;
    virtual bool isTrue() const = 0;
};
class IntValue : public Value {
public:
    int value;
    IntValue( int value ) :
        value( value ) {
    }
    virtual std::wstring render() {
        return toString( value );
    }
    bool isTrue() const {
        return value != 0;
    }
};
class FloatValue : public Value {
public:
    float value;
    FloatValue( float value ) :
        value( value ) {
    }
    virtual std::wstring render() {
        return toString( value );
    }
    bool isTrue() const {
        return value != 0.0;
    }
};
class StringValue : public Value {
public:
    std::wstring value;
    StringValue( std::wstring value ) :
        value( value ) {
    }
    virtual std::wstring render() {
        return value;
    }
    bool isTrue() const {
        return !value.empty();
    }
};

class Root;
class ControlSection;

class Template {
public:
    std::wstring sourceCode;

    std::map< std::wstring, Value * > valueByName;
//    std::vector< std::wstring > varNameStack;
    Root *root;

    // [[[cog
    // import cog_addheaders
    // cog_addheaders.add(classname='Template')
    // ]]]
    // generated, using cog:
    Template( std::wstring sourceCode );
    STATIC bool isNumber( std::wstring astring, int *p_value );
    VIRTUAL ~Template();
    Template &setValue( std::wstring name, int value );
    Template &setValue( std::wstring name, float value );
    Template &setValue( std::wstring name, std::wstring value );
    std::wstring render();
    void print(ControlSection *section);
    int eatSection( int pos, ControlSection *controlSection );
    STATIC std::wstring doSubstitutions( std::wstring sourceCode, std::map< std::wstring, Value *> valueByName );

    // [[[end]]]
};

class ControlSection {
public:
    std::vector< ControlSection * >sections;
    virtual std::wstring render( std::map< std::wstring, Value *> &valueByName ) = 0;
    virtual void print() {
        print(L"");
    }
    virtual void print(std::wstring prefix) = 0;
};

class Container : public ControlSection {
public:
//    std::vector< ControlSection * >sections;
    int sourceCodePosStart;
    int sourceCodePosEnd;

//    std::wstring render( std::map< std::wstring, Value *> valueByName );
    virtual void print( std::wstring prefix ) {
        std::wcout << prefix << L"Container ( " << sourceCodePosStart << ", " << sourceCodePosEnd << " ) {" << std::endl;
        for( int i = 0; i < (int)sections.size(); i++ ) {
            sections[i]->print( prefix + L"    " );
        }
        std::wcout << prefix << L"}" << std::endl;
    }
};

class ForSection : public ControlSection {
public:
    int loopStart;
    int loopEnd;
    std::wstring varName;
    int startPos;
    int endPos;
    std::wstring render( std::map< std::wstring, Value *> &valueByName ) {
        std::wstring result = L"";
//        bool nameExistsBefore = false;
        if( valueByName.find( varName ) != valueByName.end() ) {
            throw render_error(L"variable " + varName + L" already exists in this context" );
        }
        for( int i = loopStart; i < loopEnd; i++ ) {
            valueByName[varName] = new IntValue( i );
            for( size_t j = 0; j < sections.size(); j++ ) {
                result += sections[j]->render( valueByName );
            }
            delete valueByName[varName];
            valueByName.erase( varName );
        }
        return result;
    }
    //Container *contents;
    virtual void print( std::wstring prefix ) {
        std::wcout << prefix << L"For ( " << varName << L" in range(" << loopStart << L", " << loopEnd << L" ) {" << std::endl;
        for( int i = 0; i < (int)sections.size(); i++ ) {
            sections[i]->print( prefix + L"    " );
        }
        std::wcout << prefix << L"}" << std::endl;
    }
};

class Code : public ControlSection {
public:
//    vector< ControlSection * >sections;
    int startPos;
    int endPos;
    std::wstring templateCode;

    std::wstring render();
    virtual void print( std::wstring prefix ) {
        std::wcout << prefix << L"Code ( " << startPos << L", " << endPos << L" ) {" << std::endl;
        for( int i = 0; i < (int)sections.size(); i++ ) {
            sections[i]->print( prefix + L"    " );
        }
        std::wcout << prefix << L"}" << std::endl;
    }
    virtual std::wstring render( std::map< std::wstring, Value *> &valueByName ) {
//        std::wstring templateString = sourceCode.substr( startPos, endPos - startPos );
//        std::wcout << "Code section, rendering [" << templateCode << "]" << std::endl;
        std::wstring processed = Template::doSubstitutions( templateCode, valueByName );
//        std::wcout << "Code section, after rendering: [" << processed << "]" << std::endl;
        return processed;
    }
};

class Root : public ControlSection {
public:
    virtual ~Root() {}
//    std::vector< ControlSection * >sections;
    virtual std::wstring render( std::map< std::wstring, Value *> &valueByName ) {
        std::wstring resultString = L"";
        for( int i = 0; i < (int)sections.size(); i++ ) {
            resultString += sections[i]->render( valueByName );
        }
        return resultString;
    }
    virtual void print(std::wstring prefix) {
        std::wcout << prefix << L"Root {" << std::endl;
        for( int i = 0; i < (int)sections.size(); i++ ) {
            sections[i]->print( prefix + L"    " );
        }
        std::wcout << prefix << L"}" << std::endl;
    }
};

class IfSection : public ControlSection {
public:
    IfSection(const std::wstring& expression) {
        parseIfCondition(expression);
    }

    std::wstring render(std::map< std::wstring, Value *> &valueByName) {
        std::wstringstream ss;
        const bool expressionValue = computeExpression(valueByName);
        if (expressionValue) {
            for (size_t j = 0; j < sections.size(); j++) {
                ss << sections[j]->render(valueByName);
            }
        }
        const std::wstring renderResult = ss.str();
        return renderResult;
    }

    void print(std::wstring prefix) {
        std::wcout << prefix << L"if ( "
            << ((m_isNegation) ? L"not " : L"")
            << m_variableName << L" ) {" << std::endl;
        if (true) {
            for (int i = 0; i < (int)sections.size(); i++) {
                sections[i]->print(prefix + L"    ");
            }
        }
        std::wcout << prefix << L"}" << std::endl;
    }

private:
    //? It determines m_isNegation and m_variableName from @param[in] expression.
    //? @param[in] expression E.g. "if not myVariable" where myVariable is set by myTemplate.setValue( "myVariable", <any_value> );
    //?                       The result of this statement is false if myVariable is initialized.
    void parseIfCondition(const std::wstring& expression);

    bool computeExpression(const std::map< std::wstring, Value *> &valueByName) const;

    bool m_isNegation; ///< Tells whether is there "if not" or just "if" at the begin of expression.
    std::wstring m_variableName; ///< This simple "if" implementation allows single variable condition only.
};

}
