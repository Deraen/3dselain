#include <fstream>
#include <sstream>
#include <algorithm>

#include "lumploader.hh"
#include "block.hh"
#include "manager.hh"

LumpLoader::LumpLoader(const std::string& filename, LoadMode mode):
    filename_(filename),
    mode_(mode)
{}

void LumpLoader::load() {
    std::ifstream file(filename_.c_str());
    std::string line;
    char turha;
    bool lue = false;
    while (getline(file, line)) {
        if (line == "BEGIN") {
            lue = true;
            continue;
        }
        else if (line == "END") lue = false;

        if (!lue || line.length() == 0 || line.at(0) == ';') continue;

        std::istringstream line_stream(line);

        std::string filename;
        getline(line_stream, filename, '~');
        std::string dir;
        getline(line_stream, dir, '~');

        std::replace(dir.begin(), dir.end(), '\\', '/');

        std::string origin_s;
        getline(line_stream, origin_s, '~');
        std::istringstream str2floats(origin_s);
        glm::vec3 origin;
        str2floats >> origin.x >> turha >> origin.z >> turha >> origin.y;
        origin.z = -origin.z;

        std::string bb_s;
        getline(line_stream, bb_s, '~');
        std::istringstream str2floats2(bb_s);
        glm::vec3 bb;
        str2floats2 >> bb.x >> turha >> bb.z >> turha >> bb.y;

        if (mode_ == OBJ) {
            filename = std::string("assets/obj") + dir + filename + std::string("_hd.obj");
        } else if (mode_ == GLDATA) {
            filename = std::string("assets/gldata") + dir + filename + std::string("_hd.gldata");
        }


        Manager::instance().addObject("-", new Block(filename, origin, bb, dir.substr(0, 8) == "/streets"));
    }
}
