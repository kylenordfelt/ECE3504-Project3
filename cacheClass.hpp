#ifndef CACHECLASS_HPP
#define CACHECLASS_HPP

#include <cstddef> // for std::size_t
#include <iostream> // for std::cout
#include <vector> // for std::vector
#include <cmath> // for std::log2
#include <string> // for std::string

enum class PlacementType {
	DM,
	_2W,
	_4W,
	FA
};
typedef enum class PlacementType PlacementType;

struct requestResults {
	bool hit;
	bool dirty;
};

class cacheClass {
public:
	//constructor
	cacheClass(std::size_t cacheSize, std::size_t blockSize, bool writePolicy, const PlacementType& placementType);
	//destructor
	~cacheClass();
	
	//helper functions
	std::size_t getSetIndex(std::size_t address) const;
	std::size_t getTag(std::size_t address) const;

	requestResults accessCache(std::size_t address, bool write);


private:
	//independent variables
	std::size_t	m_cacheSize;
	std::size_t	m_blockSize;
	bool m_writeBack;
	PlacementType m_placementType;

	//dependent variables
	std::size_t m_numBlocks;
	std::size_t m_numSets;
	std::size_t m_setSize;
	std::size_t m_hits;
	std::size_t m_misses;

	//cache vectors
	std::vector<std::size_t> m_tags; // Tags for each 
	std::vector<std::size_t> m_validBits; // Valid bits for each block
	std::vector<std::size_t> m_lruPriority; // LRU bits for each block (for set associative caches)
	std::vector<std::size_t> m_dirtyBits; // Dirty bits for each block (for write-back caches)

	//access offsets/masks
	std::size_t m_setOffset;
	std::size_t m_tagOffset;
	std::size_t m_setMask;
	std::size_t m_setBits;

};



#endif