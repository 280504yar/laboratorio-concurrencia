# Laboratorio de programación concurrente y paralela en C++

## Estudiante

Nombre: Yara Izaguirre Pérez

Carné: C33910

## Descripción

Este laboratorio explora conceptos básicos de programación concurrente y paralela usando C++: creación de hilos, condiciones de carrera, mutex, benchmark secuencial vs. paralelo, efecto de la cantidad de hilos en el rendimiento y deadlocks.

## Requisitos

- g++
- C++17 o superior
- Sistema operativo Linux, macOS o Windows con soporte para compilación C++

## Compilación y ejecución

Todos los comandos se ejecutan desde la carpeta `laboratorio-concurrencia/`.

> **En Windows (PowerShell):** agregar `.exe` al nombre del ejecutable y ejecutarlo con `.\`. Ejemplo:
>
> ```powershell
> g++ -std=c++17 -Wall -Wextra -pthread src/parte1_threads.cpp -o parte1.exe
> .\parte1.exe
> ```
>
> Los resultados de `resultados/analisis.md` se obtuvieron en Windows con g++ 16.1 (MSYS2).

### Parte 1: creación de hilos

```bash
g++ -std=c++17 -Wall -Wextra -pthread src/parte1_threads.cpp -o parte1
./parte1
```

### Parte 2: condición de carrera

```bash
g++ -std=c++17 -Wall -Wextra -pthread src/parte2_race_condition.cpp -o parte2
./parte2
```

### Parte 3: corrección con mutex

```bash
g++ -std=c++17 -Wall -Wextra -pthread src/parte3_mutex.cpp -o parte3
./parte3
```

### Parte 4: benchmark secuencial vs. paralelo

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pthread src/parte4_benchmark.cpp -o parte4
./parte4
```

### Parte 5: cantidad de hilos vs. rendimiento

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pthread src/parte5_hilos_vs_rendimiento.cpp -o parte5
./parte5
```

### Parte 6: deadlock

> Este programa se queda bloqueado a propósito. Detenerlo con `Ctrl + C`.

```bash
g++ -std=c++17 -Wall -Wextra -pthread src/parte6_deadlock.cpp -o parte6
./parte6
```

Versión corregida con `std::scoped_lock`:

```bash
g++ -std=c++17 -Wall -Wextra -pthread src/parte6_deadlock_corregido.cpp -o parte6_corregido
./parte6_corregido
```

## Resultados

Las tablas y respuestas de análisis están en [`resultados/analisis.md`](resultados/analisis.md).

## Conclusiones

- Los hilos permiten ejecutar trabajo de forma concurrente, pero el orden de ejecución lo decide el sistema operativo y no es predecible.
- Acceder a una variable compartida sin sincronización produce condiciones de carrera y resultados incorrectos; `std::mutex` con `std::lock_guard` lo corrige, a cambio de un costo en rendimiento.
- Paralelizar una tarea grande (suma de un vector) reduce el tiempo, pero la mejora está limitada por la cantidad de núcleos y por el costo de crear y administrar hilos.
- Tomar varios mutex en distinto orden puede causar un deadlock; `std::scoped_lock` lo evita al adquirirlos todos juntos de forma segura.
