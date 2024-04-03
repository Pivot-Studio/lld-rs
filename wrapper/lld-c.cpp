#include <cstdlib>
#include <lld/Common/Driver.h>
#include <mutex>
#include <iostream>

const char* mun_alloc_str(const std::string& str)
{
  size_t size = str.length();
  if(size > 0)
  {
    char *strPtr = reinterpret_cast<char *>(malloc(size + 1));
    memcpy(strPtr, str.c_str(), size + 1);
    return strPtr;
  }
  return nullptr;
}

// LLD seems not to be thread safe. This is terrible. We basically only allow single threaded access to the driver using
// mutexes. Each type of LLD driver seems to be disconnected so we use a mutex for every type.
std::mutex _coffMutex;
std::mutex _elfMutex;
std::mutex _machOMutex;
std::mutex _wasmMutex;

extern "C" {

enum LldFlavor {
  Elf = 0,
  Wasm = 1,
  MachO = 2,
  Coff = 3,
};

struct LldInvokeResult {
  bool success;
  const char* messages;
};

void mun_link_free_result(LldInvokeResult* result)
{
  if(result->messages)
  {
    free(reinterpret_cast<void *>(const_cast<char*>(result->messages)));
  }
}

LldInvokeResult mun_lld_link(LldFlavor flavor, int argc, const char * *argv) {
  std::string outputString, errorString;
  llvm::raw_string_ostream outputStream(outputString);
  llvm::raw_string_ostream errorStream(errorString);
  std::vector<const char*> args(argv, argv + argc);
  LldInvokeResult result;
  // argc & argc to llvm::ArrayRef
  auto argsRef = llvm::ArrayRef<const char *>(args.data(), args.size());
  auto drivers = std::vector<lld::DriverDef>();
  auto re = lld::lldMain(argsRef, outputStream, errorStream, drivers);
  result.success = re.retCode;

  // // Delete the global context and clear the global context pointer, so that it
  // // cannot be accessed anymore.
  std::string resultMessage = errorStream.str() + outputStream.str();
  result.messages = mun_alloc_str(resultMessage);
  return result;
}

}
