#include <sstream>
#include "stringhelpers.h"
#include "../lib/common.h"

cJsonNode::cJsonNode(const char* jsonString) {
    json_error_t err;
    obj = json_loads(jsonString, 0, &err);
    if (!obj) {
        tell(0, "could not parse json, error on line %d, column %d", err.line, err.column);
    }
    root = std::shared_ptr<json_t>(obj, [](json_t* data){json_decref(data); data=nullptr;});
}

cJsonNode::cJsonNode(const std::string &jsonString) {
    json_error_t err;
    obj =  json_loads(jsonString.c_str(), 0, &err);
        if (!obj) {
        tell(0, "could not parse json, error on line %d, column %d", err.line, err.column);
    }
    root = std::shared_ptr<json_t>(obj, [](json_t* data){json_decref(data);});
}

cJsonNode::cJsonNode(json_t* data): obj(data), root(nullptr) {}

cJsonNode::cJsonNode(json_t* data, std::shared_ptr<json_t> root): obj(data), root(root) {}

bool cJsonNode::isNull(void)     { return !obj || json_is_null(obj);    }  // we need to check if this is a nullptr or a json NULL object
bool cJsonNode::isObject(void)   { return  obj && json_is_object(obj);  }
bool cJsonNode::isInt(void)      { return  obj && json_is_integer(obj); }
bool cJsonNode::isNumber(void)   { return  obj && json_is_number(obj);  }
bool cJsonNode::isString(void)   { return  obj && json_is_string(obj);  }
bool cJsonNode::isDouble(void)   { return  obj && json_is_real(obj);    }
bool cJsonNode::isArray(void)    { return  obj && json_is_array(obj);   }
size_t cJsonNode::currentArrayIndex(void) { return idx; }
size_t cJsonNode::arraySize(void){ return json_array_size(obj); }
void cJsonNode::resetArrayIterator(void) { idx = 0; }


cJsonNode cJsonNode::nextElementInArray() {
    if (isArray() && idx + 1 < json_array_size(obj)) {
        return json_array_get(obj, idx++);
    }
    idx = 0;
    return cJsonNode();
}

cJsonNode cJsonNode::operator[](const size_t index) {
    return json_array_get(obj, index);
}

cJsonNode cJsonNode::operator[](const char* name) {
    return json_object_get(obj, name);
}

cJsonNode cJsonNode::operator[](const char* name) const {
    return json_object_get(obj, name);
}

cJsonNode cJsonNode::operator[](const std::string &name) {
    return json_object_get(obj, name.c_str());
}

cJsonNode cJsonNode::operator[](const std::string &name) const {
    return json_object_get(obj, name.c_str());
}


std::vector<cJsonNode> cJsonNode::ArrayToVector(void) {
    std::vector<cJsonNode> result;
    result.reserve(json_array_size(obj));
    size_t idx;
    json_t *element;
    json_array_foreach(obj, idx, element) {
        result.emplace_back(element);
    }
    return result;
}

cJsonNode cJsonNode::getElementInArray(const size_t index) {
         if (index < json_array_size(obj)) return json_array_get(obj, index);
         return cJsonNode();
}

json_t* cJsonNode::object(void)  { return obj; }


json_t* cJsonNode::objectByName(const char* name) {
    return json_object_get(obj, name);
}

cJsonNode cJsonNode::nodeByName(const char* name) {
    return cJsonNode(objectByName(name), root);
}

int cJsonNode::intByName(const char* name) {
    json_t* int_json = json_object_get(obj, name);
    int result = json_integer_value(int_json);
    return result;
}

std::string cJsonNode::stringByName(const char* name) {
    json_t* str_json = objectByName(name);
    auto content = json_string_value(str_json);
    std::string result{content ? content : std::string()};
    return result;
}
double cJsonNode::doubleByName(const char* name) {
    json_t* double_json = objectByName(name);
    double result = json_real_value(double_json);
    return result;
}

double cJsonNode::doubleValue() { return json_number_value(obj);}

int cJsonNode::intValue() { return json_integer_value(obj); }

std::string cJsonNode::stringValue() {
         auto content = json_string_value(obj);
         return content ? content : std::string();
}

std::string cJsonNode::combine_jsonArrayStrings(const char* sep) {
    ostringstream result;
    size_t array_size = json_array_size(obj);
    size_t max_idx = array_size - 1;
    if (json_is_array(obj) && array_size > 0) {
        result << sep;
        size_t idx;
        json_t* element;
        json_array_foreach(obj, idx, element) {
            auto content = json_string_value(element);
            if (content) {
                result << content;
                if (idx < max_idx) result << sep;
            }
        }
    }
    return result.str();
}

void cJsonNode::addStringArrayToVector(std::vector<std::string> &result) {
    size_t n_elements = json_array_size(obj);
    result.reserve(n_elements + result.size());
    for (idx = 0; idx + 1 < n_elements;) {
        result.push_back(nextElementInArray().stringValue());
    }
}



cJsonLoader::cJsonLoader(const char* name)
{
   err = new json_error_t;
   obj = json_loads(name, 0, err); // TODO: why is there no check for errors? Should this fail silently?

}

cJsonLoader::~cJsonLoader()
{
   if (obj)
      json_decref(obj);

   if (err)
      delete(err);
}

json_t* cJsonLoader::objectByName(const char* name)   
{ 
   return json_object_get(obj, name); 
}

int cJsonLoader::isObject()                           
{ 
   return obj && json_is_object(obj); 
}

// TODO: remove this block
// std::string extractJsonString(const json_t *object, const char *key) {
//         json_t* str_json = json_object_get(object, key);
//         auto content = json_string_value(str_json);
//         std::string result{content ? content : std::string()};
//         return result;
// }

// int extractJsonInt(const json_t *object, const char *key) {
//         json_t* int_json = json_object_get(object, key);
//         int result = json_integer_value(int_json);
//         return result;
// }

// double extractJsonDouble(const json_t *object, const char *key) {
//         json_t* double_json = json_object_get(object, key);
//         double result = json_real_value(double_json);
//         return result;
// }

// std::vector<std::string> get_jsonArrayStrings(const json_t* object, const char* key) {
//     std::vector<std::string> result;
//     json_t* array = json_object_get(object, key);
//     if (json_is_array(array)) {
//         result.reserve(json_array_size((array)));
//         size_t idx;
//         json_t* element;
//         json_array_foreach(object, idx, element) {
//             auto content = json_string_value(element);
//             result.emplace_back(std::string{content ? content : std::string()});
//         }
//     }
//     return result;
// }

// std::string combine_jsonArrayStrings(const json_t* object, const char* key) {
//     ostringstream result;
//     result << "|";
//     json_t* array = json_object_get(object, key);
//     if (json_is_array(array)) {
//         size_t idx;
//         json_t* element;
//         json_array_foreach(object, idx, element) {
//             auto content = json_string_value(element);
//             if (content) result << content << "|";
//         }
//     }
//     return result.str();
// }

// std::vector<int> get_jsonArrayInts(const json_t* object, const char* key) {
//     std::vector<int> result;
//     json_t* array = json_object_get(object, key);
//     if (json_is_array(array)) {
//         result.reserve(json_array_size((array)));
//         size_t idx;
//         json_t* element;
//         json_array_foreach(object, idx, element) {
//             result.emplace_back(json_integer_value(element));
//         }
//     }
//     return result;
// }

//replace all "search" with "replace" in "subject"
string str_replace(const string& search, const string& replace, const string& subject) {
    string str = subject;
    size_t pos = 0;
    while((pos = str.find(search, pos)) != string::npos) {
        str.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return str;
}
//cut string after first "search"
string str_cut(const string& search, const string& subject) {
    string str = subject;
    string strCutted = "";
    size_t found = str.find_first_of(search);
    if (found != string::npos) {
        strCutted = str.substr(0, found);
        size_t foundSpace = strCutted.find_last_of(" ");
        if ((foundSpace != string::npos) && (foundSpace == (strCutted.size()-1))) {
            strCutted = strCutted.substr(0, strCutted.size()-1);
        }
    }
    return strCutted;
}
