
#ifndef _STRINGHELPERS_H_
#define _STRINGHELPERS_H_

#include <string>
#include <vector>
#include <iterator>
#include <memory>
#include <sstream>
#include <iostream>

#include <jansson.h>

#include "../lib/common.h"

using namespace std;

string str_replace(const string& search, const string& replace, const string& subject);
string str_cut(const string& search, const string& subject);

//
// This is a wrapper for libjansson, intended to be used for read-only purposes.
// the lifetime of the pointer to the json data structure is guaranteed by passing it as a shared_ptr to child nodes
// 
// To ease error handling for missing values (a lot of TMDB's fields are optional),
// a default value is returned if no value is available ("" for a stringValue(), 0 for an json number).
///

class cJsonNode
{
   public:
      cJsonNode() = default;
      cJsonNode(const char* jsonString);
      cJsonNode(const std::string &jsonString);
      cJsonNode(json_t* data); // does not own/manage the given json_t*
      cJsonNode(json_t* data, std::shared_ptr<json_t> root); // keeps root alive while used
      ~cJsonNode() = default;
      bool isNull(void);
      bool isObject(void);
      bool isInt(void);
      bool isNumber(void);
      bool isString(void);
      bool isDouble(void);
      bool isArray(void);
      size_t currentArrayIndex(void) const;
      size_t arraySize(void) const;
      void resetArrayIterator(void); 
      json_t* object(void);
      json_t* objectByName(const char* name) const;
      cJsonNode nodeByName(const char* name);
      cJsonNode operator[](const size_t index);
      cJsonNode operator[](const char* name) &;
      cJsonNode operator[](const char* name) const &;
      cJsonNode operator[](const std::string &name)&;
      cJsonNode operator[](const std::string &name) const &;
      // cJsonNode operator[](const std::string &name) const;
      cJsonNode nextElementInArray();
      std::vector<cJsonNode> ArrayToVector(void) const;

      cJsonNode getElementInArray(const size_t index);
      int intByName(const char* name) const;
      std::string stringByName(const char* name) const;
      double doubleByName(const char* name) const;
      double doubleValue() const;
      int intValue() const; // returns zero on error
      std::string stringValue() const;
      std::string combine_jsonArrayStrings(const char* sep="|", const bool surround=true);
      void addStringArrayToVector(std::vector<std::string> &result);
   protected:
      json_t* obj = nullptr;
      std::shared_ptr<json_t> root = nullptr; // this is the root of the json data structure
                                              // if we create the json_t*, we need to keep this pointer alive until all objects accessing the
                                              // json data have been destroyed - so we pack it into a shared_ptr with a destructor appropriate
                                              // for a json_t* (see constructor implementation)
      size_t idx = 0;
};

class cJsonLoader
{
   public:

      cJsonLoader(const char* name);
      ~cJsonLoader();

      json_t* object()                         { return obj; }
      json_error_t* error()                    { return err; }
      json_t* objectByName(const char* name);
      int isObject();

   protected:

      json_error_t* err;
      json_t* obj;
};

#endif // _STRINGHELPERS_H_
