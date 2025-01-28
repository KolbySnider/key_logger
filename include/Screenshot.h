#include <Windows.h>

// Function to save the current screen bitmap to a file
BOOL WINAPI save_bitmap(WCHAR* wPath) {

    // Structures to hold bitmap file and info headers
    BITMAPFILEHEADER bfHeader;
    BITMAPINFOHEADER biHeader;
    BITMAPINFO bInfo;
    BITMAP bAllDesktops;

    // Handles and pointers for device contexts and bitmap
    HGDIOBJ hTempBitmap;
    HBITMAP hBitmap;
    HDC hDC, hMemDC;
    BYTE* bBits = NULL;
    HANDLE hHeap = GetProcessHeap();
    DWORD cbBits, dwWritten = 0;
    HANDLE hFile;

    // Get the virtual screen dimensions
    INT x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    INT y = GetSystemMetrics(SM_YVIRTUALSCREEN);

    // Initialize memory
    ZeroMemory(&bfHeader, sizeof(BITMAPFILEHEADER));
    ZeroMemory(&biHeader, sizeof(BITMAPINFOHEADER));
    ZeroMemory(&bInfo, sizeof(BITMAPINFO));
    ZeroMemory(&bAllDesktops, sizeof(BITMAP));

    // Get the device context of the entire screen
    hDC = GetDC(NULL);

    // Retrieve the current bitmap object
    hTempBitmap = GetCurrentObject(hDC, OBJ_BITMAP);
    GetObjectW(hTempBitmap, sizeof(BITMAP), &bAllDesktops);

    // Get width and height of the desktop bitmap
    LONG lWidth = bAllDesktops.bmWidth;
    LONG lHeight = bAllDesktops.bmHeight;

    // Clean up the temporary bitmap object
    DeleteObject(hTempBitmap);

    // Set up the bitmap file header
    bfHeader.bfType = (WORD)('B' | ('M' << 8)); // 'BM' in little-endian format
    bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // Set up the bitmap info header
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biBitCount = 24; // 24-bit bitmap (RGB)
    biHeader.biCompression = BI_RGB; // No compression
    biHeader.biPlanes = 1; // Number of color planes
    biHeader.biWidth = lWidth;
    biHeader.biHeight = lHeight; // Positive value means the image is top-down

    bInfo.bmiHeader = biHeader;

    // Calculate the size of the bitmap data
    cbBits = (((24 * lWidth + 31) & ~31) / 8) * lHeight;

    // Create a memory device context and compatible bitmap
    hMemDC = CreateCompatibleDC(hDC);
    hBitmap = CreateDIBSection(hDC, &bInfo, DIB_RGB_COLORS, (VOID**)&bBits, NULL, 0);
    SelectObject(hMemDC, hBitmap);

    // Copy the screen content to the memory DC
    BitBlt(hMemDC, 0, 0, lWidth, lHeight, hDC, x, y, SRCCOPY);

    // Create or open the file where the bitmap will be saved
    hFile = CreateFileW(wPath, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        // Clean up resources if file creation failed
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hDC);
        DeleteObject(hBitmap);

        return FALSE; // Indicate failure
    }

    // Write the headers and bitmap data to the file
    WriteFile(hFile, &bfHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
    WriteFile(hFile, &biHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
    WriteFile(hFile, bBits, cbBits, &dwWritten, NULL);
    FlushFileBuffers(hFile); // Ensure all data is written to the file
    CloseHandle(hFile); // Close the file handle

    // Clean up resources
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hDC);
    DeleteObject(hBitmap);

    return TRUE; // Indicate success
}
