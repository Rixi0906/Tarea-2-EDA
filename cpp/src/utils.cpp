#include "utils.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <vector>

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

// =================== RadixSort (LSD) con listas enlazadas O(1) ===================

struct Node { size_t idx; Node* next; Node(size_t i): idx(i), next(nullptr) {} };
struct FastList {
    Node* head = nullptr;
    Node* tail = nullptr;
    inline void push_back(size_t idx){
        Node* nd = new Node(idx);
        if (!head) head = tail = nd;
        else { tail->next = nd; tail = nd; }
    }
    inline void clear(){
        Node* cur = head;
        while (cur){ Node* nx = cur->next; delete cur; cur = nx; }
        head = tail = nullptr;
    }
    ~FastList(){ clear(); }
};

// Counting sort estable por columna p (0..5)
static void counting_sort_by_pos(Poscode* A, size_t n, size_t p){
    const int M = radix_base_at_pos(p); // 10 o 26
    std::vector<FastList> buckets(M);
    for (size_t i = 0; i < n; ++i) {
        int v = bucket_value(A[i], p);
        // Asumimos datos válidos: 0..9 o 0..25
        buckets[v].push_back(i);
    }

    // Reconstrucción estable
    Poscode* out = new Poscode[n];
    size_t k = 0;
    for (int b = 0; b < M; ++b) {
        for (Node* cur = buckets[b].head; cur; cur = cur->next) {
            out[k++] = A[cur->idx];
        }
    }

    // Copiar resultado a A y liberar
    for (size_t i = 0; i < n; ++i) A[i] = out[i];
    delete[] out;
    // Los destructores de FastList liberan los nodos
}

void radix_sort(Poscode *A, size_t n){
    if (n <= 1) return;
    // LSD: de la última posición (5) a la primera (0)
    // Posiciones 4,5 son letras base 26; 0..3 dígitos base 10
    for (int p = 5; p >= 0; --p) {
        counting_sort_by_pos(A, n, static_cast<size_t>(p));
    }
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
