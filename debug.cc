#include <iostream>
using std::cout;
using std::endl;

#include "debug.hh"

Debug::Debug():
    taso_(0),
    varit_()
{
    // eri värejä eri tasojen viesteille
    varit_[0] = "\x1b[31m";
    varit_[1] = "\x1b[33m";
    varit_[2] = "\x1b[36m";
    varit_[3] = "\x1b[32m";
    varit_[-1] = "\x1b[0m"; // reset
}

Debug::~Debug()
{}

Debug& Debug::instance() {
    static Debug instance_;
    return instance_;
}

Debug& Debug::operator[](const unsigned int& b)
{
    taso_ = b;

    if (varit_.count(taso_) > 0) {
        virta_ << varit_[taso_];
    }

    return *this;
}

Debug& Debug::start() {
    instance().taso_ = 0;
    return instance();
}

Debug& Debug::operator <<(const Debug& /*a*/) {
    virta_ << varit_[-1]; // reset color

    cout << virta_.str() << endl;
    virta_.str("");
    return *this;
}

Debug& Debug::end() {
    return instance();
}

Debug& Debug::operator <<(const glm::vec3& v) {
    *this << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return *this;
}

