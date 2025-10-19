#include "scan.h"
#include <random>
#include <numeric>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <stdexcept>
#include <execution>

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
    auto scanLambda = [&]() {
        vector<long long> result(data.size());
        exclusive_scan(data.begin(), data.end(), result.begin(), 0LL);
        return result;
    };
    return measureScan("Library Scan (Default)", data, scanLambda);
}
double runLibraryScanSeq(const vector<long long>& data) {
    auto scanLambda = [&]() {
        vector<long long> result(data.size());
        exclusive_scan(execution::seq, data.begin(), data.end(), result.begin(), 0LL);
        return result;
    };
    return measureScan("Library Scan (seq)", data, scanLambda);
}
double runLibraryScanPar(const vector<long long>& data) {
    auto scanLambda = [&]() {
        vector<long long> result(data.size());
        exclusive_scan(execution::par, data.begin(), data.end(), result.begin(), 0LL);
        return result;
    };
    return measureScan("Library Scan (par)", data, scanLambda);
}
double runLibraryScanParUnseq(const vector<long long>& data) {
    auto scanLambda = [&]() {
        vector<long long> result(data.size());
        exclusive_scan(execution::par_unseq, data.begin(), data.end(), result.begin(), 0LL);
        return result;
    };
    return measureScan("Library Scan (par_unseq)", data, scanLambda);
}


CustomScanResult runCustomParallelScan(const vector<long long>& data, int K) {
    string name = "Custom Parallel Scan (K=" + to_string(K) + ")";
    
    auto scanLambda = [&, K, name]() {
        vector<long long> result(data.size());
        if (data.empty()) return result;

        vector<long long> partialSums(K, 0LL);
        vector<thread> threads;
        
        size_t chunkSize = (data.size() + K - 1) / K;

        // Local scans and partial sums
        for (int i = 0; i < K; ++i) {
            threads.emplace_back([&, i] {
                size_t start = i * chunkSize;
                size_t end = min(start + chunkSize, data.size());
                
                if (start >= end) return;

                exclusive_scan(data.begin() + start, data.begin() + end, result.begin() + start, 0LL);
                
                partialSums[i] = reduce(data.begin() + start, data.begin() + end, 0LL);
            });
        }
        for (auto& t : threads) t.join();
        threads.clear();

        // Sequential scan of partial sums
        vector<long long> offsets(K, 0LL);
        exclusive_scan(partialSums.begin(), partialSums.end(), offsets.begin(), 0LL);

        // Apply offsets
        for (int i = 0; i < K; ++i) {
            threads.emplace_back([&, i] {
                size_t start = i * chunkSize;
                size_t end = min(start + chunkSize, data.size());

                if (start >= end || offsets[i] == 0) return;

                for (size_t j = start; j < end; ++j) {
                    result[j] += offsets[i];
                }
            });
        }
        for (auto& t : threads) t.join();

        return result;
    };
    
    double time = measureScan(name, data, scanLambda);
    return {K, time};
}

void analyzeCustomScan(const vector<CustomScanResult>& results) {
    if (results.empty()) {
        cout << "No custom scan results to analyze." << endl;
        return;
    }

    cout << "\nCustom Algorithm Performance Table:" << endl;
    cout << "-----------------------------------" << endl;
    cout << "| " << left << setw(5) << "K" << " | " << setw(18) << "Time (ms)" << " |" << endl;
    cout << "-----------------------------------" << endl;

    // Find best K
    auto bestResult = min_element(results.begin(), results.end(), 
        [](const CustomScanResult& a, const CustomScanResult& b) {
            return a.timeMs < b.timeMs;
        });

    for (const auto& res : results) {
        cout << "| " << left << setw(5) << res.K 
             << " | " << setw(18) << fixed << setprecision(4) << res.timeMs << " |";
        if (res.K == bestResult->K) {
            cout << " <- BEST";
        }
        cout << endl;
    }
    cout << "-----------------------------------" << endl;

    unsigned int numCores = thread::hardware_concurrency();
    cout << "\nSummary:" << endl;
    cout << "Best performance achieved at K = " << bestResult->K << " with " << bestResult->timeMs << " ms." << endl;
    
    if (numCores > 0) {
        cout << "Hardware concurrency: " << numCores << endl;
        cout << "Best K vs Cores Ratio: " << fixed << setprecision(2) 
             << (static_cast<double>(bestResult->K) / numCores) << endl;
        if (bestResult->K == numCores) {
            cout << "Best K is equal to the number of hardware threads." << endl;
        } else if (bestResult->K < numCores) {
            cout << "Best K is less than the number of hardware threads." << endl;
        } else {
            cout << "Best K is greater than the number of hardware threads." << endl;
        }
    } else {
        cout << "Could not determine hardware concurrency." << endl;
    }
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
    
    unsigned int numCores = thread::hardware_concurrency();
    cout << "Detected " << numCores << " hardware threads." << endl;
    
    int maxK = numCores > 0 ? numCores * 2 : 16; // Test up to 2x cores
    vector<CustomScanResult> customResults;

    for (int k = 1; k <= maxK; ++k) {
        customResults.push_back(runCustomParallelScan(data, k));
    }

    analyzeCustomScan(customResults);
}