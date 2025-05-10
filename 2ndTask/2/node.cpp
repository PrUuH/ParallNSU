#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

std::string executeCommand(const std::string& command) {
    std::string result = "";
    char buffer[128];
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe) {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != nullptr) {
                result += buffer;
            }
        }
        pclose(pipe);
    }
    return result;
}

void writeCSV(const std::string& filename, const std::vector<std::pair<std::string, std::string>>& data) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to create file: " << filename << std::endl;
        return;
    }

    file << "Parameter,Value\n";

    for (const auto& entry : data) { 
        file << entry.first << "," << entry.second << "\n";
    }

    file.close();
}

int main() {
    std::string cpuInfo = executeCommand("lscpu");

    std::string serverName = executeCommand("cat /sys/devices/virtual/dmi/id/product_name");

    std::string numaInfo = executeCommand("numactl --hardware");

    std::istringstream numaStream(numaInfo);
    std::string numaLine;
    int numaNodeCount = 0;
    std::vector<std::string> memoryPerNode;

    while (std::getline(numaStream, numaLine)) {
        if (numaLine.find("node") != std::string::npos && numaLine.find("size") != std::string::npos) {
            memoryPerNode.push_back(numaLine);
            numaNodeCount++;
        }
    }

    std::string osInfo = executeCommand("cat /etc/os-release");

    std::istringstream cpuStream(cpuInfo);
    std::string line;
    std::string architecture, modelName, totalCPUs;

    while (std::getline(cpuStream, line)) {
        if (line.find("Architecture:") != std::string::npos) {
            architecture = line.substr(line.find(":") + 1);
        } else if (line.find("Model name:") != std::string::npos) {
            modelName = line.substr(line.find(":") + 1);
        } else if (line.find("CPU(s):") != std::string::npos) {
            totalCPUs = line.substr(line.find(":") + 1);
        }
    }

    std::istringstream osStream(osInfo);
    std::string osName, osVersion;

    while (std::getline(osStream, line)) {
        if (line.find("NAME=") != std::string::npos) {
            osName = line.substr(line.find("=") + 1);
            osName.erase(0, osName.find_first_not_of("\""));
            osName.erase(osName.find_last_not_of("\"") + 1);
        } else if (line.find("VERSION=") != std::string::npos) {
            osVersion = line.substr(line.find("=") + 1);
            osVersion.erase(0, osVersion.find_first_not_of("\""));
            osVersion.erase(osVersion.find_last_not_of("\"") + 1);
        }
    }

    std::vector<std::pair<std::string, std::string>> data = {
        {"CPU Architecture", architecture},
        {"CPU Model Name", modelName},
        {"Total CPUs", totalCPUs},
        {"NUMA Nodes", std::to_string(numaNodeCount)}
    };

    for (size_t i = 0; i < memoryPerNode.size(); ++i) {
        data.emplace_back("Memory Node " + std::to_string(i), memoryPerNode[i].substr(memoryPerNode[i].find(":") + 1));
    }

    data.emplace_back("Server Name", serverName);

    data.emplace_back("OS Name", osName);
    data.emplace_back("OS Version", osVersion);

    writeCSV("system_info.csv", data);

    std::cout << "Data has been written to system_info.csv" << std::endl;

    return 0;
}