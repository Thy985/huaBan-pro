#include "fileio.h"
#include <commdlg.h>

void FileIO::SaveImage(const Canvas& canvas) {
    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = TEXT("");
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetHWnd();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = TEXT("PNG Files (*.png)\0*.png\0BMP Files (*.bmp)\0*.bmp\0All Files (*.*)\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    
    if (GetSaveFileName(&ofn) == TRUE) {
        IMAGE img(GRID_NUM * CELL_SIZE, GRID_NUM * CELL_SIZE);
        SetWorkingImage(&img);
        
        setfillcolor(BLACK);
        solidrectangle(0, 0, GRID_NUM * CELL_SIZE, GRID_NUM * CELL_SIZE);
        
        for (int x = 0; x < GRID_NUM; x++) {
            for (int y = 0; y < GRID_NUM; y++) {
                COLORREF color = canvas.GetPixel(x, y);
                if (color != BLACK) {
                    setfillcolor(color);
                    solidrectangle(x * CELL_SIZE, y * CELL_SIZE, 
                                  (x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE);
                }
            }
        }
        
        SetWorkingImage();
        saveimage(szFile, &img);
    }
}

void FileIO::LoadImage(Canvas& canvas) {
    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = TEXT("");
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetHWnd();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = TEXT("Image Files (*.bmp;*.jpg;*.png)\0*.bmp;*.jpg;*.png\0All Files (*.*)\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileName(&ofn) == TRUE) {
        IMAGE img;
        if (loadimage(&img, szFile)) {
            int imgWidth = img.getwidth();
            int imgHeight = img.getheight();
            
            HDC hdcTemp = GetImageHDC(&img);
            
            for (int x = 0; x < GRID_NUM; x++) {
                for (int y = 0; y < GRID_NUM; y++) {
                    int imgX = (int)(x * (float)imgWidth / GRID_NUM);
                    int imgY = (int)(y * (float)imgHeight / GRID_NUM);
                    
                    if (imgX >= 0 && imgX < imgWidth && imgY >= 0 && imgY < imgHeight) {
                        COLORREF color = GetPixel(hdcTemp, imgX, imgY);
                        if (color != COLOR_TRANSPARENT) {
                            canvas.SetPixel(x, y, color);
                        }
                    }
                }
            }
            
            SetWorkingImage(NULL);
            DeleteDC(hdcTemp);
        }
    }
}

void FileIO::ExportArtwork(const Canvas& canvas) {
    SaveImage(canvas);
}
