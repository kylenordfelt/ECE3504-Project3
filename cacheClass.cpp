#include "cacheClass.hpp"


cacheClass::cacheClass(std::size_t cacheSize, std::size_t blockSize, bool writePolicy, const PlacementType& placementType) {
	m_cacheSize = cacheSize;
	m_blockSize = blockSize;
	m_writeBack = writePolicy;
	m_placementType = placementType;
	// Calculate dependent variables
	m_numBlocks = cacheSize / blockSize;
	switch (m_placementType) {
	case PlacementType::DM:
		m_numSets = m_numBlocks; // Direct-mapped cache has one set per block
		break;
	case PlacementType::FA:
		m_numSets = 1; // Fully associative cache has one set for all blocks
		break;
	case PlacementType::_4W:
		m_numSets = m_numBlocks / 4; //4-way set associative
		break;
	case PlacementType::_2W:
		m_numSets = m_numBlocks / 2; //2-way set associative
		break;
	default:
		std::cerr << "Unknown placement type" << std::endl;
		exit(EXIT_FAILURE);
	}
	m_setSize = m_numBlocks / m_numSets; // Number of blocks per set

	// Initialize hits and misses
	m_hits = 0;
	m_misses = 0;

	// Initialize cache vectors
	m_tags.resize(m_numBlocks, 0); // Tags for each address
	m_validBits.resize(m_numBlocks, 0); // Valid bits for each block
	m_lruPriority.resize(m_numBlocks, 0); // LRU bits for each block (for set associative caches)
	m_dirtyBits.resize(m_numBlocks, 0); // Dirty bits for each block (for write-back caches)

	// Calculate access offsets/masks
	m_setOffset = static_cast<std::size_t>(std::log2(blockSize)); // Offset within a block
	m_setBits = static_cast<std::size_t>(std::log2(m_numSets)); // Number of bits for set index
	m_setMask = (1 << m_setBits) - 1; // Mask for set index
	m_tagOffset = m_setOffset + m_setBits; // Offset for tag bits
}

cacheClass::~cacheClass() {
	// Destructor implementation (if needed)
}

std::size_t cacheClass::getSetIndex(std::size_t address) const {
	// Extract the set index from the address
	return (address >> m_setOffset) & m_setMask;
}

std::size_t cacheClass::getTag(std::size_t address) const {
	// Extract the tag from the address
	return address >> m_tagOffset;
}

requestResults cacheClass::accessCache(std::size_t address, bool write) {
    requestResults result{ false,false };

    // 1) Identify set and tag
    std::size_t setIndex = getSetIndex(address);
    std::size_t tag = getTag(address);

    // 2) Compute base index into your flat arrays
    std::size_t base = setIndex * m_setSize;

    int    victimWay = -1;
    int    invalidWay = -1;

    // 3) Probe all ways for hit or first invalid
    for (std::size_t way = 0; way < m_setSize; ++way) {
        std::size_t idx = base + way;

        if (!m_validBits[idx]) {
            // remember the first invalid slot
            if (invalidWay < 0)
                invalidWay = static_cast<int>(way);
            continue;
        }
        if (m_tags[idx] == tag) {
            // HIT!
            result.hit = true;
            m_hits++;
            victimWay = static_cast<int>(way);

            if (write) {
                if (m_writeBack) {
                    // mark dirty, memory update deferred
                    m_dirtyBits[idx] = 1;
                }
                else {
                    // WT → immediate memory write
                    result.dirty = true;
                }
            }
            break;
        }
    }

    // 4) MISS path: choose victimWay
    if (!result.hit) {
        m_misses++;

        if (invalidWay >= 0) {
            // fill an empty line first
            victimWay = invalidWay;
        }
        else {
            // evict true LRU (max priority)
            int maxP = -1;
            for (std::size_t way = 0; way < m_setSize; ++way) {
                int p = m_lruPriority[base + way];
                if (p > maxP) {
                    maxP = p;
                    victimWay = static_cast<int>(way);
                }
            }
            // if WB and this line was dirty, we must write it back
            std::size_t evictIdx = base + victimWay;
            if (m_writeBack && m_dirtyBits[evictIdx]) {
                result.dirty = true;
            }
        }

        // 5) Install the new block at victimWay
        std::size_t idx = base + victimWay;
        m_validBits[idx] = 1;
        m_tags[idx] = tag;

        // clear old dirty state on a read miss (WB)
        m_dirtyBits[idx] = 0;

        if (write) {
            if (m_writeBack) {
                // mark newly allocated block dirty
                m_dirtyBits[idx] = 1;
            }
            else {
                // WT on a write miss → allocate then write to memory
                result.dirty = true;
            }
        }
    }

    // 6) Update LRU priorities (0 = MRU, highest = LRU)
    {
        std::size_t idx = base + victimWay;
        int oldP = m_lruPriority[idx];

        // bump everyone as needed
        for (std::size_t way = 0; way < m_setSize; ++way) {
            auto& p = m_lruPriority[base + way];
            if (p <= oldP)
                ++p;
        }
        // this line is now most‑recently‑used
        m_lruPriority[idx] = 0;
    }

    return result;
}


