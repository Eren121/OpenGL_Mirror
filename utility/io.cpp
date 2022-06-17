#include "io.hpp"
#include <iostream>
#include <fstream>

namespace io
{
    std::string readAll(const std::filesystem::path& path)
    {
        if(!std::filesystem::is_regular_file(path))
        {
            std::cerr << "Failed to read path " << path << ": this is not a regular file" << std::endl;
            return "";
        }
    
        std::ifstream ifs(path.string());
        std::string str((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
        
        return str;
    }
}