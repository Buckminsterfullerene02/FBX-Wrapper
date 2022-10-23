#include "library.h"
#include <iostream>
#include <fstream>
#include <string>

int main() {
    // Read in Json file at path F:\DRG Modding\DRGPacker\JSON\SMs\Game\Art\Carvers\SM_Carver_Cube_01.json
    std::ifstream JsonFile("F:\\DRG Modding\\DRGPacker\\JSON\\SMs\\Game\\SM_Sphere_A.json");
    std::string str;
    std::string file_contents;
    while (std::getline(JsonFile, str))
    {
        file_contents += str;
        file_contents.push_back('\n');
    }
    JsonFile.close(); // close file
    char out[1024];
    char path[100] = "F:\\DRG Modding\\DRGPacker\\JSON\\SMs\\Game\\SM_Sphere_A.fbx";
    ExportStaticMeshIntoFbxFile(file_contents.data(), path, false, out);
}