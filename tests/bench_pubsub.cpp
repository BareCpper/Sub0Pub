#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"
#include "sub0pub/sub0pub.hpp"

#include <cstdio>
#include <iostream>
#include <string>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <fstream>
#include <unistd.h>
#elif defined(__APPLE__)
#include <sys/sysctl.h>
#include <sys/types.h>
#endif

namespace {

void printSystemInfo()
{
    std::cout << "== System Info ==" << std::endl;

    // OS
#ifdef _WIN32
    std::cout << "OS: Windows";
    OSVERSIONINFOEXW osvi{};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    std::cout << std::endl;
#elif defined(__APPLE__)
    std::cout << "OS: macOS" << std::endl;
#elif defined(__linux__)
    std::cout << "OS: Linux" << std::endl;
#endif

    // Compiler
#if defined(_MSC_VER)
    std::cout << "Compiler: MSVC " << _MSC_VER << std::endl;
#elif defined(__clang__)
    std::cout << "Compiler: Clang " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__ << std::endl;
#elif defined(__GNUC__)
    std::cout << "Compiler: GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << std::endl;
#endif

    // CPU
#ifdef _WIN32
    {
        SYSTEM_INFO si{};
        GetSystemInfo(&si);
        // Processor name from registry
        char cpuName[256] = "Unknown";
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            0, KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            DWORD size = sizeof(cpuName);
            RegQueryValueExA(hKey, "ProcessorNameString", nullptr, nullptr,
                reinterpret_cast<LPBYTE>(cpuName), &size);
            RegCloseKey(hKey);
        }
        std::cout << "CPU: " << cpuName << std::endl;
    }
#elif defined(__linux__)
    {
        std::ifstream cpuinfo("/proc/cpuinfo");
        std::string line;
        while (std::getline(cpuinfo, line)) {
            if (line.find("model name") != std::string::npos) {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                    std::cout << "CPU:" << line.substr(pos + 1) << std::endl;
                break;
            }
        }
    }
#elif defined(__APPLE__)
    {
        char buf[256];
        size_t len = sizeof(buf);
        if (sysctlbyname("machdep.cpu.brand_string", buf, &len, nullptr, 0) == 0)
            std::cout << "CPU: " << buf << std::endl;
    }
#endif

    // Logical cores
    std::cout << "Threads: " << std::thread::hardware_concurrency() << std::endl;

    // RAM
#ifdef _WIN32
    {
        MEMORYSTATUSEX mem{};
        mem.dwLength = sizeof(mem);
        if (GlobalMemoryStatusEx(&mem))
            std::cout << "RAM: " << (mem.ullTotalPhys / (1024 * 1024 * 1024)) << " GB" << std::endl;
    }
#elif defined(__linux__)
    {
        long pages = sysconf(_SC_PHYS_PAGES);
        long pageSize = sysconf(_SC_PAGE_SIZE);
        if (pages > 0 && pageSize > 0)
            std::cout << "RAM: " << ((long long)pages * pageSize / (1024 * 1024 * 1024)) << " GB" << std::endl;
    }
#elif defined(__APPLE__)
    {
        int64_t mem = 0;
        size_t len = sizeof(mem);
        if (sysctlbyname("hw.memsize", &mem, &len, nullptr, 0) == 0)
            std::cout << "RAM: " << (mem / (1024 * 1024 * 1024)) << " GB" << std::endl;
    }
#endif

    // Build config
#ifdef NDEBUG
    std::cout << "Build: Release" << std::endl;
#else
    std::cout << "Build: Debug" << std::endl;
#endif

    std::cout << "==" << std::endl << std::endl;
}



struct NoOpSubscriber : sub0::Subscribe<int> {
    void receive(const int&) noexcept override {}
};

struct IntPublisher : sub0::Publish<int> {
    void send(int v) { sub0::publish(this, v); }
};

struct FloatPublisher : sub0::Publish<float> {
    void send(float v) { sub0::publish(this, v); }
};

struct NoOpFloatSub : sub0::Subscribe<float> {
    void receive(const float&) noexcept override {}
};

struct FilteredSubscriber : sub0::Subscribe<int> {
    void receive(const int&) noexcept override {}
    bool filter(const int& v) noexcept override { return (v & 1) == 0; }
};

} // namespace

int main()
{
    printSystemInfo();

    ankerl::nanobench::Bench bench;
    bench.warmup(1000).minEpochIterations(1000000);

    // --- Publish throughput ---

    bench.title("Publish throughput");

    {
        IntPublisher pub;
        NoOpSubscriber sub;
        bench.run("1 subscriber", [&] {
            pub.send(42);
        });
    }

    {
        IntPublisher pub;
        NoOpSubscriber subs[4];
        (void)subs;
        bench.run("4 subscribers", [&] {
            pub.send(42);
        });
    }

    {
        IntPublisher pub;
        NoOpSubscriber subs[8];
        (void)subs;
        bench.run("8 subscribers (max default)", [&] {
            pub.send(42);
        });
    }

    // --- Publish with filter ---

    bench.title("Publish with filter");

    {
        IntPublisher pub;
        FilteredSubscriber sub;
        bench.run("1 filtered subscriber (pass)", [&] {
            pub.send(42); // even, passes filter
        });
    }

    {
        IntPublisher pub;
        FilteredSubscriber sub;
        bench.run("1 filtered subscriber (reject)", [&] {
            pub.send(43); // odd, rejected by filter
        });
    }

    // --- Multi-type dispatch ---

    bench.title("Multi-type dispatch");

    {
        struct MultiPub : sub0::Publish<int>, sub0::Publish<float> {
            void sendInt(int v) { sub0::publish(this, v); }
            void sendFloat(float v) { sub0::publish(this, v); }
        };

        MultiPub pub;
        NoOpSubscriber intSub;
        NoOpFloatSub floatSub;

        bench.run("int publish (2-type publisher)", [&] {
            pub.sendInt(42);
        });

        bench.run("float publish (2-type publisher)", [&] {
            pub.sendFloat(3.14f);
        });
    }

    // --- Subscribe/Unsubscribe churn ---

    bench.title("Subscribe/Unsubscribe churn");

    {
        IntPublisher pub;
        bench.run("create + destroy subscriber", [&] {
            NoOpSubscriber sub;
            ankerl::nanobench::doNotOptimizeAway(&sub);
        });
    }

    // --- No subscribers (empty publish) ---

    bench.title("Edge cases");

    {
        IntPublisher pub;
        bench.run("publish with 0 subscribers", [&] {
            pub.send(42);
        });
    }

    return 0;
}
