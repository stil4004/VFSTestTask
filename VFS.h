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
        File* Open(const char* name) override {
            mtx.lock();
            if (openFiles.find(name) != openFiles.end() && openFiles[name]->writeMode) {
                mtx.unlock();
                return nullptr;
            }

            File* file = new File();
            file->stream.open(name, std::ios::in | std::ios::binary);
            if (!file->stream.is_open()) {
                delete file;
                mtx.unlock();
                return nullptr;
            }

            file->writeMode = false;
            file->path = name;
            openFiles[name] = file;

            mtx.unlock();
            return file;
        }

        File* Create(const char* name) override {
            mtx.lock();
            if (openFiles.find(name) != openFiles.end() && !openFiles[name]->writeMode) {
                mtx.unlock();
                return nullptr;
            }

            File* file = new File();
            file->stream.open(name, std::ios::out | std::ios::binary);
            if (!file->stream.is_open()) {
                delete file;
                mtx.unlock();
                return nullptr;
            }

            file->writeMode = true;
            file->path = name;
            openFiles[name] = file;

            mtx.unlock();
            return file;
        }

        size_t Read(File* f, char* buff, size_t len) override {
            if (!f || !f->stream.is_open() || f->writeMode) return 0;

            f->stream.read(buff, len);
            return f->stream.gcount();
        }

        size_t Write(File* f, char* buff, size_t len) override {
            if (!f || !f->stream.is_open() || !f->writeMode) return 0;

            f->stream.write(buff, len);
            if (f->stream.bad()) return 0;

            return len;
        }

        void Close(File* f) override {
            if (!f || !f->stream.is_open()) return;

            mtx.lock();
            f->stream.close();
            openFiles.erase(f->path);
            delete f;
            mtx.unlock();
        }
    };

}  // namespace TestTask