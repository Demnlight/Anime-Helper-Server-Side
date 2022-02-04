#include <vector>
#include <string>

class CCrypt
{
public:
	std::string base64_encode(const std::string& in);
	std::string base64_decode(const std::string& in);

};

inline CCrypt* g_Crypt = new CCrypt();