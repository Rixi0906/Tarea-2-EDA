#include "poscode.hpp"
#include "utils.hpp"
#include <string>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <vector>

// función auxiliar para medir tiempo
double time_ms(void (*fn)(Poscode*, size_t), Poscode* data, size_t n){
    auto start = std::chrono::high_resolution_clock::now();
    fn(data, n);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// función auxiliar para clonar arreglo
Poscode* clone(Poscode* src, size_t n){
    Poscode* dst = new Poscode[n];
    for(size_t i=0;i<n;++i) dst[i] = src[i];
    return dst;
}

// chequea si está ordenado lexicográficamente;
bool is_sorted(Poscode* A, size_t n){
    for(size_t i=1;i<n;++i)
        if (A[i-1].getData() > A[i].getData()) return false;
    return true;
}

int main(int argc, char** argv){
    if (argc < 3){
        std::cerr << "Uso: " << argv[0] << " <ruta_dataset> <n_lineas>\n";
        return 1;
    }
    std::string strfile = argv[1];
    size_t n = std::stoull(argv[2]);

    Poscode *data = readCodes(strfile, n);
    if(!data){ std::cerr << "Error al leer archivo\n"; return 1; }

    std::cout << "Primeros 10 códigos leídos:\n";
    for(size_t i = 0; i < 10; i++)
        std::cout << data[i].getData() << "\n";

    std::cout << "\n===== BENCHMARK =====\n";
    const int runs = 5;

    struct Result { std::string name; double mean; double stdev; };
    std::vector<Result> results;

    // lista de funciones a probar
    struct Method { const char* name; void (*fn)(Poscode*, size_t); };
    std::vector<Method> methods = {
        {"quick_sort", quick_sort},
        {"merge_sort", merge_sort},
        {"radix_sort", radix_sort}
    };

    for (auto &m : methods) {
        std::vector<double> times;
        times.reserve(runs);

        for (int r=0; r<runs; ++r) {
            Poscode* copy = clone(data, n);
            double ms = time_ms(m.fn, copy, n);
            if (!is_sorted(copy, n))
                std::cerr << "⚠️  " << m.name << " no ordenó correctamente\n";
            delete[] copy;
            times.push_back(ms);
        }

        double sum=0, sum2=0;
        for (double t : times){ sum+=t; sum2+=t*t; }
        double mean = sum/runs;
        double var = (sum2 - runs*mean*mean)/(runs>1? runs-1:1);
        double sd = std::sqrt(var);

        results.push_back({m.name, mean, sd});
    }

    for (auto &r : results)
        std::cout << std::fixed << std::setprecision(2)
                  << r.name << ": " << r.mean << " ± " << r.stdev << " ms\n";

    deleteCodes(data);
    return 0;
}
