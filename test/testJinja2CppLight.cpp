// Copyright Hugh Perkins 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "test/gtest_supp.h"

#include "Jinja2CppLight.h"

using namespace std;
using namespace Jinja2CppLight;

TEST( testJinja2CppLight, basicsubstitution ) {
    wstring source = LR"DELIM(
        This is my {{avalue}} template.  It's {{secondvalue}}...
        Today's weather is {{weather}}.
    )DELIM";

    Template mytemplate( source );
    mytemplate.setValue( L"avalue", 3 );
    mytemplate.setValue( L"secondvalue", 12.123f );
    mytemplate.setValue( L"weather", L"rain" );
    wstring result = mytemplate.render();
    wcout << result << endl;
    wstring expectedResult = LR"DELIM(
        This is my 3 template.  It's 12.123...
        Today's weather is rain.
    )DELIM";
    EXPECT_EQ( expectedResult, result );
}
TEST( testSpeedTemplates, namemissing ) {
    wstring source = LR"DELIM(
        This is my {{avalue}} template.
    )DELIM";

    Template mytemplate( source );
    bool threw = false;
    try {
        wstring result = mytemplate.render();
    } catch( render_error &e ) {
        EXPECT_EQ( string("name avalue not defined"), e.what() );
        threw = true;
    }
    EXPECT_EQ( true, threw );
}
TEST( testSpeedTemplates, loop ) {
    wstring source = LR"DELIM(
        {% for i in range(its) %}
            a[{{i}}] = image[{{i}}];
        {% endfor %}
    )DELIM";

    Template mytemplate( source );
    mytemplate.setValue( L"its", 3 );
    wstring result = mytemplate.render();
    wcout << result << endl;
    wstring expectedResult = LR"DELIM(
        
            a[0] = image[0];
        
            a[1] = image[1];
        
            a[2] = image[2];
        
    )DELIM";
    EXPECT_EQ( expectedResult, result );
}

TEST( testSpeedTemplates, nestedloop ) {
    wstring source = LR"DELIM(
{% for i in range(its) %}a[{{i}}] = image[{{i}}];
{% for j in range(2) %}b[{{j}}] = image[{{j}}];
{% endfor %}{% endfor %}
)DELIM";

    Template mytemplate( source );
    mytemplate.setValue( L"its", 3 );
    wstring result = mytemplate.render();
    wcout << L"[" << result << L"]" << endl;
    wstring expectedResult = LR"DELIM(
a[0] = image[0];
b[0] = image[0];
b[1] = image[1];
a[1] = image[1];
b[0] = image[0];
b[1] = image[1];
a[2] = image[2];
b[0] = image[0];
b[1] = image[1];

)DELIM";
    EXPECT_EQ( expectedResult, result );
}

TEST(testSpeedTemplates, ifTrueTest) {
    const std::wstring source = L"abc{% if True %}def{% endif %}ghi";
    Template mytemplate( source );
    const wstring result = mytemplate.render();
    std::wcout << L"[" << result << L"]" << endl;
    const std::wstring expectedResult = L"abcdefghi";

    EXPECT_EQ(expectedResult, result);
}

TEST(testSpeedTemplates, ifFalseTest) {
    const std::wstring source = L"abc{% if False %}def{% endif %}ghi";
    Template mytemplate(source);
    const wstring result = mytemplate.render();
    std::wcout << "[" << result << "]" << endl;
    const std::wstring expectedResult = L"abcghi";

    EXPECT_EQ(expectedResult, result);
}

TEST(testSpeedTemplates, ifNotTrueTest) {
    const std::wstring source = L"abc{% if not True %}def{% endif %}ghi";
    Template mytemplate(source);
    const wstring result = mytemplate.render();
    std::wcout << "[" << result << "]" << endl;
    const std::wstring expectedResult = L"abcghi";

    EXPECT_EQ(expectedResult, result);
}

TEST(testSpeedTemplates, ifNotFalseTest) {
    const std::wstring source = L"abc{% if not False %}def{% endif %}ghi";
    Template mytemplate(source);
    const wstring result = mytemplate.render();
    std::wcout << L"[" << result << L"]" << endl;
    const std::wstring expectedResult = L"abcdefghi";

    EXPECT_EQ(expectedResult, result);
}

TEST(testSpeedTemplates, ifVariableExitsTest) {
    const std::wstring source = L"abc{% if its %}def{% endif %}ghi";
    
    {
        Template mytemplate(source);
        const std::wstring expectedResultNoVariable = L"abcghi";
        const std::wstring result = mytemplate.render();
        EXPECT_EQ(expectedResultNoVariable, result);
    }

    {
        Template mytemplate(source);
        mytemplate.setValue(L"its", 3);
        const std::wstring result = mytemplate.render();
        std::wcout << L"[" << result << L"]" << endl;
        const std::wstring expectedResult = L"abcdefghi";
        EXPECT_EQ(expectedResult, result);
    }
}

TEST(testSpeedTemplates, ifVariableDoesntExitTest) {
    const std::wstring source = L"abc{% if not its %}def{% endif %}ghi";
    
    {
        Template mytemplate(source);
        const std::wstring expectedResultNoVariable = L"abcdefghi";
        const std::wstring result = mytemplate.render();
        EXPECT_EQ(expectedResultNoVariable, result);
    }

    {
        Template mytemplate(source);
        mytemplate.setValue(L"its", 3);
        const std::wstring result = mytemplate.render();
        std::wcout << L"[" << result << L"]" << endl;
        const std::wstring expectedResult = L"abcghi";
        EXPECT_EQ(expectedResult, result);
    }
}

TEST(testSpeedTemplates, ifUnexpectedExpression) {
    const std::wstring source = L"abc{% if its is defined %}def{% endif %}ghi";
    Template myTemplate(source);
    bool threw = false;
    try {
        myTemplate.render();
    }
    catch ( render_error &e) {
        EXPECT_EQ(std::wstring(L"Unexpected expression after variable name: is"), e.get_message());
        threw = true;
    }
    EXPECT_EQ(true, threw);
}

