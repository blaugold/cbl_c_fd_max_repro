#include "cbl/CouchbaseLite.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

static CBLDatabase *db = nullptr;
static CBLReplicator *repl = nullptr;

// Leaks [fdCount] file descriptors.
static void openFileDescriptors(int fdCount)
{
    for (int i = 0; i < fdCount; i++)
    {
        new std::ofstream("/dev/null");
    }

    std::cout << "Opened " << fdCount << " file descriptors" << std::endl;
}

static void initCouchbaseLite()
{
    CBLLog_SetConsoleLevel(kCBLLogInfo);
}

static void openDatabase()
{
    db = CBLDatabase_Open(FLStr("db"), nullptr, nullptr);
    assert(db);
}

static void createReplicator()
{
    auto endpoint = CBLEndpoint_CreateWithURL(FLStr("ws://localhost:5000/db"), nullptr);
    assert(endpoint);

    CBLReplicatorConfiguration config{
        .database = db,
        .endpoint = endpoint,
        .maxAttempts = 1,
    };

    repl = CBLReplicator_Create(&config, nullptr);
    assert(repl);
    CBLEndpoint_Free(endpoint);
}

static void runReplicator()
{
    using namespace std::chrono_literals;

    CBLReplicator_Start(repl, false);

    while (CBLReplicator_Status(repl).activity != kCBLReplicatorStopped)
    {
        std::this_thread::sleep_for(100ms);
    }
}

static void cleanUp()
{
    CBLReplicator_Release(repl);

    auto closedDb = CBLDatabase_Close(db, nullptr);
    assert(closedDb);
    CBLDatabase_Release(db);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <fd-count>" << std::endl;
        return 1;
    }

    auto fdCount = std::stoi(argv[1]);
    openFileDescriptors(fdCount);

    initCouchbaseLite();
    openDatabase();
    createReplicator();
    runReplicator();
    cleanUp();

    return 0;
}
