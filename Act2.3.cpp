#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;

struct Registro {
    string mes;
    int dia;
    string hora;
    string ip;
    string motivo;
};

// Función para leer registros desde un archivo
vector<Registro> leerBitacora(const string& nombreArchivo) {
    vector<Registro> bitacora;
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "El archivo no se encontró" << endl;
        return bitacora;
    }
    string linea;

    while (getline(archivo, linea)) {
        istringstream stream(linea);
        Registro reg;
        stream >> reg.mes >> reg.dia >> reg.hora >> reg.ip;
        getline(stream, reg.motivo);
        bitacora.push_back(reg);
    }
    archivo.close();
    return bitacora;
}

// Función para descomponer la IP
vector<int> descomponerIP(const string& ip) {
    vector<int> partes(4, 0);  // Inicializa con ceros
    int prevPos = 0, pos = 0;
    int octetoIndex = 0;

    while ((pos = ip.find('.', prevPos)) != string::npos && octetoIndex < 4) {
        partes[octetoIndex++] = stoi(ip.substr(prevPos, pos - prevPos));  // Convertir a entero
        prevPos = pos + 1;
    }

    // Último octeto
    if (octetoIndex < 4) {
        partes[octetoIndex] = stoi(ip.substr(prevPos));  // Último octeto
    }

    return partes;
}

// Función para comparar las IPs
bool compararIPs(const string& ip1, const string& ip2) {
    vector<int> partes1 = descomponerIP(ip1);
    vector<int> partes2 = descomponerIP(ip2);

    // Comparar cada parte de la IP numéricamente
    for (int i = 0; i < 4; ++i) {
        if (partes1[i] < partes2[i]) return true;   // ip1 es menor
        if (partes1[i] > partes2[i]) return false;  // ip1 es mayor
    }
    return false;  // Si son iguales, devolver false
}

// Función para fusionar dos subvectores (para IPs)
void merge(vector<Registro>& bitacora, int inicio, int medio, int fin) {
    int n1 = medio - inicio + 1;
    int n2 = fin - medio;

    vector<Registro> izquierda(n1);
    vector<Registro> derecha(n2);

    for (int i = 0; i < n1; ++i) {
        izquierda[i] = bitacora[inicio + i];
    }
    for (int j = 0; j < n2; ++j) {
        derecha[j] = bitacora[medio + 1 + j];
    }

    int i = 0, j = 0, k = inicio;

    while (i < n1 && j < n2) {
        // Comparar las IPs
        if (compararIPs(izquierda[i].ip, derecha[j].ip)) {
            bitacora[k] = izquierda[i];
            i++;
        } else {
            bitacora[k] = derecha[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        bitacora[k] = izquierda[i];
        i++;
        k++;
    }

    while (j < n2) {
        bitacora[k] = derecha[j];
        j++;
        k++;
    }
}

// Función recursiva de Merge Sort
void mergeSort(vector<Registro>& bitacora, int inicio, int fin) {
    if (inicio < fin) {
        int medio = inicio + (fin - inicio) / 2;
        mergeSort(bitacora, inicio, medio);
        mergeSort(bitacora, medio + 1, fin);
        merge(bitacora, inicio, medio, fin);
    }
}

// Función para ordenar la bitácora por IP usando Merge Sort
void ordenarBitacoraIP(vector<Registro>& bitacora, const string& nombreArchivo) {
    mergeSort(bitacora, 0, bitacora.size() - 1); // Ordenar por IP

    ofstream archivo(nombreArchivo);
    for (const auto& registro : bitacora) {
        archivo << registro.mes << " " << registro.dia << " " << registro.hora << " "
                << registro.ip << " " << registro.motivo << endl;
    }
    archivo.close();
}

// Función para contar y ordenar puertos
// Función para contar y ordenar puertos
void ordenarYContarPuertos(const vector<Registro>& bitacora, const string& nombreArchivo) {
    map<int, int> puertoConteo;  // Mapa para contar la cantidad de veces que se repite cada puerto
    map<int, vector<Registro>> registrosPorPuerto; // Mapa para almacenar registros por puerto

    // Extraer los puertos y contar sus ocurrencias
    for (const auto& registro : bitacora) {
        int pos = registro.ip.find(':');
        if (pos != string::npos) {
            int puerto = stoi(registro.ip.substr(pos + 1));  // Extraer el puerto
            puertoConteo[puerto]++;  // Contar el puerto
            registrosPorPuerto[puerto].push_back(registro); // Almacenar el registro asociado al puerto
        }
    }

    // Convertir el mapa a un vector de pares para ordenarlo por la cantidad de repeticiones
    vector<pair<int, int>> conteoPuertos(puertoConteo.begin(), puertoConteo.end());

    // Ordenar por la cantidad de repeticiones en orden descendente
    sort(conteoPuertos.begin(), conteoPuertos.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        return a.second > b.second;  // Comparar por el número de repeticiones (descendente)
    });

    // Escribir los resultados en el archivo
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo para escribir." << endl;
        return;
    }

    archivo << "Puerto - Repeticiones - Detalles de Registros" << endl;
    for (const auto& entry : conteoPuertos) {
        int puerto = entry.first;
        int repeticiones = entry.second;

        archivo << "Puerto: " << puerto << " - Repeticiones: " << repeticiones << endl;

        // Agregar los detalles de los registros asociados al puerto
        for (const auto& registro : registrosPorPuerto[puerto]) {
            archivo << "   " << registro.mes << " " << registro.dia << " " << registro.hora
                    << " " << registro.ip << " " << registro.motivo << endl;
        }
    }

    archivo.close();
}

// Funciones para filtrar por IP
bool esMayorOIgualIP(const string& ip1, const string& ip2) {
    vector<int> partes1 = descomponerIP(ip1);
    vector<int> partes2 = descomponerIP(ip2);
    
    for (int i = 0; i < 4; ++i) {
        if (partes1[i] < partes2[i]) return false;
        if (partes1[i] > partes2[i]) return true;
    }
    return true;  // Si son iguales, devolver true
}

bool esMenorOIgualIP(const string& ip1, const string& ip2) {
    vector<int> partes1 = descomponerIP(ip1);
    vector<int> partes2 = descomponerIP(ip2);
    
    for (int i = 0; i < 4; ++i) {
        if (partes1[i] > partes2[i]) return false;
        if (partes1[i] < partes2[i]) return true;
    }
    return true;  // Si son iguales, devolver true
}

void filtrarPorIP(const vector<Registro>& bitacora, const string& ipInicio, const string& ipFin) {
    vector<Registro> registrosFiltrados;  // Para almacenar registros filtrados

    for (const auto& registro : bitacora) {
        if (esMayorOIgualIP(registro.ip, ipInicio) && esMenorOIgualIP(registro.ip, ipFin)) {
            registrosFiltrados.push_back(registro);
        }
    }

    // Ordenar los registros filtrados por IP antes de mostrarlos
    mergeSort(registrosFiltrados, 0, registrosFiltrados.size() - 1);

    cout << "Registros entre las IPs " << ipInicio << " y " << ipFin << ":" << endl;
    for (const auto& registro : registrosFiltrados) {
        cout << registro.mes << " " << registro.dia << " " << registro.hora << " "
             << registro.ip << " " << registro.motivo << endl;
    }
}

int main() {
    vector<Registro> bitacora = leerBitacora("D:/PROG/Proyectos/TC1031(Portafolio_Final)/bitacora.txt");
        ordenarBitacoraIP(bitacora, "bitacoraIPOrdenada.txt");
        cout << "La bitácora ha sido ordenada por IP y guardada en 'bitacoraIPOrdenada.txt'." << endl;
        ordenarYContarPuertos(bitacora, "puertosOrdenados.txt");
        cout << "El archivo con los puertos ordenados y su numero de repeticiones."<<endl;
        string ipInicio, ipFin;
        cout << "Ingrese la IP de inicio (ejemplo: 192.168.0.1): ";
        cin >> ipInicio;
        cout << "Ingrese la IP de fin (ejemplo: 192.168.0.255): ";
        cin >> ipFin;
        filtrarPorIP(bitacora, ipInicio, ipFin); // Llamamos a la función de filtrado por IP

    return 0;
}