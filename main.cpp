#include <iostream>
#include "VFS.h" // Replace with your actual file name

using namespace TestTask;

bool TestOpenNonExistingFile() {
    VFS vfs;
    File* file = vfs.Open("nonExistingFile.txt");
    if (file != nullptr) {
        std::cerr << "TestOpenNonExistingFile failed!" << std::endl;
        return false;
    }
    return true;
}

bool TestCreateAndOpenFile() {
    VFS vfs;
    File* file = vfs.Create("testFile.txt");
    if (file == nullptr) {
        std::cerr << "TestCreateAndOpenFile failed: unable to create file!" << std::endl;
        return false;
    }

    char data[] = "Hello, World!";
    size_t bytesWritten = vfs.Write(file, data, sizeof(data));
    if (bytesWritten != sizeof(data)) {
        std::cerr << "TestCreateAndOpenFile failed: unable to write data!" << std::endl;
        vfs.Close(file);
        return false;
    }

    vfs.Close(file);

    file = vfs.Open("testFile.txt");
    if (file == nullptr) {
        std::cerr << "TestCreateAndOpenFile failed: unable to open file!" << std::endl;
        return false;
    }

    char readBuffer[128];
    size_t bytesRead = vfs.Read(file, readBuffer, sizeof(readBuffer));
    if (bytesRead != sizeof(data)) {
        std::cerr << "TestCreateAndOpenFile failed: incorrect amount of data read!" << std::endl;
        vfs.Close(file);
        return false;
    }
    if (std::string(readBuffer, bytesRead) != std::string(data)) {
        std::cerr << "TestCreateAndOpenFile failed: data mismatch!" << std::endl;
        vfs.Close(file);
        return false;
    }

    vfs.Close(file);
    return true;
}

bool TestCreateExistingFile() {
    VFS vfs;
    File* file = vfs.Create("testFile.txt");
    if (file == nullptr) {
        std::cerr << "TestCreateExistingFile failed: unable to create file!" << std::endl;
        return false;
    }

    vfs.Close(file);

    file = vfs.Create("testFile.txt");
    if (file != nullptr) {
        std::cerr << "TestCreateExistingFile failed: created an already existing file!" << std::endl;
        return false;
    }
    return true;
}

bool TestOpenWriteOnlyFile() {
    VFS vfs;
    File* file = vfs.Create("testFile.txt");
    if (file == nullptr) {
        std::cerr << "TestOpenWriteOnlyFile failed: unable to create file!" << std::endl;
        return false;
    }

    File* file2 = vfs.Open("testFile.txt");
    if (file2 != nullptr) {
        std::cerr << "TestOpenWriteOnlyFile failed: opened a write-only file!" << std::endl;
        return false;
    }

    vfs.Close(file);
    return true;
}

int main() {
    if (TestOpenNonExistingFile() && TestCreateAndOpenFile() && TestCreateExistingFile() && TestOpenWriteOnlyFile()) {
        std::cout << "All tests passed!" << std::endl;
    }
    else {
        std::cerr << "Some tests failed!" << std::endl;
    }

    getchar();

    return 0;
}