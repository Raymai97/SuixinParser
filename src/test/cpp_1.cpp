extern "C" {
#include "base.h"
}
#include <exception>
#include <fstream>
#include <list>
#include <memory>
#include <string>

using std::string;

enum class MyError {
	None,
	OutOfMem
};

struct MyItem {
	std::string key, val;
};
struct MyData {
	std::list<MyItem> list;
	MyError error;
};

static enum SuixinUserReturn MySx_OnID(
	uint8_t const *pID_, int cbID, void *user, void **ppItem)
{
	// This callback is called when an ID is encountered.
	// 
	// Value of *ppItem, if set to non-NULL, will make the parser to
	// call the OnContent callback with its value.
	// Otherwise, it will skip the Content of current ID.
	// 
	// In this example, we allocate an Item with ID here,
	// pass the pointer of Item to OnContent callback via *ppItem.
	//
	MyData & my = *reinterpret_cast<MyData*>(user);
	char const *pID = reinterpret_cast<char const *>(pID_);
	try {
		my.list.push_back(MyItem());
		MyItem &item = my.list.back();
		try {
			item.key.assign(pID, cbID);
			*ppItem = &item;
		}
		catch (std::bad_alloc const & ex) {
			my.list.pop_back();
			throw ex;
		}
	}
	catch (std::bad_alloc const &) {
		my.error = MyError::OutOfMem;
		return ::AbortSuixinParse;
	}
	return ::ContinueSuixinParse;
}

static enum SuixinUserReturn MySx_OnContent(
	uint8_t const *pCtnt_, int cbCtnt, void *user, void *pItem)
{
	// This callback is called when the Content of ID is encountered.
	// 
	// In this example, we simply refer back the Item of ID via *ppItem
	// that we set in our OnID callback. We will complete the Item
	// with the Content we got here.
	//
	MyData & my = *reinterpret_cast<MyData*>(user);
	char const *pCtnt = reinterpret_cast<char const *>(pCtnt_);
	MyItem & item = *reinterpret_cast<MyItem*>(pItem);
	try {
		item.val.assign(pCtnt, cbCtnt);
	}
	// If messed up, remove and delete the incomplete Item.
	catch (std::bad_alloc const &) {
		my.error = MyError::OutOfMem;
		my.list.pop_back();
		return ::AbortSuixinParse;
	}
	return ::ContinueSuixinParse;
}

extern "C" void Test_Cpp_1(
	TestStat *pStat, TestSpec const test)
{
	try {
		// Open the test file
		std::ifstream file;
		file.open(test.pszFile, std::ios::binary);
		if (!file) {
			throw TestErr_OpenFile;
		}
		
		// Check the file size
		file.seekg(0, std::ios::end);
		std::streamoff cbFile = file.tellg();
		if (cbFile > 0xFFFF) {
			throw TestErr_FileTooBig;
		}

		// Allocate buffer to store the file data
		int cbData = static_cast<int>(cbFile);
		std::shared_ptr<uint8_t> data(new uint8_t[cbData]);
		if (!data) {
			throw TestErr_OutOfMem;
		}

		// Read all file data to the buffer
		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char*>(data.get()), cbData);
		std::streamsize cbRead = file.gcount();
		if (cbRead != cbData) {
			throw TestErr_ReadFileCbMismatch;
		}
		
		// Parse and enum the Suixin items
		MyData my{};
		::SuixinParseSpec spec{};
		spec.cbData = cbData;
		spec.pData = data.get();
		spec.charIs16bit = false;
		spec.user = &my;
		spec.pfnOnID = MySx_OnID;
		spec.pfnOnContent = MySx_OnContent;
		::SuixinParse(&(pStat->sxParseStat), &spec);
		if (my.error == MyError::OutOfMem) {
			throw TestErr_OutOfMem;
		}
		if (pStat->sxParseStat.error) {
			throw TestErr_SxParseError;
		}
		for (auto const & item : my.list) {
			TestUser_OnItem(test.pUser,
				item.key.c_str(),
				item.val.c_str());
		}
	}
	catch (enum TestErr const err) {
		pStat->testErr = err;
	}
}
