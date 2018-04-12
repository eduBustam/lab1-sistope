#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#ifndef _BMP_H_
#define _BMP_H_
#include <stdint.h>

#define TRUE 1
#define FALSE 0

/*
 * BMP files siguen algo parecido a este formato:
 *   --------------------------
 *   |          Header        |   54 bytes
 *   |-------------------------
 *   |    Palette (optional)  |   0 bytes (para 24-bit RGB images)
 *   |-------------------------
 *   |       Image Data       |   file size - 54 (for 24-bit images)
 *   --------------------------
 */

/**
 * BMP header .
 */

typedef struct _BMP_Header {
    uint16_t type;			// Tipo de BMP
    uint32_t size;			// Total de bits del archivo
    uint16_t reserved1;			// Not used
    uint16_t reserved2;			// Not used
    uint32_t offset;			// Bites desde el inicio hasta que empieza la imagen (54 bytes generalmente)
    uint32_t DIB_header_size;		// bits del DIB Header (40 bytes)
    int32_t  width;			// Width (largo) de la imagen
    int32_t  height;			// Height (alto) de la imagen
    uint16_t planes;			// Numeros de planos de color
    uint16_t bits;			// Bits por pixel
    uint32_t compression;		// Tipo de compresion
    uint32_t imagesize;			// Bits totales de la imagen
    int32_t  xresolution;		// Pixels per meter
    int32_t  yresolution;		// Pixels per meter
    uint32_t ncolours;			// Numero de colores
    uint32_t importantcolours;		// Colores importantes
} BMP_Header;

/**
 * BMP image .
 */

typedef struct _BMP_Image {
    BMP_Header header;
    unsigned char *data;
} BMP_Image;


// Lee una BMP_Image del archivo dado
//
BMP_Image *Read_BMP_Image(FILE *fptr);

// Comprueba la valides del archivo con el header del mismo
//
int Is_BMP_Header_Valid(BMP_Header *bmp_hdr, FILE *fptr);

// Escribe un BMP_Image en un archivo dado
//
int Write_BMP_Image(FILE *fptr, BMP_Image *image);

// Libera memoria de la imagen dada
//
void Free_BMP_Image(BMP_Image *image);

#endif /*  ESTO VA EN EL H bmp.h */

/* Verifica que el header del archivo sea valido
 * asuma que el header a sido leido a traves de fptr
 */
int Is_BMP_Header_Valid(BMP_Header* header, FILE *fptr) {
  // Asegura que sea un archivo BMP
  if (header->type != 0x4d42)
     return FALSE;

    printf("%d ",header->offset);
  // Asegura que sea un solo plano
  if (header->planes != 1)
    return FALSE;
  //Numero de colores correctos
  if (header->ncolours != 0)
    return FALSE;
  //Colores importantes correctos
  if (header->importantcolours != 0)
    return FALSE;
  //Calculo del padding
  int padding = (header->width * (header->bits/8));
  //Calculo del tamaño total de la imagen
  int size = padding * header->height;
    printf("H=%d,S=%d",header->size,size);
  //Que el tamaño calculado sea el tamaño que se encuentra detallado en el header
  if ((header->imagesize != size) || (header->size != size + header->offset))
     return FALSE;

  return TRUE;
}

/*En la siguiente funciuon el atributo de entrada corresponde al archivo de entrada.
 * Esta funcion retorna un puntero en la memoria en donde se encuentra la BMP_Image
 * si es que el archivo * contiene una imagen valida.
 * De locontrario, retorna NULL
 * Si la función no puede obtener la suficiente memoria para guardar la imagen, entonces, tambien
 * retorna NULL
 */
BMP_Image *Read_BMP_Image(FILE* fptr) {
   //Devuelve el puntero del archivo
   rewind(fptr);
   //Asigna memoria para BMP_Image*;
   BMP_Image *bmp_image = (BMP_Image *)calloc(sizeof(BMP_Image),1);
   //Reviza que el fread funcione correctamemte
   if (bmp_image == NULL)
      return NULL;
   //Lee los primeros 54 bytes de la fuente colocandolo en el header
   fread(&(bmp_image->header), 54, 1, fptr);
   //Reviza nuevamente que el fread trabaje correctamente
   if (&(bmp_image->header) == NULL)
      return NULL;
   // si selee correctamente, comprueba que sea un header valido
   if (!Is_BMP_Header_Valid(&(bmp_image->header), fptr)){
      Free_BMP_Image(bmp_image);
      return NULL;
   }
   // Asigna memoria para la imagen data
   BMP_Header *temp = &(bmp_image->header);
   int imageSize = temp->imagesize;
   //tamaño de la imagen
   bmp_image->data = (unsigned char *)malloc(imageSize);

   // Lee la data de la imagen
   fread(bmp_image->data, imageSize, 1, fptr);
   printf("\nSELEE CORRECTAMENTE\n");
   return bmp_image;
}

/* Los argumentos de entrada de esta funcion es un puntero al archivo de salida, una BMP_Image *image.
 * La función escribe el header y la data de la imagen en el archivo de destino,
 * retornando TRUE si la escritura es correcta
 * FALSE en caso contrario
 */
int Write_BMP_Image(FILE* fptr, BMP_Image* image)
{
   rewind(fptr);
   // escribe el header
   fwrite(&(image->header), 54, 1, fptr);
   //Se verifica que algo se haya encrito en el archivo de salida
   if(fptr == NULL)
      return FALSE;

   // se escribe la data de la imagen
   int fileSize = (image->header).imagesize;

   fwrite(image->data, fileSize, 1, fptr);

   if(fptr == NULL)
      return FALSE;
   return TRUE;
}

/* El argumento de entrada es el puntero a la BMP_Image. La funcion libera la memoria
 * usada por BMP_Image.
 */
void Free_BMP_Image(BMP_Image* image) {
   free(image->data);
   free(image);
}

int main(){
    int i=0;
    if (i==0){
        //Abrir archivo de entrada
        FILE* input = fopen("img_2.bmp","rb");
        if (input == NULL) {
            return EXIT_FAILURE;
        }

        //Leer archivo
        BMP_Image *image = Read_BMP_Image(input);
        if (image == NULL) {
            fclose(input);
            return EXIT_FAILURE;
        }

        //Escribir archivo de salida

        FILE* output = fopen("img_x.bmp","wb");
        if (output == NULL) {
            fclose(input);
            Free_BMP_Image(image);
            return EXIT_FAILURE;
        }
        Write_BMP_Image(output, image);

        //Cerrar archivos
        fclose(input);
        fclose(output);

        //Liberar memoria
        Free_BMP_Image(image);

    }
    else {
        return(EXIT_FAILURE);
    }
    printf("DONE\n");
    return EXIT_SUCCESS;
}
