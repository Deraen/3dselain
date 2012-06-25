#ifndef LUMPLOADER_HH
#define LUMPLOADER_HH

#include <string>

class LumpLoader {
public:
	LumpLoader(const std::string& filename);
	void load();
private:
	std::string filename_;
};

#endif
