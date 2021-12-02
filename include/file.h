#ifndef FILE_H
#define FILE_H

#include <string>
#include <vector>
#include <object.h>
#include <tuple>

class File
{
    public:        
        File();
        File(std::string fileName);

        std::string fileName;

        void saveToFile(std::vector<Object> objects);         
        std::tuple<std::vector<Object>, bool> loadFromFile();  
};

#endif