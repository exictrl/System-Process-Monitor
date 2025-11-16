#include "SecurityUtils.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <wincrypt.h>
#include <vector>

bool SecurityUtils::isRunningAsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;

    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    //SID для группвы администраторов
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, 
                                DOMAIN_ALIAS_RID_ADMINS,
                                 0,0,0,0,0,0, &adminGroup)) {
        if(!CheckTokenMembership(NULL, adminGroup, &isAdmin)) {
            isAdmin = FALSE;
        }
        FreeSid(adminGroup);
    }
    return isAdmin == TRUE;
}

std::string SecurityUtils::calculateFileHash(const std::string& filePath) {
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    std::string hashStr;

    HANDLE hFile = CreateFileA(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ,
                              NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Cannot open file: " << filePath << std::endl;
        return "";
    }
    if (!CryptAcquireContext (&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)){
        CloseHandle(hFile);
        std::cerr << "CryptAcquireContext failed" << std::endl;
        return "";
    }
    if (!CryptCreateHash (hProv, CALG_SHA_256, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        CloseHandle(hFile);
        std::cerr << "CryptCreateHash failed" << std::endl;
        return "";
    }

    const DWORD BUFFER_SIZE = 8192;
    BYTE buffer[BUFFER_SIZE];
    DWORD bytesRead = 0;
    BOOL success = TRUE;

    while (success = ReadFile (hFile, buffer, BUFFER_SIZE, &bytesRead, NULL)) {
        if (bytesRead == 0) {
            break;
        }
        if (!CryptHashData (hHash, buffer, bytesRead, 0)) {
            std::cerr << "CryptHash faild" << std::endl;
            break;
        }
    }
    if (!success) {
        std::cerr << "ReadFile failed" << std::endl;
    }
    else {
        DWORD hashSize = 0;
        DWORD hashSizeSize = sizeof(hashSize);
        if (CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&hashSize, &hashSizeSize,0)) {
                        std::vector<BYTE> hash(hashSize);
            if (CryptGetHashParam(hHash, HP_HASHVAL, hash.data(), &hashSize, 0)) {
                // Конвертируем в hex-строку
                std::stringstream ss;
                for (DWORD i = 0; i < hashSize; i++) {
                    ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
                }
                hashStr = ss.str();
            }
        }
    }

    // Очищаем ресурсы
    if (hHash) CryptDestroyHash(hHash);
    if (hProv) CryptReleaseContext(hProv, 0);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

    return hashStr;
}
