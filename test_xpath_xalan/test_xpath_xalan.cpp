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

// THESE ARE FOR XALAN ONLY -----------------------------------------
#include <xalanc/XalanSourceTree/XalanSourceTreeDOMSupport.hpp> //????
#include <xalanc/XalanSourceTree/XalanSourceTreeInit.hpp> //????
#include <xalanc/XalanSourceTree/XalanSourceTreeParserLiaison.hpp> //???
// ------------------------------------------------------------------

#include <xalanc/XercesParserLiaison/XercesWrapperHelper.hpp>

#include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>
#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>

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
//XalanDocument* WrapXercesDocument(const DOMDocument* xercesDoc);

//void TryXalanXPath(std::string fileName, std::string contextNode, std::string xpathExpression);

int main(const int argc, const char argv[])
{
    std::string xmlFile(TEST_FILE);
    std::string xpathExpression("/bookstore/book");

	XMLPlatformUtils::Initialize();
    XPathEvaluator::initialize();

    auto xercesDoc = ParseFile(xmlFile);
    //xercesDoc->normalize();
    //auto xalanDoc = WrapXercesDocument(xercesDoc);



    //TryXalanXPath(TEST_FILE, "/", "//*");

    XPathEvaluator::terminate();
	XMLPlatformUtils::Terminate();
	return 0;
}

DOMDocument* ParseFile(const std::string& file)
{
    XercesDOMParser parser;
    parser.setValidationScheme(XercesDOMParser::Val_Never);
    parser.parse(file.c_str());

    return parser.adoptDocument();
}

//XalanDocument* WrapXercesDocument(const DOMDocument* xercesDoc)
//{
//    XercesParserLiaison     theParserLiaison;
//    XercesDOMSupport        theDOMSupport(theParserLiaison);
//
//    return theParserLiaison.createDocument(xercesDoc);
//}

void GetNodeByXPath(DOMDocument* xercesDoc, std::string xpathExpression)
{
    XercesParserLiaison     theParserLiaison;
    XercesDOMSupport        theDOMSupport(theParserLiaison);

    auto xalanDoc = theParserLiaison.createDocument(xercesDoc);

    XalanDocumentPrefixResolver thePrefixResolver(xalanDoc);

    XPathEvaluator  theEvaluator;
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
        XalanDocument* const theDocument = theLiaison.parseXMLStream(theInputSource);
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
        //theEvaluator.selectNodeList()
        //NodeRefList nodeList;
        //auto aaaa = theEvaluator.selectNodeList(
        //    nodeList,
        //    theDOMSupport,
        //    theDocument,
        //    XalanDOMString(xpathExpression.c_str()).c_str(),
        //    thePrefixResolver
        //);

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

        NodeRefList nodeList(theResult->nodeset());
        //const XObject* obj = theResult.get();
        //obj->
        //auto nodeList = &theEvaluator.getExecutionContext().getContextNodeList();

        for (size_t i = 0; i < nodeList.getLength(); i++)
        {
            auto currentNode = nodeList.item(i);
            //currentNode->
        }

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

// TODO: refactor and print XercesNode result
int temp()
{
    //cout << "Found " << result->getSnapshotLength() << endl;

    //// DOMImpl
    DOMImplementation* domImpl =
        DOMImplementationRegistry::getDOMImplementation(u"");

    //// DOMLSOutput-----------------------------------------
    DOMLSOutput* theOutPut = domImpl->createLSOutput();
    theOutPut->setEncoding(XMLString::transcode("UTF-8"));
    ////-----------------------------------------------------

    //// DOMLSSerializer-------------------------------------
    DOMLSSerializer* theSerializer = domImpl->createLSSerializer();
    ////-----------------------------------------------------

    //// Error Handler---------------------------------------
    DOMPrintErrorHandler myErrorHandler;
    ////-----------------------------------------------------

    //// Configure-------------------------------------------
    DOMConfiguration* serializerConfig = theSerializer->getDomConfig();
    // Set Error Handler
    serializerConfig->setParameter(XMLUni::fgDOMErrorHandler, &myErrorHandler);
    // Set Pretty Print
    if (serializerConfig->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
        serializerConfig->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    ////-----------------------------------------------------

    //// Format Target---------------------------------------
    XMLFormatTarget* myFormTarget = new StdOutFormatTarget();
    ////-----------------------------------------------------

    ////-----------------------------------------------------
    theOutPut->setByteStream(myFormTarget);

    //XMLSize_t nLength = result->getSnapshotLength();
    //for (XMLSize_t i = 0; i < nLength; i++)
    //{
    //    result->snapshotItem(i);
    //    theSerializer->write(
    //        result->getNodeValue(),
    //        theOutPut
    //    );
    //}

    return 0;
}
