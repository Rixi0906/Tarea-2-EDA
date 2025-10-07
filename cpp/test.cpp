#include "poscode.hpp"
#include "utils.hpp"
#include <string>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <vector>

using namespace std;

double time_ms(void (*fn)(Poscode*, size_t), Poscode* data, size_t n){
    auto t0 = chrono::high_resolution_clock::now();
    fn(data, n);
    auto t1 = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(t1 - t0).count();
}

Poscode* clone(Poscode* src, size_t n){
    Poscode* dst = new Poscode[n];
    for(size_t i=0;i<n;++i) dst[i] = src[i];
    return dst;
}

int main(int argc, char** argv){
    if (argc < 3) return 1;
    string ruta = argv[1];
    size_t n = stoull(argv[2]);

    Poscode *data = readCodes(ruta, n);
    if(!data) return 1;

    const int runs = 5;

    struct Result { string name; double mean; double stdev; };
    vector<Result> results;

    struct Method { const char* name; void (*fn)(Poscode*, size_t); };
    vector<Method> methods = {
        {"quick_sort", quick_sort},
        {"merge_sort", merge_sort},
        {"radix_sort", radix_sort}
    };

    for (auto &m : methods) {
        vector<double> times; times.reserve(runs);
        for (int r=0; r<runs; ++r) {
            Poscode* copy = clone(data, n);
            double ms = time_ms(m.fn, copy, n);
            delete[] copy;
            times.push_back(ms);
        }
        double sum=0, sum2=0;
        for (double t : times){ sum+=t; sum2+=t*t; }
        double mean = sum/runs;
        double var  = (sum2 - runs*mean*mean)/(runs>1? runs-1:1);
        double sd   = sqrt(var);
        results.push_back({m.name, mean, sd});
    }

    cout << fixed << setprecision(2);
    cout << "---------------------------------------------\n";
    cout << " Algoritmo  / Promedio (ms)   / Desviacion \n";
    cout << "---------------------------------------------\n";
    for (auto &r : results){
        cout << " " << left << setw(10) << r.name
             << " " << right << setw(15) << r.mean
             << " " << right << setw(10) << r.stdev
             << "\n";
    }
    cout << "---------------------------------------------\n";

    deleteCodes(data);
    return 0;
}
