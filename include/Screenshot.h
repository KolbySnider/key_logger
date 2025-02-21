#include <vector>
#include <Windows.h>

std::vector<char> capture_screenshot() {
    std::vector<char> bmpData;

    // Get virtual screen dimensions
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    // Get device contexts
    HDC hScreenDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    if (!hMemDC) {
        ReleaseDC(NULL, hScreenDC);
        return bmpData;
    }

    // Set up bitmap info header for a 24-bit image
    BITMAPINFOHEADER bi;
    ZeroMemory(&bi, sizeof(BITMAPINFOHEADER));
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = height;  // positive for bottom-up DIB (you can make it negative for top-down)
    bi.biPlanes = 1;
    bi.biBitCount = 24;    // 24-bit
    bi.biCompression = BI_RGB;

    // Prepare BITMAPINFO structure
    BITMAPINFO biInfo;
    ZeroMemory(&biInfo, sizeof(BITMAPINFO));
    biInfo.bmiHeader = bi;

    // Pointer to receive the pixel data
    void* lpPixels = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hScreenDC, &biInfo, DIB_RGB_COLORS, &lpPixels, NULL, 0);
    if (!hBitmap) {
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        return bmpData;
    }

    HGDIOBJ oldBitmap = SelectObject(hMemDC, hBitmap);

    // Copy the screen into the memory device context
    if (!BitBlt(hMemDC, 0, 0, width, height, hScreenDC, x, y, SRCCOPY)) {
        SelectObject(hMemDC, oldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        return bmpData;
    }

    // Calculate padded row size (each row is aligned to 4 bytes)
    int rowStride = ((width * 24 + 31) / 32) * 4;
    DWORD bmpSize = rowStride * height;

    // Create a BITMAPFILEHEADER
    BITMAPFILEHEADER bmfHeader;
    ZeroMemory(&bmfHeader, sizeof(BITMAPFILEHEADER));
    bmfHeader.bfType = 0x4D42; // 'BM'
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfSize = bmfHeader.bfOffBits + bmpSize;

    // Resize the vector to hold the entire BMP file data
    bmpData.resize(bmfHeader.bfSize);

    // Copy headers and pixel data into the vector
    memcpy(bmpData.data(), &bmfHeader, sizeof(BITMAPFILEHEADER));
    memcpy(bmpData.data() + sizeof(BITMAPFILEHEADER), &bi, sizeof(BITMAPINFOHEADER));
    memcpy(bmpData.data() + bmfHeader.bfOffBits, lpPixels, bmpSize);

    // Clean up
    SelectObject(hMemDC, oldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);

    return bmpData;
}
