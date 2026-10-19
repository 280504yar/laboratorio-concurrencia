#include <iostream>
#include <thread>
#include <mutex>
#include <string>

std::mutex recurso1;
std::mutex recurso2;

void tarea(const std::string& nombre) {
    // scoped_lock bloquea ambos mutex a la vez con un algoritmo que evita deadlock
    std::scoped_lock lock(recurso1, recurso2);
    std::cout << nombre << " obtuvo ambos recursos de forma segura." << std::endl;
}

int main() {
    std::thread a(tarea, "Hilo A");
    std::thread b(tarea, "Hilo B");
    a.join();
    b.join();
    return 0;
}
