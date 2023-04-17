#include <bitset>
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <tuple>
#include <vector>

std::string hexToBin(const std::string& s) //function to convert hexadecimal to binary
{
    std::stringstream ss;
    ss << std::hex << s;
    unsigned n;
    ss >> n;
    std::bitset<32> b(n);

    return b.to_string().substr(32 - 4 * (s.length()));
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
    std::cout << "How many lines per set? " << std::endl;
    std::cin >> linesPerSet;

    //data for splitting binary to be stored
    int numLines = bytes / blockSize;
    int offsetWidth = std::log2(blockSize);
    int lineWidth = std::log2(numLines);
    int setWidth = std::log2(numLines / linesPerSet);
    //direct variables
    int directHit = 0;
    int directMiss = 0;

    //full associative variables
    int fullHit = 0;
    int fullMiss = 0;
    int fullHitLRU = 0;
    int fullMissLRU = 0;
    int counter = 0;

    //set associative variables
    int setHit = 0;
    int setMiss = 0;
    int setHitLRU = 0;
    int setMissLRU = 0;

    // data structures for cache implementation
    std::vector<std::tuple<int, int, int, char>> directMap(numLines);
    std::vector<std::tuple<int, int, int, int>> setAssociativeLRU(numLines);
    std::vector<std::tuple<int, int, int, char>> setAssociativeFIFO(numLines);
    std::vector<std::tuple<int, int, int>> fullAssociativeLRU(numLines);
    std::vector<std::tuple<int, int, char>> fullAssociativeFIFO(numLines);

    std::ifstream inputFile;
    inputFile.open("TraceFiles/gcc.trace");
    if (inputFile.is_open()) {
        std::string temp;
        while (std::getline(inputFile, temp)) {

            temp = temp.substr(4, 8);
            std::string tempBin = hexToBin(temp);

            //direct map case
            std::string tag = tempBin.substr(0, tempBin.size() - offsetWidth - lineWidth);
            std::string line = tempBin.substr(tag.size(), lineWidth);
            std::string offset = tempBin.substr(tag.size() + line.size(), offsetWidth);

            //converting tag to decimal for ease
            int tagDec = stoi(tag, 0, 2);

            if (std::get<3>(directMap.at(stoi(line, 0, 2))) == 'F') {
                if (std::get<0>(directMap.at(stoi(line, 0, 2))) == tagDec) {
                    directHit++;
                }
                else {
                    directMap.at(stoi(line, 0, 2)) = (std::tuple<int, int, int, char>(tagDec, stoi(offset), stoi(line), 'F'));
                    directMiss++;
                }
            }
            else {
                directMap.at(stoi(line, 0, 2)) = (std::tuple<int, int, int, char>(tagDec, stoi(offset), stoi(line), 'F'));
                directMiss++;
            }



            //Fully associative case
            tag = tempBin.substr(0, tempBin.size() - offsetWidth);
            offset = tempBin.substr(tag.size(), offsetWidth);

            //converting tag to decimal for ease
            tagDec = stoi(tag, 0, 2);
            //FIFO full associative
            int position = counter % numLines;
            bool found = false;
            for (int i = 0; i < fullAssociativeFIFO.size(); i++) {
                if (std::get<0>(fullAssociativeFIFO.at(i)) == tagDec) {
                    fullHit++;
                    found = true;
                    break;
                }
            }
            if (!found) {
                fullAssociativeFIFO.at(position) = (std::tuple<int, int, char>(tagDec, stoi(offset), 'F'));
                fullMiss++;
            }


            found = false;

            //LRU full associative
            for (int i = 0; i < fullAssociativeLRU.size(); i++) {
                if (std::get<0>(fullAssociativeLRU.at(i)) == tagDec) {
                    fullHitLRU++;
                    std::get<2>(fullAssociativeLRU.at(i)) = counter;
                    found = true;
                    break;
                }
            }
            if (!found) {
                int min = 0;
                for (int i = 0; i < fullAssociativeLRU.size(); i++) {
                    if (std::get<2>(fullAssociativeLRU.at(i)) < std::get<2>(fullAssociativeLRU.at(min))) {
                        min = i;
                    }
                }
                fullAssociativeLRU.at(min) = (std::tuple<int, int, int>(tagDec, stoi(offset), counter));
                fullMissLRU++;
            }

            found = false;
            //set associative case
            tag = tempBin.substr(0, tempBin.size() - setWidth - offsetWidth);
            std::string cacheSet = tempBin.substr(tag.size(), setWidth);
            offset = tempBin.substr(tag.size() + cacheSet.size(), offsetWidth);

            //converting tag to decimal for ease
            tagDec = stoi(tag, 0, 2);

            position = ((stoi(cacheSet, 0, 2)) * linesPerSet);
            int n = linesPerSet;
            //FIFO case
            for (int i = 0; i < n; i++) {
                if (i + position >= setAssociativeFIFO.size()) {
                    n -= i;
                    i = 0;
                }
                if (std::get<0>(setAssociativeFIFO.at(i + position)) == tagDec) {
                    setHit++;
                    found = true;
                    break;
                }
            }
            if (!found) {
                setAssociativeFIFO.at(position + (counter % linesPerSet)) = (std::tuple<int, int, int, char>(tagDec, stoi(offset), stoi(cacheSet), 'F'));
                setMiss++;
            }
            found = false;
            //LRU case
            position = ((stoi(cacheSet, 0, 2)) * linesPerSet);
            n = linesPerSet;
            for (int i = 0; i < n; i++) {
                if (i + position >= setAssociativeLRU.size()) {
                    n -= i;
                    i = 0;
                }
                if (std::get<0>(setAssociativeLRU.at(i + position)) == tagDec) {
                    setHitLRU++;
                    std::get<3>(setAssociativeLRU.at(i + position)) = counter;
                    found = true;
                    break;
                }
            }
            if (!found) {
                n = linesPerSet;
                int min = position;
                for (int i = 0; i < n; i++) {
                    if (i + position >= setAssociativeLRU.size()) {
                        n -= i;
                        i = 0;
                    }
                    if (std::get<3>(setAssociativeLRU.at(i + position)) < std::get<3>(setAssociativeLRU.at(min))) {
                        min = i + position;
                    }
                }
                setAssociativeLRU.at(min) = (std::tuple<int, int, int, char>(tagDec, stoi(offset), stoi(cacheSet), counter));
                setMissLRU++;
            }
            found = false;
            counter++;
        }
        inputFile.close();

        //output stats for each cache
        std::cout << "Direct Associative: " << std::endl;
        std::cout << "Hit: " << directHit << std::endl;
        std::cout << "Miss: " << directMiss << std::endl;
        std::cout << "Hit rate: " << (double)directHit/(double)counter << std::endl;
        std::cout << std::endl;

        std::cout << "Fully Associative FIFO: " << std::endl;
        std::cout << "Hit: " << fullHit << std::endl;
        std::cout << "Miss: " << fullMiss << std::endl;
        std::cout << "Hit rate: " << (float)fullHit/(float)counter << std::endl;
        std::cout << std::endl;

        std::cout << "Fully Associative LRU: " << std::endl;
        std::cout << "Hit: " << fullHitLRU << std::endl;
        std::cout << "Miss: " << fullMissLRU << std::endl;
        std::cout << "Hit rate: " << (float)fullHitLRU/(float)counter << std::endl;
        std::cout << std::endl;

        std::cout << "Set Associative FIFO: " << std::endl;
        std::cout << "Hit: " << setHit << std::endl;
        std::cout << "Miss: " << setMiss << std::endl;
        std::cout << "Hit rate: " << (float)setHit/(float)counter << std::endl;
        std::cout << std::endl;

        std::cout << "Set Associative LRU: " << std::endl;
        std::cout << "Hit: " << setHitLRU << std::endl;
        std::cout << "Miss: " << setMissLRU << std::endl;
        std::cout << "Hit rate: " << (float)setHitLRU/(float)counter << std::endl;
        std::cout << std::endl;
    }

    return 0;
}