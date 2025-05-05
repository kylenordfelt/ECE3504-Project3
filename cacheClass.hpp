#ifndef CACHECLASS_HPP
#define CACHECLASS_HPP

#include <cstddef> // for std::size_t
#include <iostream> // for std::cout
#include <vector> // for std::vector

enum class PlacementType {
	DM,
	_2W,
	_4W,
	FA
};
typedef enum class PlacementType PlacementType;

class cacheClass {
public:
	//constructor
	cacheClass(std::size_t cacheSize, std::size_t blockSize, const std::string& writePolicy, const PlacementType& placementType);
	//destructor
	~cacheClass();



private:
	//independent variables
	std::size_t	m_cacheSize;
	std::size_t	m_blockSize;
	std::string m_writePolicy;
	PlacementType m_placementType;

	//dependent variables
	std::size_t m_numBlocks;
	std::size_t m_numSets;
	std::size_t m_setSize;
	std::size_t m_hits;
	std::size_t m_misses;

};



#endif