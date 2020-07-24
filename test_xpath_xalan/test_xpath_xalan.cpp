#include "test_xpath_xalan.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>

#include <xercesc/util/TransService.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

#include <xercesc/dom/impl/DOMImplementationImpl.hpp>
#include <xercesc/util/XMLUni.hpp>

using namespace xercesc;

#include <xalanc/XPath/XalanXPathException.hpp> // ???

#include <xalanc/PlatformSupport/XSLException.hpp> //???

#include <xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp> //???

#include <xalanc/XPath/XObject.hpp> //????
#include <xalanc/XPath/XPathEvaluator.hpp> //????

#include <xalanc/XalanSourceTree/XalanSourceTreeDOMSupport.hpp> //????
#include <xalanc/XalanSourceTree/XalanSourceTreeInit.hpp> //????
#include <xalanc/XalanSourceTree/XalanSourceTreeParserLiaison.hpp> //???

#include <xalanc/XPath/NodeRefListBase.hpp>

//#include <xalanc/XPath/XPath.hpp> // ???
//#include <xalanc/XalanTransformer/XalanTransformer.hpp> // ???
//
//#include <xalanc/Include/PlatformDefinitions.hpp>

//#include <xalanc/XercesParserLiaison/FormatterToXercesDOM.hpp> // ??
//#include <xalanc/XMLSupport/FormatterToXML.hpp> // ???

using namespace xalanc;

#include <iostream>
#include <string>
#include <sstream>

#define TEST_FILE "sample.xml"

class DOMPrintErrorHandler : public DOMErrorHandler
{
public:

    DOMPrintErrorHandler() {};
    ~DOMPrintErrorHandler() {};

    /** @name The error handler interface */
    bool handleError(const DOMError& domError) override;
    void resetErrors() {};

private:
    /* Unimplemented constructors and operators */
    //DOMPrintErrorHandler(const DOMErrorHandler &);
    //void operator=(const DOMErrorHandler &);

};

bool DOMPrintErrorHandler::handleError(const DOMError& domError)
{
    // Display whatever error message passed from the serializer
    if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
        std::cerr << "\nWarning Message: ";
    else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
        std::cerr << "\nError Message: ";
    else
        std::cerr << "\nFatal Message: ";

    char* msg = XMLString::transcode(domError.getMessage());
    std::cerr << msg << std::endl;
    XMLString::release(&msg);

    // Instructs the serializer to continue serialization if possible.
    return true;
}

DOMDocument* ParseFile(const std::string& file);

void TryXalanXPath(std::string fileName, std::string contextNode, std::string xpathExpression);

int main(const int argc, const char argv[])
{
	XMLPlatformUtils::Initialize();
	//XalanTransformer::initialize(); // ???
    XPathEvaluator::initialize();

    //auto domDoc = ParseFile(TEST_FILE);
    TryXalanXPath(TEST_FILE, "/", "//*");

	//XalanTransformer::terminate(); // ???
    XPathEvaluator::terminate();
	XMLPlatformUtils::Terminate();
    //XalanTransformer::ICUCleanUp();
	return 0;
}

DOMDocument* ParseFile(const std::string& file)
{
    XercesDOMParser* parser = new XercesDOMParser;
    parser->setValidationScheme(XercesDOMParser::Val_Never);
    parser->parse(file.c_str());

    return parser->adoptDocument();
}

void TryXalanXPath(std::string fileName, std::string contextNode, std::string xpathExpression)
{
    try
    {
        XalanSourceTreeInit theSourceTreeInit;

        // We'll use these to parse the XML file.
        XalanSourceTreeDOMSupport theDOMSupport;
        XalanSourceTreeParserLiaison theLiaison(theDOMSupport);

        // Hook the two together...
        theDOMSupport.setParserLiaison(&theLiaison);

        const XalanDOMString theFileName(fileName.c_str());

        // Create an input source that represents a local file...
        const LocalFileInputSource theInputSource(theFileName.c_str());

        // Parse the document...
        XalanDocument* const theDocument =
            theLiaison.parseXMLStream(theInputSource);
        assert(theDocument != 0);

        XalanDocumentPrefixResolver thePrefixResolver(theDocument);

        XPathEvaluator  theEvaluator;

        // OK, let's find the context node...
        XalanNode* const theContextNode =
            theEvaluator.selectSingleNode(
                theDOMSupport,
                theDocument,
                XalanDOMString(contextNode.c_str()).c_str(),
                thePrefixResolver
            );

        if (theContextNode == nullptr)
        {
            std::cerr << "Warning -- No nodes matched the location path \""
                << contextNode
                << "\"."
                << std::endl
                << "Execution cannot continue..."
                << std::endl
                << std::endl;
            return;
        }

        // OK, let's evaluate the expression...
        const XObjectPtr theResult(
            theEvaluator.evaluate(
                theDOMSupport,
                theContextNode,
                XalanDOMString(xpathExpression.c_str()).c_str(),
                thePrefixResolver
            )
        );

        assert(theResult.null() == false);
        XalanDOMString result = theResult->str(theEvaluator.getExecutionContext());

        std::stringstream ss;
        ss << result;

        std::string resultStr = ss.str();

        std::cout << "The string value of the result is:\n"
            << result
            << std::endl;

        auto nodeList = &theEvaluator.getExecutionContext().getContextNodeList();

        std::cout << "END" << std::endl;
    }
    catch (const XSLException& theException)
    {
        using xalanc::XalanDOMString;

        XalanDOMString theError;

        std::cerr << "XSL exception: "
            << theException.defaultFormat(theError)
            << std::endl;
    }
    catch (...)
    {
        std::cerr << "Generic exception caught!" << std::endl;
    }
}
