#include "VFS.h"

TestTask::File* TestTask::VFS::Open(const char* name)
{
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

TestTask::File* TestTask::VFS::Create(const char* name)
{
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

size_t TestTask::VFS::Read(File* f, char* buff, size_t len)
{
    if (!f || !f->stream.is_open() || f->writeMode) return 0;

    f->stream.read(buff, len);
    return f->stream.gcount();
}

size_t TestTask::VFS::Write(File* f, char* buff, size_t len)
{
    if (!f || !f->stream.is_open() || !f->writeMode) return 0;

    f->stream.write(buff, len);
    if (f->stream.bad()) return 0;

    return len;
}

void TestTask::VFS::Close(File* f)
{
    if (!f || !f->stream.is_open()) return;

    mtx.lock();
    f->stream.close();
    openFiles.erase(f->path);
    delete f;
    mtx.unlock();
}
