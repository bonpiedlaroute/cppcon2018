#include <iostream>
#include <string>
#include <experimental/filesystem>
#include <fstream>
#include <cstdint>
#include <benchmark/benchmark.h>

namespace fs=std::experimental::filesystem::v1;

int64_t counter = 0;

const auto linux_folder = std::string("linux-4.16.8");
const auto gecko_folder = std::string("gecko-dev-master");
const auto hadoop_folder = std::string("hadoop-rel-release-2.8.4");


const char* get_filename_ext(const char* filename)
{
   const char* ext = strrchr(filename, '.');
   if ( !ext || ext == filename ) return "";

   return ext;

}


void do_some_work(const char* file)
{
   counter++;
}

void rdi_current(const std::string& folder)
{
   counter = 0;
   for(auto& entry : fs::recursive_directory_iterator(folder))
   {
      auto ext = entry.path().extension().c_str();

      if( strcmp(ext, ".c") == 0 || strcmp(ext, ".h") == 0 || 
          strcmp(ext, ".cpp") == 0 || strcmp(ext, ".hpp") == 0 )
      {
         do_some_work(entry.path().filename().c_str());        
      }
   }
}

void rdi_regex(const std::string& folder)
{
   counter = 0;
   for(auto& entry : fs::recursive_directory_iterator_r(folder, std::regex(".*\\.h|.*\\.c|.*\\.cpp|.*\\.hpp")
                                                             , fs::pattern_options::file_only))
   {
      do_some_work(entry.path().filename().c_str());        
   } 
}


void rdi_lambda(const std::string& folder)
{
   counter = 0;
   for(auto& entry : fs::recursive_directory_iterator_l(folder, [](const char* filename) 
                                                                                      { 
                                                                                       auto ext = get_filename_ext(filename); 
                                                                                       return strcmp(ext, ".c") == 0 || 
                                                                                              strcmp(ext, ".h") == 0 || 
                                                                                              strcmp(ext, ".cpp") == 0 || 
                                                                                              strcmp(ext, ".hpp") == 0;
                                                                                       }
                                                                                       , fs::pattern_options::file_only ))
   {
      do_some_work(entry.path().filename().c_str());
   }
}

static void BM_rdi_current(benchmark::State& state)
{
   for(auto _ : state)
   {
      rdi_current(gecko_folder);
   }
}

static void BM_rdi_regex(benchmark::State& state)
{
   for(auto _ : state)
   {
      rdi_regex(gecko_folder);
   }
}

static void BM_rdi_lambda(benchmark::State& state)
{
   for(auto _ : state)
   {
      rdi_lambda(gecko_folder);
   }
}


BENCHMARK(BM_rdi_current);

BENCHMARK(BM_rdi_regex);

BENCHMARK(BM_rdi_lambda);


BENCHMARK_MAIN();

