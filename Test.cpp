#include "library.h"
#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::ifstream JsonFile("F:\\DRG Modding\\DRGPacker\\JSON\\Temp\\Game\\SK_Vanity_Armor_HospitalGown.json");
    std::string str;
    std::string file_contents;
    while (std::getline(JsonFile, str))
    {
        file_contents += str;
        file_contents.push_back('\n');
    }
    std::cout << "finished getline" << std::endl;
    JsonFile.close();
    char out[1024];
    char path[150] = "F:\\DRG Modding\\DRGPacker\\JSON\\Temp\\Game\\SK_Vanity_Armor_HospitalGown.fbx";

    //ExportStaticMeshIntoFbxFile(file_contents.data(), path, false, out);
    //ExportSkeletonIntoFbxFile(file_contents.data(), path, false, out);
    ExportSkeletalMeshIntoFbxFile(file_contents.data(), path, false, out);
}