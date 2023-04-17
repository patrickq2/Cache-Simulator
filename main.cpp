#include <bitset>
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <tuple>
#include <vector>

std::string hexToBin(const std::string& s)
{
    std::stringstream ss;
    ss << std::hex << s;
    unsigned n;
    ss >> n;
    std::bitset<32> b(n);

    return b.to_string().substr(32 - 4*(s.length()));
}

int main() {
    // gets input parameters for cache from user
    int bytes; 
    std::cout << "How many bytes will the cache be? " << std::endl;
    std::cin >> bytes;

    int blockSize;
    std::cout << "How big will a block be?" << std::endl;
    std::cin >> blockSize;

    int linesPerSet;
    std::cout<< "How many lines per set? " << std::endl;
    std::cin >> linesPerSet;

    //data for splitting binary to be stored
    int numLines = bytes/blockSize;
    int offsetWidth = std::log2(blockSize);
    int lineWidth = std::log2(numLines);
    int setWidth = std::log2(numLines/linesPerSet);

    // data structures for cache implementation
    std::vector<std::tuple<int, int, int, char>> directMap(numLines);
    std::vector<std::tuple<int, int, int, char>> setAssociative(numLines);
    std::vector<std::tuple<int, int, char>> fullAssociative(numLines); 

    std::ifstream inputFile;
    inputFile.open("TraceFiles/swim.trace");
    if (inputFile.is_open()) {
        std::string temp;
        while (std::getline(inputFile, temp)) {

            temp = temp.substr(4,8);
            std::string tempBin = hexToBin(temp);

            //direct map case
            std::string tag = tempBin.substr(0, tempBin.size() - offsetWidth - lineWidth);
            std::string line = tempBin.substr(tag.size(), lineWidth);
            std::string offset = tempBin.substr(tag.size() + line.size(), offsetWidth);

            //converting tag to decimal for ease
            int tagDec = stoi(tag, 0, 2);

            //Fully associative case
            tag =  tempBin.substr(0, tempBin.size() - offsetWidth);
            offset = tempBin.substr(tag.size(), offsetWidth);

            //converting tag to decimal for ease
            tagDec = stoi(tag, 0, 2);

            //set associative case
            tag = tempBin.substr(0, tempBin.size() - setWidth - offsetWidth);
            std::string cacheSet = tempBin.substr(tag.size(), setWidth);
            offset = tempBin.substr(tag.size() + cacheSet.size(), offsetWidth); 

            //converting tag to decimal for ease
            tagDec = stoi(tag, 0, 2);
        }
        inputFile.close();   
    }
    
    return 0;
}