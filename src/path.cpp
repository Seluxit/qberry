#include "path.hpp"
#include <iostream>

namespace berry {

    Path::Path(const std::string& path)
    {
        DIR* dird;
        DIR* dirf;
        struct dirent* pd;
        struct dirent* pf;
    
        dit_ = dirnames_.end();
        fit_ = filenames_.end();

        if ( (dird = opendir(path.c_str())) == NULL)  {
            std::string msg = "Can not open " + path + " directory!";
            std::cout << msg << "\n";
            return;
        }

        while ( (pd = readdir(dird)) != NULL) {
            std::string dirName = pd->d_name; 
            if (pd->d_type == DT_DIR && dirName != ".." && dirName != ".") {
                dirnames_.push_back(dirName); 
            } 
        }

        dirf = opendir(path.c_str());
        while ( (pf = readdir(dirf)) != NULL) {
            std::string fileName = pf->d_name; 
            if (pf->d_type == DT_REG) {
                filenames_.push_back(fileName); 
            } 
         }

        closedir(dird);
        closedir(dirf);
        
        if (!dirnames_.empty())
            dit_ = dirnames_.begin();
        
        if (!filenames_.empty())
            fit_ = filenames_.begin();
    }
    
    Path::~Path()
    {
    }

    piterator& Path::file()
    {
        return fit_;
    }
    
    piterator& Path::directory()
    {
         return dit_;
    }
    
    piterator Path::files_end()
    {
        return filenames_.end();
    }
   
    piterator Path::dirs_end()
    {
        return dirnames_.end();
    }

} // namespace qplus
