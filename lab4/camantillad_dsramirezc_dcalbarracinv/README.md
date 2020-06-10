# Blurring Paralelo (Practica 1)

El siguiente codigo hace uso del algoritmo boxBlur para aproximar el blurring de Gauss, dicho algoritmo se obtuvo del siguiente articulo http://blog.ivank.net/about. Para compilar el codigo es necesario utilizar el siguiente comando

> g++ parallel_filter.cpp -Wall -pedantic -lpthread -w

Luego de compilado simplemente se debe pasar como argumentos el nombre de la imagen fuente, el nombre de la imagen de salida, el tamaño del kernel y el numero de threads, se muestra acontinuacion un ejemplo

> ./a.out 720.jpg 720blur.jpg 15 4

**Nota:** Para correr el script por favor tenga en consideracion el nombre de la imagen, si corresponde a una de 720p nombrela 720.jpg, si es de 1080p nombrela 1080.jpg y asi sucesivamente.
