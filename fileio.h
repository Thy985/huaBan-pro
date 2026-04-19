#pragma once
#include "huaBan.h"
#include "canvas.h"

class FileIO {
public:
    static void SaveImage(const Canvas& canvas);
    static void LoadImage(Canvas& canvas);
    static void ExportArtwork(const Canvas& canvas);
};
