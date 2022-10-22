#ifndef FBX_WRAPPER_JSONDESERIALIZER_H
#define FBX_WRAPPER_JSONDESERIALIZER_H

#include <string>
#include "json.hpp"
#include "SMStructs.h"
#include "SKStructs.h"
#include "SKMStructs.h"

class JsonDeserializer {
public:
    static FStaticMeshStruct DeserializeSM(std::string Json);
    static FSkeletonStruct DeserializeSK(std::string Json);
    static FSkeletalMeshStruct DeserializeSKM(std::string Json);
};


#endif //FBX_WRAPPER_JSONDESERIALIZER_H
