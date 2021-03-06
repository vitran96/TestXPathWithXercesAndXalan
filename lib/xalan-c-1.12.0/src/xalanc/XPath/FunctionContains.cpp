/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the  "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "FunctionContains.hpp"



#include <xalanc/PlatformSupport/XalanMessageLoader.hpp>



#include "XObjectFactory.hpp"



namespace XALAN_CPP_NAMESPACE {



FunctionContains::FunctionContains()
{
}



FunctionContains::~FunctionContains()
{
}



XObjectPtr
FunctionContains::execute(
            XPathExecutionContext&  executionContext,
            XalanNode*              /* context */,          
            const XObjectPtr        arg1,
            const XObjectPtr        arg2,
            const Locator* const    /* locator */) const
{
    assert(arg1.null() == false && arg2.null() == false);   

    const XalanDOMString&   str1 = arg1->str(executionContext);
    const XalanDOMString&   str2 = arg2->str(executionContext);

    bool                    fResult = true;

    // If str2 is empty, then don't bother to check anything.
    if (str2.empty() == false)
    {
        // Is str1 empty?
        if (str1.empty() == true)
        {
            fResult = false;
        }
        else
        {
            // OK, both strings have some data, so look for
            // the index...
            const XalanDOMString::size_type     theIndex = indexOf(str1, str2);

            fResult = theIndex < str1.length() ? true : false;
        }
    }

    return executionContext.getXObjectFactory().createBoolean(fResult);
}



FunctionContains*
FunctionContains::clone(MemoryManager& theManager) const
{
    return XalanCopyConstruct(theManager, *this);
}



const XalanDOMString&
FunctionContains::getError(XalanDOMString& theResult) const
{
    return XalanMessageLoader::getMessage(
                theResult,
                XalanMessages::FunctionTakesTwoArguments_1Param,
                "contains()");
}



}
