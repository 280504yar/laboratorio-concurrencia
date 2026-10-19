# Análisis de resultados

Equipo: Windows, g++ 16.1 (MSYS2), 8 núcleos lógicos.

---

## Parte 1: creación básica de hilos

Compilación y ejecución (5 veces):

```powershell
g++ -std=c++17 -Wall -Wextra -pthread src/parte1_threads.cpp -o parte1.exe
1..5 | ForEach-Object { "== Ejecucion $_ =="; .\parte1.exe }
```

Salida obtenida:

```
== Ejecucion 1 ==
Hola desde el hilo Hola desde el hilo 4
Hola desde el hilo 3
Hola desde el hilo 2Hola desde el hilo 1
0

Todos los hilos finalizaron.
== Ejecucion 2 ==
Hola desde el hilo Hola desde el hilo 2
Hola desde el hilo 4
Hola desde el hilo 3
0
Hola desde el hilo 1
Todos los hilos finalizaron.
== Ejecucion 3 ==
Hola desde el hilo Hola desde el hilo 1
0
Hola desde el hilo 2
Hola desde el hilo 3
Hola desde el hilo 4
Todos los hilos finalizaron.
== Ejecucion 4 ==
Hola desde el hilo Hola desde el hilo 1Hola desde el hilo 3
Hola desde el hilo 2
Hola desde el hilo 4

0
Todos los hilos finalizaron.
== Ejecucion 5 ==
Hola desde el hilo Hola desde el hilo 3
0
Hola desde el hilo 2
Hola desde el hilo 1
Hola desde el hilo 4
Todos los hilos finalizaron.
```

1. **¿Los mensajes aparecen siempre en el mismo orden?**
   No. En las 5 ejecuciones el orden fue distinto, y en todas los mensajes se mezclaron: el "0" del hilo 0 quedó separado de su texto y aparece en otra línea.

2. **¿Por qué podría cambiar el orden de impresión?**
   Porque el planificador del sistema operativo decide cuándo corre cada hilo, y eso varía en cada ejecución. Además, `std::cout` es compartido y cada `<<` es una operación separada, así que un hilo puede escribir en medio del mensaje de otro.

3. **¿Qué función cumple `join()`?**
   Hace que el hilo principal espere hasta que el hilo termine su ejecución. Por eso "Todos los hilos finalizaron." siempre sale de último.

4. **¿Qué podría pasar si no se llama a `join()`?**
   Al destruirse un `std::thread` que sigue siendo "joinable", el programa llama a `std::terminate()` y aborta. Además, `main` podría terminar antes que los hilos y sus mensajes no aparecerían.

---

## Parte 2: condición de carrera

Compilación y ejecución (5 veces):

```powershell
g++ -std=c++17 -Wall -Wextra -pthread src/parte2_race_condition.cpp -o parte2.exe
1..5 | ForEach-Object { "== Ejecucion $_ =="; .\parte2.exe }
```

| Ejecución | Valor esperado | Valor obtenido |
|-----------|----------------|----------------|
| 1 | 4000000 | 1379090 |
| 2 | 4000000 | 1228919 |
| 3 | 4000000 | 1560737 |
| 4 | 4000000 | 1375405 |
| 5 | 4000000 | 1400398 |

En todas las ejecuciones se obtuvo solo entre el 30 % y el 39 % del valor esperado; es decir, se perdieron más de la mitad de los incrementos.

1. **¿El valor obtenido siempre coincide con el valor esperado?**
   No. Siempre fue mucho menor que 4000000 y distinto en cada ejecución.

2. **¿Por qué se pierden incrementos?**
   Dos hilos leen el mismo valor del contador, ambos le suman 1 y ambos escriben el mismo resultado. Uno de los dos incrementos se sobrescribe y se pierde.

3. **¿La operación `contador++` es realmente una sola operación a nivel de CPU?**
   No. Son tres pasos: leer el valor de memoria a un registro, sumarle 1 y escribirlo de vuelta. Otro hilo puede intervenir entre esos pasos.

4. **¿Qué problema de concurrencia se está observando?**
   Una condición de carrera (*race condition*) sobre una variable compartida sin sincronización.


---

## Parte 3: corrección usando mutex

Compilación y ejecución (5 veces):

```powershell
g++ -std=c++17 -Wall -Wextra -pthread src/parte3_mutex.cpp -o parte3.exe
1..5 | ForEach-Object { "== Ejecucion $_ =="; .\parte3.exe }
```

| Ejecución | Valor esperado | Valor obtenido |
|-----------|----------------|----------------|
| 1 | 4000000 | 4000000 |
| 2 | 4000000 | 4000000 |
| 3 | 4000000 | 4000000 |
| 4 | 4000000 | 4000000 |
| 5 | 4000000 | 4000000 |

### Comparación con la parte 2

| Ejecución | Parte 2 (sin mutex) | Parte 3 (con mutex) |
|-----------|---------------------|---------------------|
| 1 | 1379090 | 4000000 |
| 2 | 1228919 | 4000000 |
| 3 | 1560737 | 4000000 |
| 4 | 1375405 | 4000000 |
| 5 | 1400398 | 4000000 |

**Confirmación:** con el mutex, el valor obtenido coincide con el esperado en las 5 ejecuciones. Sin mutex nunca coincidió.

1. **¿Qué cambió con respecto al programa anterior?**
   Se agregó un `std::mutex` global y cada incremento se protege con `std::lock_guard`.

2. **¿Qué hace `std::mutex`?**
   Garantiza exclusión mutua: solo un hilo a la vez puede tenerlo bloqueado; los demás esperan hasta que se libere.

3. **¿Qué hace `std::lock_guard`?**
   Bloquea el mutex al construirse y lo libera automáticamente al salir del alcance (RAII), incluso si ocurre una excepción.

4. **¿Por qué ahora el resultado sí debería ser correcto?**
   Porque la lectura, suma y escritura de `contador++` se hacen completas por un solo hilo a la vez; ningún incremento se sobrescribe.

5. **¿Qué desventaja podría tener proteger cada incremento individual con un mutex?**
   Es lento: se bloquea y libera el mutex 4 millones de veces, y los hilos pasan mucho tiempo esperando. En la práctica el programa queda casi secuencial. Sería mejor que cada hilo sume en una variable local y bloquee una sola vez al final, o usar `std::atomic<int>`.

---

## Parte 4: mini benchmark secuencial vs. paralelo (4 hilos)

Compilación y ejecución (3 veces):

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -pthread src/parte4_benchmark.cpp -o parte4.exe
1..3 | ForEach-Object { "== Ejecucion $_ =="; .\parte4.exe }
```

| Ejecución | Tiempo secuencial | Tiempo paralelo | ¿Cuál fue más rápido? |
|-----------|-------------------|-----------------|-----------------------|
| 1 | 76 ms | 20 ms | Paralelo (3,8×) |
| 2 | 81 ms | 30 ms | Paralelo (2,7×) |
| 3 | 82 ms | 28 ms | Paralelo (2,9×) |

En promedio la versión paralela fue unas 3 veces más rápida (≈80 ms vs. ≈26 ms).

1. **¿El resultado secuencial y el paralelo son iguales?**
   Sí, ambos dan 100000000 en todas las ejecuciones.

2. **¿La versión paralela siempre fue más rápida?**
   Sí, entre 2,7 y 3,8 veces más rápida. No llega exactamente a 4 veces (el número de hilos) por el costo de crear los hilos y porque todos compiten por el acceso a memoria.

3. **¿Por qué dividir el vector en bloques permite paralelizar el trabajo?**
   Cada bloque es independiente: cada hilo suma su propio rango y guarda el resultado en su propia posición de `resultados`. No comparten datos que se modifiquen, así que no necesitan mutex.

4. **¿Qué costos adicionales tiene la versión paralela?**
   Crear y destruir hilos, esperar con `join()`, combinar los resultados parciales y el *context switching*. También compiten por el ancho de banda de memoria.

5. **¿Qué podría pasar si el vector fuera muy pequeño?**
   El costo de crear los hilos sería mayor que el trabajo útil, y la versión paralela sería más lenta que la secuencial.

---

## Parte 5: cantidad de hilos vs. rendimiento

Compilación y ejecución (3 veces):

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -pthread src/parte5_hilos_vs_rendimiento.cpp -o parte5.exe
1..3 | ForEach-Object { "== Ejecucion $_ =="; .\parte5.exe }
```

Núcleos lógicos detectados: **8**

| Cantidad de hilos | Ejecución 1 | Ejecución 2 | Ejecución 3 | Promedio |
|-------------------|-------------|-------------|-------------|----------|
| 1  | 55 ms | 53 ms | 53 ms | 54 ms |
| 2  | 30 ms | 29 ms | 28 ms | 29 ms |
| 4  | 19 ms | 19 ms | 18 ms | 19 ms |
| 8  | 20 ms | 17 ms | 18 ms | 18 ms |
| 16 | 19 ms | 18 ms | 18 ms | 18 ms |

**Conclusión:** el tiempo baja casi a la mitad al pasar de 1 a 2 hilos y vuelve a bajar con 4 hilos (≈2,8 veces más rápido que con 1). A partir de 4 hilos el tiempo prácticamente no cambia (18–19 ms): agregar más hilos ya no ayuda.

*Nota:* en otra corrida aparecieron tiempos irregulares (por ejemplo, 2 hilos tardó más que 1), probablemente por otros procesos activos en la computadora. Por eso se usó la corrida estable.

1. **¿Cuál cantidad de hilos produjo el mejor tiempo?**
   8 hilos (17 ms en la ejecución 2; 18 ms en promedio, empatado con 16). La diferencia con 4 hilos es de apenas 1 ms, así que la mejora real se alcanza con 4 hilos.

2. **¿El tiempo mejoró siempre al aumentar los hilos?**
   No. Mejoró mucho de 1 a 2 y de 2 a 4 hilos, pero con 8 y 16 hilos se mantuvo igual y en algunas ejecuciones incluso subió (por ejemplo, 8 hilos tardó 20 ms en la ejecución 1, más que con 4).

3. **¿Cuántos núcleos tiene la computadora donde se ejecutó el programa?**
   8 núcleos lógicos (`std::thread::hardware_concurrency()` = 8).

4. **¿Qué ocurre cuando se usan más hilos que núcleos disponibles?**
   Con 16 hilos y 8 núcleos, los hilos no pueden correr todos al mismo tiempo: el sistema operativo los turna en los núcleos disponibles. No hay más paralelismo real, solo más hilos esperando su turno, y por eso el tiempo no mejora.

5. **¿Qué relación tiene esto con el context switching?**
   Cada vez que el SO cambia de un hilo a otro debe guardar y restaurar su estado (registros, pila, caché). Con más hilos que núcleos hay más cambios de contexto, y ese tiempo no hace trabajo útil.

6. **¿Por qué la versión con 16 hilos podría no ser la mejor?**
   Porque supera la cantidad de núcleos: agrega costo de creación de hilos y de context switching sin dar más paralelismo. Además, esta suma está limitada por la velocidad de la memoria y no por el CPU, así que desde 4 hilos la memoria ya es el cuello de botella.

---

## Parte 6: ejemplo simple de deadlock

Compilación y ejecución:

```powershell
g++ -std=c++17 -Wall -Wextra -pthread src/parte6_deadlock.cpp -o parte6.exe
.\parte6.exe
```

Salida obtenida (el programa nunca terminó y hubo que detenerlo con `Ctrl + C`):

```
Hilo A intentando tomar recurso 1...
Hilo B intentando tomar recurso 2...
Hilo A intentando tomar recurso 2...
Hilo B intentando tomar recurso 1...
```

1. **¿El programa termina normalmente?**
   No. Después de imprimir las 4 líneas se queda bloqueado y hay que detenerlo con `Ctrl + C`. Nunca aparece "obtuvo ambos recursos".

2. **¿Qué recurso tomó primero el hilo A?**
   `recurso1`.

3. **¿Qué recurso tomó primero el hilo B?**
   `recurso2`.

4. **¿Por qué ninguno de los dos hilos puede continuar?**
   A tiene `recurso1` y espera `recurso2`; B tiene `recurso2` y espera `recurso1`. Ninguno libera lo que tiene hasta obtener el otro, así que esperan para siempre.

5. **¿Qué significa espera circular?**
   Que existe un ciclo de hilos donde cada uno espera un recurso que tiene el siguiente: A espera a B y B espera a A. Es una de las cuatro condiciones necesarias para un deadlock.

6. **¿Cómo se podría evitar este problema?**
   Tomando siempre los mutex en el mismo orden en todos los hilos, o tomando ambos a la vez con `std::scoped_lock` / `std::lock`.

### Versión corregida

```powershell
g++ -std=c++17 -Wall -Wextra -pthread src/parte6_deadlock_corregido.cpp -o parte6_corregido.exe
.\parte6_corregido.exe
```

Salida obtenida (termina normalmente):

```
Hilo A obtuvo ambos recursos de forma segura.
Hilo B obtuvo ambos recursos de forma segura.
```

**¿Por qué `std::scoped_lock` ayuda a evitar este deadlock?**
Porque bloquea varios mutex en una sola operación usando un algoritmo que evita deadlock: si no puede obtenerlos todos, libera los que tomó y vuelve a intentar. Así ningún hilo se queda con un recurso mientras espera el otro, y se rompe la espera circular. Además libera ambos automáticamente al salir del alcance.

---

## Parte 7: reflexión final

1. **¿Cuál fue la diferencia más clara que observó entre ejecución secuencial y ejecución con hilos?**
   Con hilos el orden de ejecución no es predecible (los mensajes de la parte 1 salieron en distinto orden y mezclados), pero el trabajo grande terminó mucho más rápido: la suma del vector pasó de ≈80 ms a ≈26 ms con 4 hilos (≈3 veces más rápido).

2. **¿Qué es una condición de carrera?**
   Un error que ocurre cuando dos o más hilos acceden a un recurso compartido al mismo tiempo, al menos uno lo modifica, y el resultado depende del orden en que se ejecuten.

3. **¿Por qué `contador++` puede fallar cuando muchos hilos lo ejecutan al mismo tiempo?**
   Porque no es atómico: es leer, sumar y escribir. Si dos hilos leen el mismo valor antes de escribir, uno de los incrementos se pierde.

4. **¿Qué problema resuelve un mutex?**
   Garantiza exclusión mutua en una sección crítica, evitando condiciones de carrera.

5. **¿Qué ventaja tiene `std::lock_guard` sobre llamar manualmente a `lock()` y `unlock()`?**
   Libera el mutex automáticamente al salir del alcance, aunque haya un `return` temprano o una excepción. Evita olvidar el `unlock()` y dejar el mutex bloqueado.

6. **¿Por qué más hilos no siempre significan mejor rendimiento?**
   Porque el paralelismo real está limitado por los núcleos. Los hilos extra agregan costo de creación, sincronización y context switching, y pueden competir por memoria y caché.

7. **¿Qué es un deadlock?**
   Una situación en la que dos o más hilos quedan bloqueados para siempre, cada uno esperando un recurso que tiene otro.

8. **¿Qué buenas prácticas aplicaría al programar con hilos?**
   - Minimizar los datos compartidos; preferir variables locales por hilo.
   - Usar RAII (`std::lock_guard`, `std::scoped_lock`) en vez de `lock()`/`unlock()` manual.
   - Mantener las secciones críticas cortas.
   - Tomar varios mutex siempre en el mismo orden o con `std::scoped_lock`.
   - Usar `std::atomic` para contadores simples.
   - Ajustar la cantidad de hilos a los núcleos (`std::thread::hardware_concurrency()`).
   - Siempre hacer `join()` (o usar `std::jthread` en C++20).

9. **¿En qué tipo de programas reales podría ser útil la programación concurrente?**
   Servidores web que atienden varios clientes, interfaces gráficas que no se congelan mientras hacen trabajo en segundo plano, descargas simultáneas, lectura de sensores mientras se procesa información en sistemas embebidos.

10.  **¿En qué tipo de programas reales podría ser útil la programación paralela?**
    Procesamiento de imágenes y video, simulaciones científicas (circuitos, elementos finitos), entrenamiento de redes neuronales, procesamiento de señales (FFT), compilación de proyectos grandes y análisis de grandes volúmenes de datos.
