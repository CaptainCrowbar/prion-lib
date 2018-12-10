#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"
#include "unicorn/path.hpp"
#include <cstdio>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace RS {

    // I/O abstract base class

    class IO {
    public:
        class line_iterator: public InputIterator<line_iterator, const std::string> {
        public:
            line_iterator() = default;
            explicit line_iterator(IO& io): iop(&io), line() { ++*this; }
            const std::string& operator*() const noexcept { return line; }
            line_iterator& operator++();
            bool operator==(const line_iterator& rhs) const noexcept { return iop == rhs.iop; }
        private:
            IO* iop = nullptr;
            std::string line;
        };
        enum class mode {
            read_only = 1,
            write_only,
            append,
            create_always,
            open_always,
            open_existing,
        };
        virtual ~IO() = default;
        virtual void close() noexcept = 0;
        virtual void flush() noexcept {}
        virtual int getc() noexcept;
        virtual Unicorn::Path get_path() const { return {}; }
        virtual bool is_open() const noexcept = 0;
        virtual bool is_terminal() const noexcept = 0;
        virtual void putc(char c) { write(&c, 1); }
        virtual size_t read(void* ptr, size_t maxlen) noexcept = 0;
        virtual std::string read_line();
        virtual void seek(ptrdiff_t offset, int which = SEEK_CUR) noexcept = 0;
        virtual ptrdiff_t tell() noexcept = 0;
        virtual size_t write(const void* ptr, size_t len) = 0;
        virtual void write_n(size_t n, char c);
        void check(Uview detail = "") const;
        void clear_error() noexcept { set_error(0); }
        std::error_code error() const noexcept { return status; }
        template <typename... Args> void format(Uview pattern, const Args&... args) { write_str(fmt(pattern, args...)); }
        Irange<line_iterator> lines() { return {line_iterator(*this), {}}; }
        bool ok() const noexcept { return ! status && is_open(); }
        template <typename... Args> void print(const Args&... args);
        std::string read_all();
        size_t read_n(std::string& s, size_t maxlen = 1024);
        std::string read_str(size_t maxlen);
        void write_line() { putc('\n'); }
        void write_line(std::string_view str);
        size_t write_str(std::string_view str);
    protected:
        static constexpr const char* null_device =
            #ifdef _XOPEN_SOURCE
                "/dev/null";
            #else
                "NUL:";
            #endif
        IO() = default;
        void set_error(int err, const std::error_category& cat = std::generic_category()) noexcept;
    private:
        std::error_code status;
    };

        template <typename... Args>
        void IO::print(const Args&... args) {
            std::vector<Ustring> vec{to_str(args)...};
            write_line(join(vec, " "));
        }

    // C standard I/O

    class Cstdio:
    public IO {
    public:
        RS_MOVE_ONLY(Cstdio);
        using handle_type = FILE*;
        Cstdio() = default;
        explicit Cstdio(FILE* f, bool owner = true) noexcept;
        explicit Cstdio(const Unicorn::Path& f, mode m = mode::read_only);
        Cstdio(const Unicorn::Path& f, const Ustring& iomode);
        virtual void close() noexcept override;
        virtual void flush() noexcept override;
        virtual int getc() noexcept override;
        virtual Unicorn::Path get_path() const;
        virtual bool is_open() const noexcept override { return bool(fp); }
        virtual bool is_terminal() const noexcept override;
        virtual void putc(char c) override;
        virtual size_t read(void* ptr, size_t maxlen) noexcept override;
        virtual std::string read_line() override;
        virtual void seek(ptrdiff_t offset, int which = SEEK_CUR) noexcept override;
        virtual ptrdiff_t tell() noexcept override;
        virtual size_t write(const void* ptr, size_t len) override;
        int fd() const noexcept;
        FILE* get() const noexcept { return fp.get(); }
        FILE* release() noexcept { return fp.release(); }
        void ungetc(char c);
        static Cstdio dev_null() noexcept;
        static Cstdio std_input() noexcept { return Cstdio(stdin, false); }
        static Cstdio std_output() noexcept { return Cstdio(stdout, false); }
        static Cstdio std_error() noexcept { return Cstdio(stderr, false); }
    private:
        Resource<FILE*> fp;
    };

    // File descriptor I/O

    class Fdio:
    public IO {
    public:
        RS_MOVE_ONLY(Fdio);
        using handle_type = int;
        Fdio() = default;
        explicit Fdio(int f, bool owner = true) noexcept;
        explicit Fdio(const Unicorn::Path& f, mode m = mode::read_only);
        Fdio(const Unicorn::Path& f, int iomode, int perm = 0666);
        virtual void close() noexcept override;
        virtual void flush() noexcept override;
        virtual Unicorn::Path get_path() const;
        virtual bool is_open() const noexcept override { return fd.get() != -1; }
        virtual bool is_terminal() const noexcept override;
        virtual size_t read(void* ptr, size_t maxlen) noexcept override;
        virtual void seek(ptrdiff_t offset, int which = SEEK_CUR) noexcept override;
        virtual ptrdiff_t tell() noexcept override;
        virtual size_t write(const void* ptr, size_t len) override;
        Fdio dup() noexcept;
        Fdio dup(int f) noexcept;
        int get() const noexcept { return fd.get(); }
        int release() noexcept { return fd.release(); }
        static Fdio dev_null() noexcept;
        static std::pair<Fdio, Fdio> pipe(size_t winmem = 1024);
        static Fdio std_input() noexcept { return Fdio(0, false); }
        static Fdio std_output() noexcept { return Fdio(1, false); }
        static Fdio std_error() noexcept { return Fdio(2, false); }
    private:
        Resource<int, -1> fd;
        #ifdef _MSC_VER
            using iosize = unsigned;
            using ofsize = long;
        #else
            using iosize = size_t;
            using ofsize = ptrdiff_t;
        #endif
    };

    #ifdef _WIN32

        // Windows file I/O

        class Winio:
        public IO {
        public:
            RS_MOVE_ONLY(Winio);
            using handle_type = void*;
            Winio() = default;
            explicit Winio(void* f, bool owner = true) noexcept;
            explicit Winio(const Unicorn::Path& f, mode m = mode::read_only);
            Winio(const Unicorn::Path& f, uint32_t desired_access, uint32_t share_mode, LPSECURITY_ATTRIBUTES security_attributes,
                uint32_t creation_disposition, uint32_t flags_and_attributes = 0, HANDLE template_file = nullptr);
            virtual void close() noexcept override;
            virtual void flush() noexcept override;
            virtual Unicorn::Path get_path() const;
            virtual bool is_open() const noexcept override { return bool(fh); }
            virtual bool is_terminal() const noexcept override;
            virtual size_t read(void* ptr, size_t maxlen) noexcept override;
            virtual void seek(ptrdiff_t offset, int which = SEEK_CUR) noexcept override;
            virtual ptrdiff_t tell() noexcept override;
            virtual size_t write(const void* ptr, size_t len) override;
            void* get() const noexcept { return fh.get(); }
            void* release() noexcept { return fh.release(); }
            static Winio dev_null() noexcept;
            static Winio std_input() noexcept { return Winio(GetStdHandle(STD_INPUT_HANDLE), false); }
            static Winio std_output() noexcept { return Winio(GetStdHandle(STD_OUTPUT_HANDLE), false); }
            static Winio std_error() noexcept { return Winio(GetStdHandle(STD_ERROR_HANDLE), false); }
        private:
            Resource<void*> fh;
        };

    #endif

    // String buffer

    class StringBuffer:
    public IO {
    public:
        StringBuffer() noexcept: sptr(&sbuf) {}
        explicit StringBuffer(std::string& s): sptr(&s), wpos(s.size()) {}
        StringBuffer(const StringBuffer&) = delete;
        StringBuffer(StringBuffer&& src) noexcept;
        StringBuffer& operator=(const StringBuffer&) = delete;
        StringBuffer& operator=(StringBuffer&& src) noexcept;
        virtual void close() noexcept override { clear(); }
        virtual void flush() noexcept override;
        virtual int getc() noexcept override;
        virtual bool is_open() const noexcept override { return true; }
        virtual bool is_terminal() const noexcept override { return false; }
        virtual void putc(char c) override;
        virtual size_t read(void* ptr, size_t maxlen) noexcept override;
        virtual std::string read_line() override;
        virtual void seek(ptrdiff_t offset, int which = SEEK_CUR) noexcept override;
        virtual ptrdiff_t tell() noexcept override { return 0; }
        virtual size_t write(const void* ptr, size_t len) override;
        virtual void write_n(size_t n, char c) override;
        char* begin() noexcept { return c_ptr() + rpos; }
        const char* begin() const noexcept { return sptr->data() + rpos; }
        char* end() noexcept { return c_ptr() + wpos; }
        const char* end() const noexcept { return sptr->data() + wpos; }
        void clear() noexcept;
        bool empty() const noexcept { return rpos == wpos; }
        size_t size() const noexcept { return wpos - rpos; }
        std::string str() const { return std::string(begin(), end()); }
    private:
        std::string sbuf;
        std::string* sptr;
        size_t rpos = 0;
        size_t wpos = 0;
        char* c_ptr() noexcept { return &(*sptr)[0]; }
        void trim_buffer() noexcept;
    };

}
