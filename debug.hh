#ifndef DEBUG_HH
#define DEBUG_HH

#include <glm/glm.hpp>
#include <tr1/memory>
#include <sstream>
#include <map>
#include <string>

class Debug {
public:
    Debug();
    ~Debug();

    Debug& operator [](const unsigned int& b);

    Debug& operator <<(const glm::vec3& v);

    template <typename T>
    Debug& operator <<(T const& viesti);

    Debug& operator <<(const Debug& end);

    static Debug& start();
    static Debug& end();

private:
    static Debug* me_;
    unsigned int taso_;
    std::ostringstream virta_;
    std::map<unsigned int, std::string> varit_;

    // kielletty
    Debug(const Debug&);
    Debug& operator=(const Debug&);
};

template <typename T>
Debug& Debug::operator <<(T const& viesti) {
    virta_ << viesti;
    return *this;
}

#endif
