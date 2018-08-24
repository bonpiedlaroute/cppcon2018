#include <iostream>
#include <string>
#include <filesystem>
#include <cstdint>
#include <llvm/Support/FileSystem.h>
#include <benchmark/benchmark.h>


namespace llvmfs = llvm::sys::fs;

int64_t counter = 0;


const auto linux_folder = std::string("linux-4.16.8");
const auto gecko_folder = std::string("gecko-dev-master");
const auto hadoop_folder = std::string("hadoop-rel-release-2.8.4");


const char* get_filename_ext(const char* filename)
{
   const char* ext = strrchr(filename, '.');
   if (!ext || ext == filename) return "";

   return ext;

}

void do_some_work(const char* file)
{
   counter++;
}

void rdi_current(const std::string& folder)
{
   counter = 0;
   std::error_code ec;
   const auto rdi_end = llvmfs::recursive_directory_iterator();

   for (auto rdi = llvmfs::recursive_directory_iterator(llvm::Twine(folder), ec, false); rdi != rdi_end ; rdi.increment(ec))
   {
      const auto filename = rdi->path();
      const auto ext = get_filename_ext(filename.c_str());

      if (strcmp(ext, ".c") == 0 || strcmp(ext, ".h") == 0 ||
         strcmp(ext, ".cpp") == 0 || strcmp(ext, ".hpp") == 0)
      {
         do_some_work(filename.c_str());
      }
   }
}

void rdi_regex(const std::string& folder)
{
   counter = 0;
   std::error_code ec;
   const auto rdi_end = llvmfs::recursive_directory_iterator_r();

   for (auto rdi = llvmfs::recursive_directory_iterator_r(llvm::Twine(folder), ec, std::regex(".*\\.h|.*\\.c|.*\\.cpp|.*\\.hpp"), llvmfs::pattern_options::file_only, false);
      rdi != rdi_end; rdi.increment(ec))
   {
      const auto filename = rdi->path();
      do_some_work(filename.c_str());
   }
}

void rdi_lambda(const std::string& folder)
{
   counter = 0;
   std::error_code ec;
   const auto rdi_end = llvmfs::recursive_directory_iterator_l();

   for (auto rdi = llvmfs::recursive_directory_iterator_l(llvm::Twine(folder), ec, [](const char* filename)
         {
            auto ext = get_filename_ext(filename);
            return strcmp(ext, ".c") == 0 ||
               strcmp(ext, ".h") == 0 ||
               strcmp(ext, ".cpp") == 0 ||
               strcmp(ext, ".hpp") == 0;
         },
      llvmfs::pattern_options::file_only, false);
      rdi != rdi_end; rdi.increment(ec))
   {
      const auto filename = rdi->path();
      do_some_work(filename.c_str());
   }
}

static void BM_rdi_current(benchmark::State& state)
{
   for (auto _ : state)
   {
      rdi_current(gecko_folder);
   }
}

static void BM_rdi_regex(benchmark::State& state)
{
   for (auto _ : state)
   {
      rdi_regex(gecko_folder);
   }
}

static void BM_rdi_lambda(benchmark::State& state)
{
   for (auto _ : state)
   {
      rdi_lambda(gecko_folder);
   }
}


BENCHMARK(BM_rdi_current);

BENCHMARK(BM_rdi_regex);

BENCHMARK(BM_rdi_lambda);

BENCHMARK_MAIN();


