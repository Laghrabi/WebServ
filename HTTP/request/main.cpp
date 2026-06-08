#include "HttpRequest.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

int main() {
    // 1. Simulate the incoming network packet
    std::string packet = "POST /upload HTTP/1.1\r\n"
                         "Host: localhost\r\n"
                         "Content-Length: 15\r\n\r\n"
                         "Hello World!!!!";

    // 2. Convert to the raw buffer our parser expects
    std::vector<char> rawBuffer(packet.begin(), packet.end());

    // 3. Create the request object and parse
    HttpRequest test;
    test.parse(rawBuffer);

    // 4. Print the parsed metadata
    // std::cout << "--- Parsed Metadata ---\n";
    // std::cout << "Method: " << test.getMethod() << "\n";
    // std::cout << "URI: " << test.getUri() << "\n";
    // // Assuming 5 is FINISHED based on your enum order
    // std::cout << "State: " << test.getCurrentState() << " (5 = FINISHED)\n\n";

    // // 5. Open the saved file and read the body back out
    // std::cout << "--- File Extraction ---\n";
    // std::cout << "Saved to: " << test.getBodyFilePath() << "\n";

    // std::ifstream inFile(test.getBodyFilePath().c_str());
    // if (inFile.is_open()) {
    //     std::ostringstream buffer;
    //     buffer << inFile.rdbuf(); // Dumps the whole file into the buffer
    //     std::cout << "File Contents: [" << buffer.str() << "]\n";
    //     inFile.close();
    // } else {
    //     std::cout << "Failed to open the file!\n";
    // }

    return 0;
}