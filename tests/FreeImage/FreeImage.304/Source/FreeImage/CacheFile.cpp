// ==========================================================
// Multi-Page functions
//
// Design and implementation by
// - Floris van den Berg (flvdberg@wxs.nl)
//
// This file is part of FreeImage 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================

#pragma warning (disable : 4786) // identifier was truncated to 'number' characters

#include "CacheFile.h"

// ----------------------------------------------------------

CacheFile::CacheFile(const char *filename, BOOL keep_in_memory) :
m_file(NULL),
m_filename(NULL),
m_free_pages(),
m_page_cache(),
m_page_map(),
m_page_count(0),
m_current_block(NULL),
m_keep_in_memory(keep_in_memory) {
	if (filename) {
		m_filename = new char[strlen(filename) + 1];

		strcpy(m_filename, filename);
	}
}

CacheFile::~CacheFile() {
	if (m_filename) {
		delete [] m_filename;
	}
}

BOOL
CacheFile::open() {
	if ((m_filename) && (!m_keep_in_memory)) {
		m_file = fopen(m_filename, "w+b");

		return (m_file != NULL);
	}

	return (m_keep_in_memory == TRUE);
}

void
CacheFile::close() {
	// dispose the cache entries

	while (!m_page_cache.empty()) {
		Cache *block = *m_page_cache.begin();

		m_page_cache.pop_front();
		delete block->data;
		delete block;
	}

	if ((m_filename) && (!m_keep_in_memory)) {
		// close the file

		fclose(m_file);

		// delete the file

		remove(m_filename);
	}
}

int
CacheFile::allocateBlock() {
	//Block *block = new Block;
	//memset(block, 0, sizeof(Block));

	if (!m_free_pages.empty()) {
		int free_page = *m_free_pages.begin();
		m_free_pages.pop_front();

		return free_page;
	} else {
		return m_page_count++;
	}
}

Block *
CacheFile::lockBlock(int nr) {
	if (m_current_block == NULL) {
		PageMapIt it = m_page_map.find(nr);

		if (it != m_page_map.end()) {
			// block is in cache...

			m_current_block = (*(it->second))->data;

			return m_current_block;
		} else {
			// block is not in cache...

			if (!m_keep_in_memory) {
				if (m_page_cache.size() > CACHE_SIZE) {
					// flush the least used block to file

					Cache *old_block = *m_page_cache.begin();

					fseek(m_file, old_block->nr * sizeof(Block), SEEK_SET);
					fwrite(old_block->data, sizeof(Block), 1, m_file);

					// remove the block from memory

					m_page_map.erase(old_block->nr);
					m_page_cache.pop_front();

					delete old_block->data;
					delete old_block;
				}
			}

			// allocate a new block

			m_current_block = new Block;
			memset(m_current_block, 0, sizeof(Block));

			// allocate a new cache entry

			Cache *cache = new Cache();
			cache->nr = nr;
			cache->data = m_current_block;

			// add the new block to the cache

			m_page_cache.push_back(cache);
			m_page_map[nr] = --m_page_cache.end(); // this is possible only for std::list!

			return m_current_block;
		}
	}

	return NULL;
}

BOOL
CacheFile::unlockBlock(int nr) {
	if (m_current_block) {
		m_current_block = NULL;

		return TRUE;
	}

	return FALSE;
}

BOOL
CacheFile::deleteBlock(int nr) {
	if (!m_current_block) {
		PageMapIt it = m_page_map.find(nr);

		// remove block from cache

		if (it != m_page_map.end())
			m_page_map.erase(nr);

		// add block to free page list

		m_free_pages.push_back(nr);

		return TRUE;
	}

	return FALSE;
}

BOOL
CacheFile::readFile(BYTE *data, int nr, int size) {
	if ((data) && (size > 0)) {
		int block_size = sizeof(Block) - 4;
		int s = 0;
		int block_nr = nr;

		do {
			int copy_nr = block_nr;

			Block *block = lockBlock(copy_nr);

			block_nr = block->next;

			memcpy(data + s, block->data, (s + block_size > size) ? size - s : block_size);

			unlockBlock(copy_nr);

			s += block_size;
		} while (block_nr != 0);

		return TRUE;
	}

	return FALSE;
}

int
CacheFile::writeFile(BYTE *data, int size) {
	if ((data) && (size > 0)) {
		int block_size = sizeof(Block) - 4;
		int nr_blocks_required = 1 + (size / block_size);
		int count = 0;
		int s = 0;
		int stored_alloc;
		int alloc;
		
		stored_alloc = alloc = allocateBlock();

		do {
			int copy_alloc = alloc;

			Block *block = lockBlock(copy_alloc);

			block->next = 0;

			memcpy(block->data, data + s, (s + block_size > size) ? size - s : block_size);

			if (count + 1 < nr_blocks_required)
				alloc = block->next = allocateBlock();

			unlockBlock(copy_alloc);

			s += block_size;			
		} while (++count < nr_blocks_required);

		return stored_alloc;
	}

	return 0;
}

void
CacheFile::deleteFile(int nr) {
	int next = nr;
	
	do {
		Block *block = lockBlock(next);

		next = block->next;

		unlockBlock(nr);

		deleteBlock(nr);
	} while (next != 0);
}
