// Compiler: VS Code

#include <iostream>
#include <vector>
#include <stdexcept>
#include "scan.h"

using namespace std;

int main() {
    try {
        // Define data sizes to test
        vector<size_t> sizes = { 100000, 1000000, 10000000 };

        for (size_t size : sizes) {
            runExperiments(size);
            cout << "\n" << endl;
        }

    } catch (const exception &e) {
        cerr << "An error has occurred: " << e.what() << "\n";
        return 1;
    } catch (...) {
        cerr << "An unknown error has occurred!\n";
        return 2;
    }
    return 0;
}