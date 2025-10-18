#include "scan.h"
#include <random>
#include <numeric>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <stdexcept>

// Generates a vector of random data
vector<long long> generateData(size_t size) {
    if (size == 0) return {};
    
    vector<long long> data(size);
    mt19937 gen(random_device{}());
    uniform_int_distribution<long long> dist(1, 100);

    for (size_t i = 0; i < size; ++i) {
        data[i] = dist(gen);
    }
    return data;
}

// Wraps a scan function to measure its execution time
double measureScan(const string& name, const vector<long long>& data, function<vector<long long>()> scanFunc) {
    cout << "Running: " << left << setw(30) << name << " | Size: " << data.size() << " | ";
    
    Timer timer;
    vector<long long> result = scanFunc(); // Execute the scan
    double time = timer.elapsed();

    cout << "Time: " << fixed << setprecision(4) << time << " ms" << endl;
    
    return time;
}


double runLibraryScanDefault(const vector<long long>& data) {
    cout << "Running: Library Scan (Default) - NOT IMPLEMENTED" << endl;
    return 0.0;
}
double runLibraryScanSeq(const vector<long long>& data) {
    cout << "Running: Library Scan (seq) - NOT IMPLEMENTED" << endl;
    return 0.0;
}
double runLibraryScanPar(const vector<long long>& data) {
    cout << "Running: Library Scan (par) - NOT IMPLEMENTED" << endl;
    return 0.0;
}
double runLibraryScanParUnseq(const vector<long long>& data) {
    cout << "Running: Library Scan (par_unseq) - NOT IMPLEMENTED" << endl;
    return 0.0;
}


CustomScanResult runCustomParallelScan(const vector<long long>& data, int K) {
    cout << "Running: Custom Parallel Scan (K=" << K << ") - NOT IMPLEMENTED" << endl;
    return {K, 0.0};
}

void analyzeCustomScan(const vector<CustomScanResult>& results) {
     cout << "Analysis - NOT IMPLEMENTED" << endl;
}

void runExperiments(size_t dataSize) {
    cout << "Starting experiments for data size: " << dataSize << endl;

    auto data = generateData(dataSize);
    if (data.empty()) {
        cout << "Data size is zero, skipping experiments." << endl;
        return;
    }

    cout << "--- Library Algorithm Tests ---" << endl;
    runLibraryScanDefault(data);
    runLibraryScanSeq(data);
    runLibraryScanPar(data);
    runLibraryScanParUnseq(data);

    cout << "\n--- Custom Parallel Scan Analysis ---" << endl;
    
    runCustomParallelScan(data, 1);

    analyzeCustomScan({}); // Pass empty results
}