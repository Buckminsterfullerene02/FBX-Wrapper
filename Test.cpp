#include "library.h"
#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::ifstream JsonFile("F:\\DRG Modding\\DRGPacker\\JSON\\SMs\\Game\\SM_Planet_Hoxxes_A_500k.json");
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
    char path[100] = "F:\\DRG Modding\\DRGPacker\\JSON\\SMs\\Game\\SM_Planet_Hoxxes_A_500k.fbx";

    //ExportStaticMeshIntoFbxFile(file_contents.data(), path, false, out);
    ExportSkeletonIntoFbxFile(file_contents.data(), path, false, out);
}