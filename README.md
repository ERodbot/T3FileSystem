# UNIDAD DE INGENIERÍA EN COMPUTACIÓN  
## Sede Central Cartago  
### Principios de Sistemas Operativos  
**Profesor:** Kenneth Obando Rodríguez

# Trabajo Corto 3  
**Fecha de Entrega:** 30 de octubre, 2024

## Sistema de Archivos Simple

### Descripción de la tarea

En esta tarea, los estudiantes deberán implementar un sistema de archivos simple en C que emule las operaciones básicas de un sistema de archivos. Este sistema manejará operaciones como crear archivos, escribir en ellos, leer su contenido, eliminar archivos y listar el contenido del directorio. Los estudiantes deben simular la estructura de un sistema de archivos básico utilizando una tabla de archivos y bloques de datos.

### Especificaciones

#### Estructura del Sistema de Archivos

- El sistema simulado debe dividirse en bloques de un tamaño fijo (por ejemplo, 512 bytes por bloque).
- Los archivos deben estar representados por estructuras de control que almacenen información como el nombre del archivo, el tamaño, y los bloques ocupados.
- El sistema debe simular un directorio raíz donde se almacenarán los archivos.

#### Operaciones Soportadas

- **CREATE <nombre_archivo> <tamaño_bytes>:** Crea un archivo con el nombre dado y asigna espacio en los bloques simulados para almacenar el archivo.
- **WRITE <nombre_archivo> <offset> <data>:** Escribe datos en el archivo, comenzando en el offset especificado. Si el archivo no existe o no hay suficiente espacio, se debe retornar un error.
- **READ <nombre_archivo> <offset> <tamaño>:** Lee una cantidad específica de bytes desde el offset de un archivo dado.
- **DELETE <nombre_archivo>:** Elimina el archivo del sistema y libera los bloques que ocupaba.
- **LIST:** Lista todos los archivos almacenados en el sistema, mostrando su nombre y tamaño.

#### Simulación de los Bloques

- Se debe implementar un manejo de bloques de memoria simulados para la escritura y lectura de datos.
- Implementar una tabla de archivos que mantenga el seguimiento de qué bloques están ocupados y qué archivos los utilizan.

#### Manejo de Errores

El sistema debe manejar errores como:

- Falta de espacio en los bloques simulados.
- Intentar leer o escribir en un archivo que no existe.
- Sobrepasar los límites de un archivo.

#### Restricciones

- Limitar el número máximo de archivos y el tamaño máximo de cada archivo. (Por ejemplo, un total de 100 archivos y 1 MB de almacenamiento simulado).

### Ejemplo de ejecución


