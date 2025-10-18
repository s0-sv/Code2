#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <functional>

using namespace std;

struct Timer {
    chrono::high_resolution_clock::time_point start;

    Timer() {
        start = chrono::high_resolution_clock::now();
    }

    double elapsed() {
        auto end = chrono::high_resolution_clock::now();
        return chrono::duration_cast<chrono::duration<double, milli>>(end - start).count();
    }
};

// Generates a vector of random data
vector<long long> generateData(size_t size);

// Wraps a scan function to measure its execution time
double measureScan(const string& name, const vector<long long>& data, function<vector<long long>()> scanFunc);

double runLibraryScanDefault(const vector<long long>& data);
double runLibraryScanSeq(const vector<long long>& data);
double runLibraryScanPar(const vector<long long>& data);
double runLibraryScanParUnseq(const vector<long long>& data);

struct CustomScanResult {
    int K;
    double timeMs;
};

CustomScanResult runCustomParallelScan(const vector<long long>& data, int K);

void analyzeCustomScan(const vector<CustomScanResult>& results);
void runExperiments(size_t dataSize);