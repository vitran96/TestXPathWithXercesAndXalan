﻿#include "test_xpath_xalan.h"

#include <xercesc/dom/DOM.hpp>

#include <xercesc/util/TransService.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>

#include <xercesc/util/XMLUni.hpp>

XERCES_CPP_NAMESPACE_USE

#include <xalanc/PlatformSupport/XSLException.hpp>

#include <xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp>

#include <xalanc/XPath/XObject.hpp>
#include <xalanc/XPath/XPathEvaluator.hpp>


#include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>
#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>

#include <xalanc/XercesParserLiaison/XercesElementWrapper.hpp>

using namespace xalanc;

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <list>

#include <chrono>

#define TEST_FILE "sample.xml"

long long GetTimestamp()
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return millis;
}

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
std::list<DOMElement*> GetNodeByXPath(DOMDocument* xercesDoc, const std::string& xpathExpression);
void PrintDOMElement(std::list<DOMElement*> elementsList);

int mainXpathTest(const int argc, const char* argv[]);

int main(const int argc, const char* argv[])
{
    XMLPlatformUtils::Initialize();
    XPathEvaluator::initialize();

    int result = mainXpathTest(argc, argv);

    XPathEvaluator::terminate();
    XMLPlatformUtils::Terminate();

    return result;
}

int mainXpathTest(const int argc, const char* argv[])
{
    std::cout << "This is a program to test Xalan 1.12.0 XPath 1.0 support.\n"
        << "'sample.xml' next to TestXercesXpathFeatures.exe will be used for testing.\n"
        << "If you want to use your XML file, rename and replace sample.xml.\n"
        << "This program only take 1 argument as XPath. Pass in more than 1 the program will only use the 1st one.\n";

    if (argc == 1)
    {
        std::cout << "Please pass in a XPath argument" << std::endl;
        return 1;
    }

    std::string xmlFile(TEST_FILE);
    std::string xpathExpression(argv[1]);

    std::cout << "\nXPath: " << xpathExpression << std::endl;

    try
    {
        long long startTime(GetTimestamp());

        auto xercesDoc = ParseFile(xmlFile);

        long long afterParsingAFile(GetTimestamp());

        auto xercesElementsList = GetNodeByXPath(xercesDoc, xpathExpression);

        long long afterAnXPathExpression(GetTimestamp());

        //PrintDOMElement(xercesElementsList);

        xercesElementsList.clear();
        xercesDoc->release();

        std::cout << "Parsing time: " << (afterParsingAFile - startTime) << std::endl;
        std::cout << "XPath time: " << (afterAnXPathExpression - afterParsingAFile) << std::endl;

        return 0;
    }
    catch (std::exception e)
    {
        std::cout << "\n" << e.what() << std::endl;
        return 1;
    }
}

DOMDocument* ParseFile(const std::string& file)
{
    XercesDOMParser parser;
    parser.setValidationScheme(XercesDOMParser::Val_Auto);
    parser.setDoNamespaces(true);
    parser.parse(file.c_str());

    return parser.adoptDocument();
}

std::list<DOMElement*> GetNodeByXPath(DOMDocument* xercesDoc, const std::string& xpathExpression)
{
    try
    {
        XercesParserLiaison     theParserLiaison;
        //theParserLiaison.setDoNamespaces(true);
        XercesDOMSupport        theDOMSupport(theParserLiaison);

        // BuildWrapper = true
        // ThreadSafe = false
        // BuildMap = false
        auto xalanDoc = theParserLiaison.createDocument(xercesDoc);
        //theParserLiaison.createDOMFactory();

        XalanDocumentPrefixResolver thePrefixResolver(xalanDoc);

        XPathEvaluator  theEvaluator;

        const XObjectPtr theResult(
            theEvaluator.evaluate(
                theDOMSupport,
                xalanDoc,
                XalanDOMString(xpathExpression.c_str()).c_str(),
                thePrefixResolver
            )
        );

        //auto compiledXPath = theEvaluator.createXPath(XalanDOMString(xpathExpression.c_str()).c_str(), thePrefixResolver);

        //const XObjectPtr theResult(
        //    theEvaluator.evaluate(
        //        theDOMSupport,
        //        xalanDoc,
        //        *compiledXPath,
        //        thePrefixResolver
        //    )
        //);

        auto nodeList = &theResult->nodeset();
        size_t size = nodeList->getLength();
        if (size == 0)
            throw std::runtime_error("No nodes!!!");

        std::list<DOMElement*> xercesElementsList;

        for (size_t i = 0; i < size; i++)
        {
            auto currentNode = nodeList->item(i);
            if (currentNode->getNodeType() != XalanNode::ELEMENT_NODE)
            {
                throw std::runtime_error("XPath result contain non-element node!!");
            }

            auto currentXalanElementWrapper = dynamic_cast<XercesElementWrapper*>(currentNode);
            xercesElementsList.push_back(const_cast<DOMElement*>(currentXalanElementWrapper->getXercesNode()));
        }

        theParserLiaison.destroyDocument(xalanDoc);
        return xercesElementsList;
    }
    catch (const XSLException& e)
    {
        CharVectorType errorMessage;
        e.getMessage().transcode(errorMessage);
        std::stringstream ss;
        ss << errorMessage;
        throw std::runtime_error(ss.str());
    }
    catch (const std::exception& e)
    {
        throw e;
    }
    catch (...)
    {
        throw std::runtime_error("Unkown exception occurred!");
    }
}

void PrintDOMElement(std::list<DOMElement*> elementsList)
{
    std::cout << "\nFound " << elementsList.size() << " elements" << std::endl;

    // DOMImpl
    DOMImplementation* domImpl = DOMImplementationRegistry::getDOMImplementation(u"");
    //-----------------------------------------------------

    // DOMLSOutput-----------------------------------------
    DOMLSOutput* theOutPut = domImpl->createLSOutput();
    theOutPut->setEncoding(XMLString::transcode("UTF-8"));
    //-----------------------------------------------------

    // DOMLSSerializer-------------------------------------
    DOMLSSerializer* theSerializer = domImpl->createLSSerializer();
    //-----------------------------------------------------

    // Error Handler---------------------------------------
    DOMPrintErrorHandler myErrorHandler;
    //-----------------------------------------------------

    // Configure-------------------------------------------
    DOMConfiguration* serializerConfig = theSerializer->getDomConfig();
    //-----------------------------------------------------

    // Set Error Handler-----------------------------------
    serializerConfig->setParameter(XMLUni::fgDOMErrorHandler, &myErrorHandler);
    //-----------------------------------------------------

    // Set Pretty Print------------------------------------
    if (serializerConfig->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
        serializerConfig->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    //-----------------------------------------------------

    // Format Target---------------------------------------
    StdOutFormatTarget consoleOutputFormatTarget;
    //-----------------------------------------------------

    //-----------------------------------------------------
    theOutPut->setByteStream(&consoleOutputFormatTarget);

    // Print-----------------------------------------------
    for (auto element : elementsList)
        theSerializer->write(element, theOutPut);
    //-----------------------------------------------------

    // Release memory--------------------------------------
    theOutPut->release();
    theSerializer->release();
    consoleOutputFormatTarget.flush();
    //-----------------------------------------------------

    std::cout << "\n";
}
