#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sys/resource.h>

// Select implementation via compiler macro.
#if defined(IMPL_C_STYLE)
#   include "../attempt0-c-style/lib.h"
    struct ListWrapper { Linked_List impl; void add(int v){impl.add(v);} bool remove(int v){return impl.remove(v);} };
    constexpr const char* kImplName = "c-style";
    constexpr bool kThreadSafe = false;
#elif defined(IMPL_CPP)
#   include "../attempt1-c++/lib.h"
    struct ListWrapper { LinkedList<int> impl; void add(int v){impl.add(v);} bool remove(int v){return impl.remove(v);} };
    constexpr const char* kImplName = "c++";
    constexpr bool kThreadSafe = false;
#elif defined(IMPL_MUTEX)
#   include "../attempt2-mutex/lib.h"
    struct ListWrapper { LinkedList<int> impl; void add(int v){impl.add(v);} bool remove(int v){return impl.remove(v);} };
    constexpr const char* kImplName = "mutex";
    constexpr bool kThreadSafe = true;
#elif defined(IMPL_SHARE_MUTEX)
#   include "../attempt3-share-mutex/lib.h"
    struct ListWrapper { LinkedList<int> impl; void add(int v){impl.add(v);} bool remove(int v){return impl.remove(v);} };
    constexpr const char* kImplName = "shared_mutex";
    constexpr bool kThreadSafe = true;
#elif defined(IMPL_ATOMIC)
#   include "../attempt4-atomic/lib.h"
    struct ListWrapper { LockFreeLinkedList<int> impl; void add(int v){impl.add(v);} bool remove(int v){return impl.remove(v);} };
    constexpr const char* kImplName = "atomic";
    constexpr bool kThreadSafe = true;
#else
#   error "Pick one: -DIMPL_C_STYLE | -DIMPL_CPP | -DIMPL_MUTEX | -DIMPL_SHARE_MUTEX | -DIMPL_ATOMIC"
#endif

struct Metrics { double seconds{}; std::size_t attempts{}; long max_rss_kb{}; };

long rss_kb() { rusage ru{}; getrusage(RUSAGE_SELF, &ru); return ru.ru_maxrss; }

Metrics run_single(std::size_t ops) {
    ListWrapper list;
    auto start = std::chrono::steady_clock::now();
    for (std::size_t i=0;i<ops;++i) list.add(static_cast<int>(i));
    for (std::size_t i=0;i<ops;i+=2) list.remove(static_cast<int>(i));
    auto end = std::chrono::steady_clock::now();
    Metrics m; m.seconds=std::chrono::duration<double>(end-start).count(); m.attempts=ops+ops/2; m.max_rss_kb=rss_kb(); return m;
}

Metrics run_multi(std::size_t ops_per_thread,int threads) {
    ListWrapper list;
    auto start = std::chrono::steady_clock::now();
    std::vector<std::thread> ts;
    ts.reserve(threads);
    for(int t=0;t<threads;++t){
        ts.emplace_back([&,t]{ std::size_t base=static_cast<std::size_t>(t)*ops_per_thread*2;
            for(std::size_t i=0;i<ops_per_thread;++i) list.add(static_cast<int>(base+i));
            for(std::size_t i=0;i<ops_per_thread;i+=2) list.remove(static_cast<int>(base+i));
        });
    }
    for(auto& th:ts) th.join();
    auto end = std::chrono::steady_clock::now();
    Metrics m; m.seconds=std::chrono::duration<double>(end-start).count(); m.attempts=static_cast<std::size_t>(threads)*(ops_per_thread+ops_per_thread/2); m.max_rss_kb=rss_kb(); return m;
}

void print_header(){ std::cout<<"name,mode,threads,ops_per_thread,attempts,seconds,ops_per_sec,max_rss_kb\n"; }

void print_row(const char* mode,int threads,std::size_t ops,const Metrics& m){
    double throughput = m.attempts / m.seconds;
    std::cout<<kImplName<<','<<mode<<','<<threads<<','<<ops<<','<<m.attempts<<','<<m.seconds<<','<<throughput<<','<<m.max_rss_kb<<"\n";
}

int main(int argc,char** argv){
    std::size_t ops=20000;
    int threads=std::max(2u,std::thread::hardware_concurrency());
    std::string mode="both";
    for(int i=1;i<argc;++i){
        std::string arg=argv[i];
        if(arg=="--ops" && i+1<argc){ ops=static_cast<std::size_t>(std::strtoull(argv[++i],nullptr,10)); }
        else if(arg=="--threads" && i+1<argc){ threads=std::atoi(argv[++i]); }
        else if(arg=="--mode" && i+1<argc){ mode=argv[++i]; }
        else if(arg=="--help"){ std::cout<<"Usage: ./bench_run [--ops N] [--threads T] [--mode single|multi|both]\n"; return 0; }
    }

    print_header();
    if(mode=="single" || mode=="both"){ auto m=run_single(ops); print_row("single",1,ops,m); }
    if((mode=="multi" || mode=="both") && kThreadSafe){ auto m=run_multi(ops,threads); print_row("multi",threads,ops,m); }
    else if((mode=="multi" || mode=="both") && !kThreadSafe){ std::cerr<<"# skipped multi for "<<kImplName<<" (not thread safe)\n"; }
    return 0;
}
