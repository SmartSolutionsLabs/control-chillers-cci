#ifndef BITMAP_HPP
#define BITMAP_HPP

#include <stdint.h>

struct Bitmap {
    int width;
    int height;
    const uint8_t* data; // Apuntador a los datos de la imagen
};

#endif // BITMAP_HPP