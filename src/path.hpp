#pragma once

#include <dirent.h>
#include <string>
#include <vector>

namespace berry {
 
    typedef std::vector<std::string>::const_iterator piterator;

    class Path {

        private:
            piterator fit_;
            piterator dit_;

            std::vector<std::string> filenames_;
            std::vector<std::string> dirnames_;

        public:
            Path(const std::string& dir);
            ~Path();

            piterator& file();
            piterator& directory();
            piterator files_end();
            piterator dirs_end();
    };

} // namespace qplus
