#include <fstream>
#include <sstream>
#include <algorithm>

#include "lumploader.hh"
#include "block.hh"
#include "manager.hh"

LumpLoader::LumpLoader(const std::string& filename):
    filename_(filename)
{}

void LumpLoader::load() {
    std::ifstream file(filename_.c_str());
    std::string line;
    char turha;
    bool lue = false;
    while (getline(file, line)) {
        if (line == "BEGIN") lue = true;
        else if (line == "END") lue = false;

        if (!lue || line.length() == 0 || line.at(0) == ';') continue;

        std::istringstream line_stream(line);

        std::string filename;
        getline(line_stream, filename, '~');
        std::string dir;
        getline(line_stream, dir, '~');

        std::replace(dir.begin(), dir.end(), '\\', '/');

        if (dir.substr(0, 7) != "/cd-001") continue;

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

        filename = std::string("assets/obj") + dir + filename + std::string("_hd.obj");
        Manager::instance().addObject("-", new Block(filename, origin, bb));
    }
}
