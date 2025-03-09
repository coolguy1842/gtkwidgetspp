#include <Utils/ProcessUtil.hpp>

using namespace Util::Process;

std::optional<std::string> Util::Process::runCommand(const std::string command, const uint64_t bufSize) {
    std::string out;

    FILE* proc = popen(command.c_str(), "r");
    if(proc == nullptr) {
        return std::nullopt;
    }

    char* buf = (char*)calloc(bufSize, sizeof(char));
    while(fgets(buf, bufSize, proc) != nullptr) {
        out.append(buf);
    }

    pclose(proc);
    free(buf);

    return out;
}