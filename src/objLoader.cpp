#include "objLoader.h"

int ObjLoader::loadObjFile(std::string fileName, 
    std::vector<std::array<double, 4>>* vertexArray, 
    std::vector<int>* indices, 
    std::vector<std::array<double, 4>>* normals, 
    std::vector<std::array<double, 2>>* texCoords, 
    int offset=0, float scale=1.0){
    std::ifstream file_handle;
    file_handle.open(fileName);
    std::string identifier = "";
    int n_vertices = 0;
    while(file_handle >> identifier){
        if(identifier == "v"){
            readVertex(file_handle, vertexArray, scale);
            n_vertices++;
        }
        else if(identifier == "vt")
            readTexCoord(file_handle, texCoords);
        else if(identifier == "vn")
            readNormal(file_handle, normals);
        else if(identifier == "f")
            readTriangle(file_handle, indices, offset);
        else{
            std::string line;
            getline(file_handle, line, '\n');
        }
    }
    return n_vertices;
}

void ObjLoader::readVertex(std::ifstream& file_handle, std::vector<std::array<double, 4>>* vertexArray, float scale){
    std::array<double, 4> pArr;
    for(int i=0;i<3;i++){
        file_handle >> pArr[i];
        pArr[i] *= scale;
    }
    pArr[3] = 1.0;
    vertexArray->push_back(pArr);
}
void ObjLoader::readTexCoord(std::ifstream& file_handle, std::vector<std::array<double, 2>>* texCoords){
    std::array<double, 2> pArr;
    for(int i=0;i<2;i++)
        file_handle >> pArr[i];
    texCoords->push_back(pArr);
}
void ObjLoader::readNormal(std::ifstream& file_handle, std::vector<std::array<double, 4>>* normals){
    std::array<double, 4> pArr;
    for(int i=0;i<3;i++)
        file_handle >> pArr[i];
    pArr[3] = 1.0;
    normals->push_back(pArr);
}
void ObjLoader::readTriangle(std::ifstream& file_handle, std::vector<int>* indices, int offset){
    std::string item;
    for(int i=0;i<3;i++){
        file_handle >> item;
        std::string indexString = item.substr(0, item.find('/'));
        indices->push_back(std::stoi(indexString) + offset - 1);
    }
}