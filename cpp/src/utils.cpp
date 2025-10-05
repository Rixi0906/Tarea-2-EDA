#include "utils.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <vector>
#include <numeric> 
// =================== Helpers comunes ===================

// Mapea el carácter de la posición p al valor de bucket.
// Asumo formato: 4 dígitos (0..9) + 2 letras (A..Z). Largo = 6.
// p es 0..5 (0 = primer char)
static inline int bucket_value(const Poscode& pc, size_t p) {
    char c = pc.getValue(p);
    if (p <= 3) {
        // dígitos
        return int(c - '0'); // asume '0'..'9'
    } else {
        // letras
        c = std::toupper(static_cast<unsigned char>(c));
        return int(c - 'A'); // asume 'A'..'Z'
    }
}

static inline int radix_base_at_pos(size_t p) {
    return (p <= 3) ? 10 : 26;
}

// =================== QuickSort ===================

void quick_sort(Poscode *A, size_t n){
    // Orden lexicográfico por el string completo
    std::sort(A, A + n, [](const Poscode& a, const Poscode& b){
        return a.getData() < b.getData();
    });
}

// =================== MergeSort (estable) ===================

static void merge_rec(Poscode* A, Poscode* aux, size_t l, size_t r){
    if (r - l <= 1) return;
    size_t m = (l + r) >> 1;
    merge_rec(A, aux, l, m);
    merge_rec(A, aux, m, r);
    size_t i = l, j = m, k = l;
    while (i < m && j < r) {
        if (A[i].getData() <= A[j].getData()) aux[k++] = A[i++];
        else                                    aux[k++] = A[j++];
    }
    while (i < m) aux[k++] = A[i++];
    while (j < r) aux[k++] = A[j++];
    for (size_t t = l; t < r; ++t) A[t] = aux[t];
}

void merge_sort(Poscode *A, size_t n){
    if (n <= 1) return;
    // Auxiliar en heap para no reventar stack en n grande
    Poscode* aux = new Poscode[n];
    merge_rec(A, aux, 0, n);
    delete[] aux;
}


// p: 0..5  (0..3 dígitos, 4..5 letras en MAYÚSCULAS)
static inline int bucket_value_fast(const Poscode& pc, size_t p){
    char c = pc.getValue(p);
    return (p <= 3) ? (int)(c - '0') : (int)(c - 'A');
}

void radix_sort(Poscode *A, size_t n){
    if (n <= 1) return;

    // Ordenamos ÍNDICES en cada pasada; copiamos objetos UNA sola vez al final.
    std::vector<int> I(n), J(n);
    std::iota(I.begin(), I.end(), 0);

    auto pass_idx = [&](size_t p, int M){
        // 1) contar
        int cnt[26] = {0};
        for (size_t k=0; k<n; ++k){
            cnt[ bucket_value_fast(A[I[k]], p) ]++;
        }
        // 2) offsets (prefix sum -> territorio contiguo por bucket)
        int pos[26];
        pos[0] = 0;
        for (int b=1; b<M; ++b) pos[b] = pos[b-1] + cnt[b-1];
        // 3) distribución estable
        for (size_t k=0; k<n; ++k){
            int idx = I[k];
            int v = bucket_value_fast(A[idx], p);
            J[ pos[v]++ ] = idx; // O(1) “push” por bucket
        }
        I.swap(J);
    };

    // Pasadas LSD: letras 5,4 (26) y dígitos 3..0 (10)
    pass_idx(5, 26);
    pass_idx(4, 26);
    pass_idx(3, 10);
    pass_idx(2, 10);
    pass_idx(1, 10);
    pass_idx(0, 10);

    // Reordenamos los objetos UNA vez según I
    std::vector<Poscode> B(n);
    for (size_t i=0;i<n;++i) B[i] = A[I[i]];
    for (size_t i=0;i<n;++i) A[i] = B[i];
}


// =================== I/O ===================

Poscode *readCodes(const std::string &strfile, size_t n){
    Poscode *codes = new Poscode[n];
    std::ifstream inputFile(strfile);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the file!\n";
        delete[] codes;
        return nullptr;
    }
    std::string line;
    size_t i = 0;
    for (; i < n && std::getline(inputFile, line); ++i){
        // Opcional: validar largo==6, pero mantenemos tus supuestos
        codes[i] = Poscode(line);
    }
    // Si el archivo tuvo menos de n líneas, completamos con strings vacíos
    for (; i < n; ++i) codes[i] = Poscode("");
    return codes;
}

void deleteCodes(Poscode *codes){
    delete[] codes;
}
