#ifndef FBX_WRAPPER_JSONDESERIALIZER_H
#define FBX_WRAPPER_JSONDESERIALIZER_H

#include <string>
#include "include/nlohmann/json.hpp"
#include "SMStructs.h"

class JsonDeserializer {
public:
    static FStaticMeshStruct DeserializeSM(std::string Json);
};


#endif //FBX_WRAPPER_JSONDESERIALIZER_H
