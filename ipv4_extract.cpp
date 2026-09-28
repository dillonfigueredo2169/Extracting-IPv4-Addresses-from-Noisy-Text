// ipv4_extract.cpp
// Extracts a single valid IPv4 address (with optional :port) from a line of text.
// No library numeric conversion, address parsing, or regex is used.

#include <iostream>
#include <string>
#include <cctype>

// A character that can be part of a candidate token.
static bool isTokenChar(char c) {
    return std::isdigit(static_cast<unsigned char>(c)) || c == '.' || c == ':';
}

// Reads one number starting at pos, stopping at the first non-digit or at end.
// Rules: 1..maxDigits digits, no leading zero unless the number is exactly "0",
// value <= maxValue. On success, advances pos past the digits and returns true.
// Digits are counted before accumulating, so long runs can never overflow.
static bool parseNumber(const std::string& s, size_t& pos, size_t end,
                        int maxDigits, unsigned long maxValue,
                        unsigned long& value) {
    size_t digitsStart = pos;
    size_t digitsEnd = pos;
    while (digitsEnd < end && std::isdigit(static_cast<unsigned char>(s[digitsEnd]))) {
        ++digitsEnd;
    }

    size_t count = digitsEnd - digitsStart;
    if (count == 0 || count > static_cast<size_t>(maxDigits)) {
        return false;                       // empty, or too many digits
    }
    if (count > 1 && s[digitsStart] == '0') {
        return false;                       // leading zero ("01", "00")
    }

    unsigned long result = 0;
    for (size_t k = digitsStart; k < digitsEnd; ++k) {
        result = result * 10 + static_cast<unsigned long>(s[k] - '0');
    }
    if (result > maxValue) {
        return false;                       // out of range
    }

    value = result;
    pos = digitsEnd;
    return true;
}

// Checks whether the whole run s[start, end) matches the grammar exactly:
//   octet.octet.octet.octet[:port]
static bool parseToken(const std::string& s, size_t start, size_t end,
                       unsigned long& address, int& port) {
    size_t pos = start;
    unsigned long octets[4];

    for (int i = 0; i < 4; ++i) {
        if (!parseNumber(s, pos, end, 3, 255, octets[i])) {
            return false;
        }
        if (i < 3) {
            if (pos >= end || s[pos] != '.') {
                return false;               // missing period between octets
            }
            ++pos;
        }
    }

    int foundPort = -1;
    if (pos < end) {
        if (s[pos] != ':') {
            return false;                   // e.g. a fifth octet or trailing '.'
        }
        ++pos;
        unsigned long p = 0;
        if (!parseNumber(s, pos, end, 5, 65535, p)) {
            return false;                   // empty, bad, or out-of-range port
        }
        foundPort = static_cast<int>(p);
    }

    if (pos != end) {
        return false;                       // anything left over (e.g. "::", ":80.")
    }

    address = (octets[0] << 24) | (octets[1] << 16) | (octets[2] << 8) | octets[3];
    port = foundPort;
    return true;
}

// Returns true if a valid address was found, false otherwise.
// On success: outAddress holds the 32-bit value,
// and outPort holds the port number, or -1 if no port was present.
// On failure: outAddress is set to 0 and outPort is set to -1.
// If several valid addresses appear, the first one is returned.
bool extractIPv4(const std::string& str, unsigned long& outAddress, int& outPort) {
    outAddress = 0;
    outPort = -1;

    size_t i = 0;
    const size_t n = str.size();
    while (i < n) {
        if (!isTokenChar(str[i])) {
            ++i;                            // garbage: skip it
            continue;
        }

        // A token is the maximal run of digits, periods, and colons.
        size_t j = i;
        while (j < n && isTokenChar(str[j])) {
            ++j;
        }

        unsigned long address = 0;
        int port = -1;
        if (parseToken(str, i, j, address, port)) {
            outAddress = address;
            outPort = port;
            return true;
        }

        i = j;                              // reject the whole run; never retry inside it
    }
    return false;
}

int main() {
    std::string line;
    while (true) {
        std::cout << "Enter a string (or 'END' to quit): ";
        if (!std::getline(std::cin, line)) {
            break;                          // end of input stream
        }
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();                // handle Windows line endings
        }
        if (line == "END") {
            break;
        }

        unsigned long address;
        int port;
        if (extractIPv4(line, address, port)) {
            std::cout << "Extracted IPv4 address: "
                      << ((address >> 24) & 0xFF) << '.'
                      << ((address >> 16) & 0xFF) << '.'
                      << ((address >> 8) & 0xFF) << '.'
                      << (address & 0xFF)
                      << " (decimal value: " << address << ", port: ";
            if (port == -1) {
                std::cout << "none";
            } else {
                std::cout << port;
            }
            std::cout << ")\n";
        } else {
            std::cout << "Invalid input: no valid IPv4 address found\n";
        }
    }
    std::cout << "Program terminated.\n";
    return 0;
}
