#ifndef LUMPLOADER_HH
#define LUMPLOADER_HH

#include <string>

class LumpLoader {
public:
    enum LoadMode { OBJ, GLDATA };

    LumpLoader(const std::string& filename, LoadMode mode = GLDATA);
    void load();
private:
    std::string filename_;
    LoadMode mode_;
};

#endif
