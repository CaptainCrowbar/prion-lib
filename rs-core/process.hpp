#pragma once

#include "rs-core/channel.hpp"
#include "unicorn/utf.hpp"
#include <atomic>
#include <string>

namespace RS {

    // Spawned process channels

    class StreamProcess:
    public StreamChannel {
    public:
        RS_NO_COPY_MOVE(StreamProcess);
        explicit StreamProcess(const Ustring& cmd);
        virtual ~StreamProcess() noexcept { do_close(); }
        virtual void close() noexcept { do_close(); }
        virtual bool is_closed() const noexcept { return ! fp; }
        virtual size_t read(void* dst, size_t maxlen);
        int status() const noexcept { return st; }
    protected:
        virtual bool do_wait_for(duration t);
    private:
        std::atomic<FILE*> fp;
        int st = -1;
        void do_close() noexcept;
    };

    class TextProcess:
    public MessageChannel<Ustring> {
    public:
        RS_NO_COPY_MOVE(TextProcess);
        explicit TextProcess(const Ustring& cmd): sp(cmd), buf() {}
        virtual void close() noexcept;
        virtual bool is_closed() const noexcept { return sp.is_closed() && buf.empty(); }
        virtual bool read(Ustring& t);
        std::string read_all() { return buf + sp.read_all(); }
        int status() const noexcept { return sp.status(); }
    protected:
        virtual bool do_wait_for(duration t);
    private:
        StreamProcess sp;
        Ustring buf;
    };

    // Shell commands

    std::string shell(const Ustring& cmd);

}
