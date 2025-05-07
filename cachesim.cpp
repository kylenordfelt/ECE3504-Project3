#include "cacheClass.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>  // for EXIT_FAILURE

// ––– Sizes from the spec –––
static const std::vector<std::size_t> CACHE_SIZES = { 1024, 2048, 8192, 65536 };
static const std::vector<std::size_t> BLOCK_SIZES = { 4,    8,   32,   256 };

static const std::vector<PlacementType> PLACEMENTS = {
    PlacementType::DM,
    PlacementType::_2W,
    PlacementType::_4W,
    PlacementType::FA
};
static const std::vector<std::string> WRITE_POLICIES = { "WB", "WT" };

static std::string placementToString(PlacementType p) {
    switch (p) {
    case PlacementType::DM:  return "DM";
    case PlacementType::_2W: return "2W";
    case PlacementType::_4W: return "4W";
    case PlacementType::FA:  return "FA";
    }
    return "?";
}

int main() {
    const std::string traceFile = "test.trace";
    const std::string resultFile = "test.result";

    // Open input trace
    std::ifstream in(traceFile);
    if (!in) {
        std::cerr << "Error: cannot open trace file '" << traceFile << "'\n";
        return EXIT_FAILURE;
    }

    // Open output result
    std::ofstream out(resultFile);
    if (!out) {
        std::cerr << "Error: cannot create result file '" << resultFile << "'\n";
        return EXIT_FAILURE;
    }

    // Iterate over all configurations
    for (auto cs : CACHE_SIZES) {
        for (auto bs : BLOCK_SIZES) {
            for (auto pt : PLACEMENTS) {
                for (auto const& wp : WRITE_POLICIES) {
                    bool writeBack = (wp == "WB");

                    // Reset file to beginning
                    in.clear(); in.seekg(0, std::ios::beg);

                    cacheClass cache(cs, bs, writeBack, pt);

                    // Statistics
                    std::size_t accesses = 0, hits = 0, misses = 0;
                    std::size_t bytesToCache = 0, bytesToMem = 0;
                    std::string line;

                    // Process each trace line
                    while (std::getline(in, line)) {
                        std::istringstream iss(line);
                        std::string op;
                        unsigned long addr;
                        if (!(iss >> op >> std::hex >> addr))
                            continue;

                        bool isWrite = (op == "write");
                        auto res = cache.accessCache(addr, isWrite);

                        ++accesses;
                        if (res.hit) {
                            ++hits;
                        }
                        else {
                            ++misses;
                            bytesToCache += bs;
                        }
                        if (res.dirty) {
                            bytesToMem += bs;
                        }
                    }

                    // Hit rate
                    double hitRate = accesses ? double(hits) / accesses : 0.0;

                    // Blocks per set
                    std::size_t bps;
                    switch (pt) {
                    case PlacementType::DM:  bps = 1;         break;
                    case PlacementType::_2W: bps = 2;         break;
                    case PlacementType::_4W: bps = 4;         break;
                    case PlacementType::FA:  bps = (cs / bs); break;
                    }

                    // Write one line to test.result
                    out
                        << cs << ' '
                        << bs << ' '
                        << placementToString(pt) << ' '
                        << bps << ' '
                        << wp << ' '
                        << accesses << ' '
                        << hits << ' '
                        << std::fixed << std::setprecision(2)
                        << hitRate << ' '
                        << bytesToCache << ' '
                        << bytesToMem
                        << '\n';
                }
            }
        }
    }

    return 0;
}