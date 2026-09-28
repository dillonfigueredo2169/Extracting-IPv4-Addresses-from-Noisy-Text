#include <iostream>
#include <string>

// Returns true if c is an ASCII digit.
static bool isDigitChar(char c)
{
    return c >= '0' && c <= '9';
}

// Returns true if c belongs inside a candidate run (digit, period, or colon).
static bool isRunChar(char c)
{
    return isDigitChar(c) || c == '.' || c == ':';
}

// Reads a decimal number from run starting at pos.
// Rules:
//   - at least 1 digit, at most maxDigits digits
//   - no leading zero unless the number is exactly "0"
//   - value must be <= maxValue
// Only accumulates up to maxDigits digits, so long digit strings cannot overflow.
// On success, advances pos past the digits and stores the value.
static bool parseNumber(const std::string& run, std::size_t& pos,
                        int maxDigits, unsigned long maxValue,
                        unsigned long& value)
{
    std::size_t start = pos;
    int count = 0;
    unsigned long v = 0;

    while (pos < run.size() && isDigitChar(run[pos]))
    {
        ++count;
        if (count <= maxDigits)
        {
            v = v * 10UL + static_cast<unsigned long>(run[pos] - '0');
        }
        ++pos;
    }

    if (count == 0 || count > maxDigits)
        return false;

    if (count > 1 && run[start] == '0')
        return false;

    if (v > maxValue)
        return false;

    value = v;
    return true;
}

// Checks whether an entire run exactly matches octet.octet.octet.octet[:port].
static bool matchRun(const std::string& run, unsigned long& address, int& port)
{
    std::size_t pos = 0;
    unsigned long octets[4];

    for (int i = 0; i < 4; ++i)
    {
        if (!parseNumber(run, pos, 3, 255UL, octets[i]))
            return false;

        if (i < 3)
        {
            if (pos >= run.size() || run[pos] != '.')
                return false;
            ++pos;
        }
    }

    int foundPort = -1;

    if (pos < run.size())
    {
        if (run[pos] != ':')
            return false;
        ++pos;

        unsigned long p = 0;
        if (!parseNumber(run, pos, 5, 65535UL, p))
            return false;

        if (pos != run.size())
            return false;

        foundPort = static_cast<int>(p);
    }

    address = (octets[0] << 24) | (octets[1] << 16) | (octets[2] << 8) | octets[3];
    port = foundPort;
    return true;
}

bool extractIPv4(const std::string& str, unsigned long& outAddress, int& outPort)
{
    outAddress = 0;
    outPort = -1;

    std::size_t i = 0;
    const std::size_t n = str.size();

    while (i < n)
    {
        // Skip separators.
        while (i < n && !isRunChar(str[i]))
            ++i;

        if (i >= n)
            break;

        // Collect one run of digits, periods, and colons.
        std::size_t start = i;
        while (i < n && isRunChar(str[i]))
            ++i;

        std::string run = str.substr(start, i - start);

        unsigned long address = 0;
        int port = -1;
        if (matchRun(run, address, port))
        {
            outAddress = address;
            outPort = port;
            return true;
        }
    }

    outAddress = 0;
    outPort = -1;
    return false;
}

int main()
{
    std::string line;

    while (true)
    {
        std::cout << "Enter a string (or 'END' to quit): ";
        if (!std::getline(std::cin, line))
            break;

        if (line == "END")
            break;

        unsigned long address = 0;
        int port = -1;

        if (extractIPv4(line, address, port))
        {
            unsigned long a = (address >> 24) & 0xFFUL;
            unsigned long b = (address >> 16) & 0xFFUL;
            unsigned long c = (address >> 8) & 0xFFUL;
            unsigned long d = address & 0xFFUL;

            std::cout << "Extracted IPv4 address: "
                      << a << '.' << b << '.' << c << '.' << d
                      << " (decimal value: " << address
                      << ", port: ";
            if (port == -1)
                std::cout << "none";
            else
                std::cout << port;
            std::cout << ")" << std::endl;
        }
        else
        {
            std::cout << "Invalid input: no valid IPv4 address found" << std::endl;
        }
    }

    std::cout << "Program terminated." << std::endl;
    return 0;
}
