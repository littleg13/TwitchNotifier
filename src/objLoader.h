#ifndef OBJLOADER_H
#define OBJLOADER_H
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <fstream>

class ObjLoader {
public:
    static int loadObjFile(std::string fileName, std::vector<std::array<double, 4>>* vertexArray, std::vector<int>* indices, std::vector<std::array<double, 4>>* normals, std::vector<std::array<double, 2>>* texCoords, int offset, float scale);
private:
    static void readVertex(std::ifstream& file_handle, std::vector<std::array<double, 4>>* vertexArray, float scale);
    static void readTexCoord(std::ifstream& file_handle, std::vector<std::array<double, 2>>* texCoords);
    static void readNormal(std::ifstream& file_handle, std::vector<std::array<double, 4>>* normals);
    static void readTriangle(std::ifstream& file_handle, std::vector<int>* indices, int offset);
};
#endif