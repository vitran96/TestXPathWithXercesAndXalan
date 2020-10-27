#include "TestXalanDom.h"

#include <xercesc/util/PlatformUtils.hpp>
//#include <xercesc/framework/MemoryManager.hpp>

#include <xalanc/XalanSourceTree/XalanSourceTreeDOMSupport.hpp>
#include <xalanc/XalanSourceTree/XalanSourceTreeParserLiaison.hpp>

#include <xalanc/XalanSourceTree/XalanSourceTreeDocument.hpp>
#include <xalanc/XalanSourceTree/XalanSourceTreeElement.hpp>

#include <xalanc/PlatformSupport/AttributesImpl.hpp>

#include <xalanc/XalanDOM/XalanDOMString.hpp>

xercesc::MemoryManager& getDefaultXercesMemMgr()
{
	using xercesc::XMLPlatformUtils;

	assert(XMLPlatformUtils::fgMemoryManager != 0);

	return *XMLPlatformUtils::fgMemoryManager;
}

void tryXalanDOM_1();

int main(const int argc, const char* argv[])
{
	xercesc::XMLPlatformUtils::Initialize();

	tryXalanDOM_1();

	xercesc::XMLPlatformUtils::Terminate();

	return 0;
}

void tryXalanDOM_1()
{
	xalanc::XalanSourceTreeDocument* doc1 = nullptr;
	xalanc::XalanSourceTreeElement* element1 = nullptr;

	xalanc::XalanSourceTreeParserLiaison    theParserLiaison;
	xalanc::XalanSourceTreeDOMSupport       theDOMSupport(theParserLiaison);

	doc1 = theParserLiaison.createXalanSourceTreeDocument();
	xalanc::AttributesImpl attributes1;
	element1 = doc1->createElementNode(u"root", attributes1);
	//doc1->getAttributes();

	//theParserLiaison.destroyDocument(doc1);

	//
}
