#ifndef _FASTCGI_REQUEST_CACHE_MMAP_FILE_H_
#define _FASTCGI_REQUEST_CACHE_MMAP_FILE_H_

#include <unistd.h>
#include <memory>

namespace fastcgi
{
    class FileDescriptor {
    public:
        FileDescriptor(int fdes) : fdes_(fdes) {}
        ~FileDescriptor() {
            if (-1 != fdes_) {
                close(fdes_);
            }
        }

        int value() const {
            return fdes_;
        }
    private:
        int fdes_;
    };

    class MMapFile {
    public:
        MMapFile(const char *name, uint64_t window, bool is_read_only = false);
        virtual ~MMapFile();

        uint64_t size() const;
        bool empty() const;
        void resize(uint64_t newsize);

        char at(uint64_t index);
        char* atRange(uint64_t index, uint64_t length);
        std::pair<char*, uint64_t> atSegment(uint64_t index);

        uint64_t window() const;

        MMapFile* clone() const;

    private:
        MMapFile();
        void checkWindow();
        void unmap();
        char* map_segment(uint64_t segment);
        char* map_range(uint64_t begin, uint64_t end);
        void check_index(uint64_t index) const;
        bool mapped(uint64_t index) const;
        bool mapped(uint64_t begin, uint64_t end) const;
        std::string error(int error);

    private:
        void *pointer_;
        uint64_t size_;
        std::shared_ptr<FileDescriptor> fdes_;
        bool is_read_only_;
        uint64_t window_;
        uint64_t segment_start_, segment_len_;
        int page_size_;
    };

} // namespace fastcgi

#endif // _FASTCGI_REQUEST_CACHE_MMAP_FILE_H_
