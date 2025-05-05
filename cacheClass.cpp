#include "cacheClass.hpp"
#include <iostream>


cacheClass::cacheClass(std::size_t cacheSize, std::size_t blockSize, const std::string& writePolicy, const PlacementType& placementType) {
	m_cacheSize = cacheSize;
	m_blockSize = blockSize;
	m_writePolicy = writePolicy;
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
}