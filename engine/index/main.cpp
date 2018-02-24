#include "common/communication/server/server_factory.hpp"
#include "common/communication/server/server.hpp"

#include <easylogging/easylogging++.h>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        LOG(ERROR) << "usage: index working_directory_path bind_ip";
        return 1;
    }

    const auto working_directory = argv[1];
    const auto bind_address = argv[2];

    LOG(INFO) << "Indexer starting.";
    LOG(INFO) << "Working directory: " << working_directory;
    LOG(INFO) << "Bind address: " << bind_address;

    auto server = usl::common::communication::server::server_factory{}.create(bind_address);
    server.run();

    return 0;
}