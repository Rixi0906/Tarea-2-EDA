#include "utils.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>
using namespace std;

static inline int valor_bucket(const Poscode& codigo, size_t pos) {
    char caracter = codigo.getValue(pos);
    if (pos <= 3) {
        return static_cast<int>(caracter - '0');
    } else {
        caracter = toupper(static_cast<unsigned char>(caracter));
        return static_cast<int>(caracter - 'A');
    }
}

static inline int base_radix_en_pos(size_t pos) {
    return (pos <= 3) ? 10 : 26;
}

static inline int valor_bucket_rapido(const Poscode& codigo, size_t pos){
    char caracter = codigo.getValue(pos);
    return (pos <= 3) ? static_cast<int>(caracter - '0') : static_cast<int>(caracter - 'A');
}

void quick_sort(Poscode *arreglo, size_t cantidad){
    sort(arreglo, arreglo + cantidad, [](const Poscode& izq, const Poscode& der){
        return izq.getData() < der.getData();
    });
}

static void merge2(Poscode* arreglo, Poscode* auxiliar, size_t idx_izq, size_t idx_der){
    if (idx_der - idx_izq <= 1) return;
    size_t idx_medio = (idx_izq + idx_der) >> 1;
    merge2(arreglo, auxiliar, idx_izq, idx_medio);
    merge2(arreglo, auxiliar, idx_medio, idx_der);
    size_t idx_i = idx_izq, idx_j = idx_medio, idx_k = idx_izq;
    while (idx_i < idx_medio && idx_j < idx_der) {
        if (arreglo[idx_i].getData() <= arreglo[idx_j].getData())
            auxiliar[idx_k++] = arreglo[idx_i++];
        else
            auxiliar[idx_k++] = arreglo[idx_j++];
    }
    while (idx_i < idx_medio) auxiliar[idx_k++] = arreglo[idx_i++];
    while (idx_j < idx_der)   auxiliar[idx_k++] = arreglo[idx_j++];
    for (size_t idx_t = idx_izq; idx_t < idx_der; ++idx_t)
        arreglo[idx_t] = auxiliar[idx_t];
}

void merge_sort(Poscode *arreglo, size_t cantidad){
    if (cantidad <= 1) return;
    Poscode* auxiliar = new Poscode[cantidad];
    merge2(arreglo, auxiliar, 0, cantidad);
    delete[] auxiliar;
}
void radix_sort(Poscode *arreglo, size_t cantidad){
    if (cantidad <= 1) return;

    using namespace std;

    auto clave16 = [&](const Poscode& c, size_t hi, size_t lo)->int{
        unsigned a = (unsigned)(unsigned char)c.getValue(hi);
        unsigned b = (unsigned)(unsigned char)c.getValue(lo);
        return (int)((a<<8) | b);
    };

    vector<int> idx(cantidad), tmp(cantidad);
    iota(idx.begin(), idx.end(), 0);

    vector<int> conteo(65536), offset(65536);

    auto pasada = [&](size_t hi, size_t lo){
        fill(conteo.begin(), conteo.end(), 0);
        for (size_t k=0; k<cantidad; ++k) conteo[ clave16(arreglo[idx[k]], hi, lo) ]++;
        offset[0] = 0;
        for (int v=1; v<65536; ++v) offset[v] = offset[v-1] + conteo[v-1];
        for (size_t k=0; k<cantidad; ++k){
            int id = idx[k];
            int v  = clave16(arreglo[id], hi, lo);
            tmp[ offset[v]++ ] = id;
        }
        idx.swap(tmp);
    };

    pasada(4,5);
    pasada(2,3);
    pasada(0,1);

    vector<Poscode> buf(cantidad);
    for (size_t i=0; i<cantidad; ++i) buf[i] = arreglo[idx[i]];
    for (size_t i=0; i<cantidad; ++i) arreglo[i] = std::move(buf[i]);
}

Poscode *readCodes(const string &ruta_archivo, size_t cantidad){
    Poscode *codigos = new Poscode[cantidad];
    ifstream archivo(ruta_archivo);

    if (!archivo.is_open()) {
        cerr << "Error: Unable to open the file!\n";
        delete[] codigos;
        return nullptr;
    }
    string linea;
    size_t idx = 0;
    for (; idx < cantidad && getline(archivo, linea); ++idx){
        codigos[idx] = Poscode(linea);
    }
    for (; idx < cantidad; ++idx)
        codigos[idx] = Poscode("");
    return codigos;
}

void deleteCodes(Poscode *codigos){
    delete[] codigos;
}
