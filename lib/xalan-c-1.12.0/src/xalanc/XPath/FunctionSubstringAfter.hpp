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
#if !defined(FUNCTIONSUBSTRINGAFTER_HEADER_GUARD_1357924680)
#define FUNCTIONSUBSTRINGAFTER_HEADER_GUARD_1357924680



// Base header file.  Must be first.
#include <xalanc/XPath/XPathDefinitions.hpp>



// Base class header file...
#include <xalanc/XPath/Function.hpp>



namespace XALAN_CPP_NAMESPACE {



/**
 * XPath implementation of "substring-after" function.
 */
class XALAN_XPATH_EXPORT FunctionSubstringAfter : public Function
{
public:

    typedef Function    ParentType;

    FunctionSubstringAfter();

    virtual
    ~FunctionSubstringAfter();

    // These methods are inherited from Function ...

    virtual XObjectPtr
    execute(
            XPathExecutionContext&  executionContext,
            XalanNode*              context,
            const XObjectPtr        arg1,
            const XObjectPtr        arg2,
            const Locator*          locator) const;

    using ParentType::execute;

    virtual FunctionSubstringAfter*
    clone(MemoryManager& theManager) const;

protected:

    virtual const XalanDOMString&
    getError(XalanDOMString& theResult) const;

private:

    // Not implemented...
    FunctionSubstringAfter&
    operator=(const FunctionSubstringAfter&);

    bool
    operator==(const FunctionSubstringAfter&) const;
};



}



#endif  // FUNCTIONSUBSTRINGAFTER_HEADER_GUARD_1357924680
