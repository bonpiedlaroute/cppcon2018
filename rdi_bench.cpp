#include <iostream>
#include <string>
#include <experimental/filesystem>
#include <fstream>
#include <cstdint>
#include <benchmark/benchmark.h>

namespace fs=std::experimental::filesystem::v1;

int64_t counter = 0, counter_r = 0, counter_l = 0;

const auto linux_folder = std::string("linux-4.16.8");
const auto gecko_folder = std::string("gecko-dev-master");
const auto hadoop_folder = std::string("hadoop-rel-release-2.8.4");


const char* get_filename_ext(const char* filename)
{
   const char* ext = strrchr(filename, '.');
   if ( !ext || ext == filename ) return "";

   return ext;

}


void do_some_work(const std::string& file)
{
   counter++;
  //std::cout << file << "\n"; 
}


void do_some_work_r(const std::string& file)
{
   counter_r++;
   //std::cout << file << "\n";
}

void do_some_work_l(const std::string& file)
{
   counter_l++;
   //std::cout << file << "\n";
}

std::string get_filename(const fs::directory_entry& entry)
{
   std::ostringstream ostring;
   ostring << entry;
   auto filename = ostring.str();
   return filename.substr(1,filename.length() - 2);
}


void rdi_current(const std::string& folder)
{
   for(auto& path : fs::recursive_directory_iterator(folder))
   {
      auto filename = get_filename(path);

      if( filename.substr(filename.length() - 2) == ".c" || filename.substr(filename.length() - 2) == ".h" 
           || filename.substr(filename.length() - 4) == ".cpp" || filename.substr(filename.length() - 4) == ".hpp" )
      {
         do_some_work(filename);        
      }
   }
}

void rdi_regex(const std::string& folder)
{

   for(auto& path : fs::recursive_directory_iterator_r(folder, std::regex(".*\\.h|.*\\.c|.*\\.cpp|.*\\.hpp")
                                                             , fs::pattern_options::file_only))
   {
      auto filename = get_filename(path);

      do_some_work_r(filename);        
   } 
}


void rdi_lambda(const std::string& folder)
{

   for(auto& path : fs::recursive_directory_iterator_l(folder, [](const char* filename) 
                                                                                      { 
                                                                                       const char* ext = get_filename_ext(filename); 
                                                                                       return strcmp(ext, ".c") == 0 || 
                                                                                              strcmp(ext, ".h") == 0 || 
                                                                                              strcmp(ext, ".cpp") == 0 || 
                                                                                              strcmp(ext, ".hpp") == 0;
                                                                                       }
                                                                                       , fs::pattern_options::file_only ))
   {
      auto filename = get_filename(path);

      do_some_work_l(filename);
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
