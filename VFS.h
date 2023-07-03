#include <string>
#include <fstream>
#include <map>
#include <mutex>

namespace TestTask {

    struct File {
        std::fstream stream;
        std::string path;
        bool writeMode;
    };


    class IVFS {
        virtual File* Open(const char* name) = 0;
        virtual File* Create(const char* name) = 0;
        virtual size_t Read(File* file, char* buff, size_t len) = 0;
        virtual size_t Write(File* file, char* buff, size_t len) = 0;
        virtual void Close(File* file) = 0;
    };


    class VFS : public IVFS {
    private:
        std::map<std::string, File*> openFiles;
        std::mutex mtx;

    public:
        File* Open(const char* name) override;

        File* Create(const char* name) override;

        size_t Read(File* f, char* buff, size_t len) override;

        size_t Write(File* f, char* buff, size_t len) override;

        void Close(File* f) override;
    };

} 