#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
using namespace std;
namespace fs = filesystem;

// Function to copy a file
void copyFile(const fs::path& source, const fs::path& target) {
    fs::copy_file(source, target, fs::copy_options::overwrite_existing);
    cout << "Copied file: " << source << " to " << target << endl; // Log to console
}

// Function to remove a file
void deleteFile(const fs::path& file) {
    fs::remove(file);
    cout << "Removed file: " << file << endl; // Log to console
}

// Function to synchronize folders
void synchronization(const fs::path& sourceDir, const fs::path& replicaDir, const fs::path& logFile) {
    ofstream log(logFile, ios_base::app);
    if (!log.is_open()) {
        cerr << "Error: Unable to open log file." << endl;
        return;
    }

    // Remove files in replica folder that do not exist in source folder
    for (const auto& entry : fs::directory_iterator(replicaDir)) {
        const fs::path replicaPath = entry.path();
        const fs::path sourcePath = sourceDir / replicaPath.filename();

        if (!fs::exists(sourcePath)) {
            deleteFile(replicaPath);
            log << "Removed file: " << replicaPath << std::endl;
        }
    }

    // Copy files from source folder to replica folder
    for (const auto& entry : fs::directory_iterator(sourceDir)) {
        const fs::path sourcePath = entry.path();
        const fs::path replicaPath = replicaDir / sourcePath.filename();

        if (fs::is_regular_file(sourcePath)) {
            if (fs::exists(replicaPath)) {
                deleteFile(replicaPath);
                log << "Removed file: " << replicaPath << endl;
            }
            copyFile(sourcePath, replicaPath);
            log << "Copied file: " << sourcePath << " to " << replicaPath << endl;
        }
    }

    log.close();
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " <sourceDir> <replicaDir> <logFile> <syncInterval>" << endl;
        return 1;
    }

    const fs::path sourceDir = argv[1];
    const fs::path replicaDir = argv[2];
    const fs::path logFile = argv[3];
    const int syncInterval = stoi(argv[4]);

    while (true) {
        synchronization(sourceDir, replicaDir, logFile);
        this_thread::sleep_for(chrono::seconds(syncInterval));
    }

    return 0;
}
 