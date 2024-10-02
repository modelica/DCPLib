#ifndef DCPLIB_IPTOSTR
#define DCPLIB_IPTOSTR

#if defined(DEBUG) || defined(LOGGING)
#include <string>
inline std::string ipToString(const uint32_t ip) {
    return std::to_string(ip >> 24 & 0xFF) + "." + std::to_string(ip >> 16 & 0xFF) + "." + std::to_string(ip >> 8 & 0xFF) + "." + std::to_string(ip & 0xFF);
}
#endif // defined(DEBUG) || defined(LOGGING)

#endif //DCPLIB_IPTOSTR