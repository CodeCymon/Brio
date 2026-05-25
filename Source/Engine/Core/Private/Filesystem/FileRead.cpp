#include "Filesystem/FileRead.h"

#include <fstream>

DEFINE_LOG_CATEGORY_STATIC(LogFilesystem)

TArray<u8> ReadBinaryFile(const char* path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        LOG_ERROR(LogFilesystem, "Could not open file '{}'", path);
        return TArray<u8>();
    }

    usize size = file.tellg();

    TArray<u8> buffer(size);

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(buffer.Data()), size);
    file.close();

    return buffer;
}
