#include "filesys/cache.h"
#include "threads/palloc.h"
#include <debug.h>
#include <string.h>

#define NUM_CACHE 64

struct buffer_cache_entry cache[NUM_CACHE];
struct buffer_cache_entry *clock;
struct lock buffer_cache_lock;

// init buffer cache
void buffer_cache_init(void)
{
    int i;

    // lock init
    lock_init(&buffer_cache_lock);

    // cache init
    for (i = 0; i < NUM_CACHE; ++i)
    {
        memset(&cache[i], 0, sizeof(struct buffer_cache_entry));
        lock_init(&cache[i].lock);
    }

    // clock init to cache
    clock = cache;
}

// terminate buffer_cache
void buffer_cache_terminate(void)
{
    // flush all buffer_cache
    buffer_cache_flush_all();

    // set NULL to clock;
    clock = NULL;
}

// read using buffer cache
bool buffer_cache_read(block_sector_t disk_sector, void *buffer, off_t offset, int chunk_size, int sector_ofs)
{
    struct buffer_cache_entry *cache = buffer_cache_lookup(disk_sector);
    if (cache == NULL) // cache miss
    {
        lock_acquire(&buffer_cache_lock);

        // find replacement cache
        struct buffer_cache_entry *new_cache = buffer_cache_select_victim();

        lock_acquire(&new_cache->lock);

        // replacement cache flush
        buffer_cache_flush_entry(new_cache);

        // bit and disk_sector set
        new_cache->valid_bit = true;
        new_cache->dirty_bit = false;
        new_cache->reference_bit = true;
        new_cache->disk_sector = disk_sector;

        // read from real file
        block_read(fs_device, disk_sector, new_cache->buffer);

        // copy to buffer
        memcpy(buffer + offset, new_cache->buffer + sector_ofs, chunk_size);

        lock_release(&new_cache->lock);
        lock_release(&buffer_cache_lock);
    }
    else // cache find
    {
        lock_acquire(&cache->lock);
        memcpy(buffer + offset, cache->buffer + sector_ofs, chunk_size);
        cache->reference_bit = true;
        lock_release(&cache->lock);
    }
    return true;
}

// write using buffer cache
bool buffer_cache_write(block_sector_t disk_sector, void *buffer, off_t offset, int chunk_size, int sector_ofs)
{
    struct buffer_cache_entry *cache = buffer_cache_lookup(disk_sector);

    if (cache == NULL) // cache miss
    {
        lock_acquire(&buffer_cache_lock);

        // find replacement cache
        struct buffer_cache_entry *new_cache = buffer_cache_select_victim();
        lock_acquire(&new_cache->lock);

        // replacement cache flush
        buffer_cache_flush_entry(new_cache);

        // set information
        new_cache->valid_bit = true;
        new_cache->disk_sector = disk_sector;
        new_cache->reference_bit = true;
        new_cache->dirty_bit = true;

        // read from real file
        block_read(fs_device, disk_sector, new_cache->buffer);

        // copy to cache
        memcpy(new_cache->buffer + sector_ofs, buffer + offset, chunk_size);

        lock_release(&new_cache->lock);
        lock_release(&buffer_cache_lock);
    }
    else // cache find
    {
        lock_acquire(&cache->lock);

        // copy to cache
        memcpy(cache->buffer + sector_ofs, buffer + offset, chunk_size);
        cache->reference_bit = true;
        cache->dirty_bit = true;

        lock_release(&cache->lock);
    }

    return true;
}

// find buffer cache which has same value with disk sector
struct buffer_cache_entry *buffer_cache_lookup(block_sector_t disk_sector)
{
    lock_acquire(&buffer_cache_lock);

    int i;
    for (i = 0; i < NUM_CACHE; ++i)
    {
        if (cache[i].valid_bit && cache[i].disk_sector == disk_sector)
        {
            break;
        }
    }

    lock_release(&buffer_cache_lock);

    // cache find
    if (0 <= i && i < NUM_CACHE)
    {
        return &cache[i];
    }

    // cache miss
    return NULL;
}

// find replacement cache using clock algorithm
struct buffer_cache_entry *buffer_cache_select_victim(void)
{
    int i;
    while (true)
    {
        if (clock == cache + NUM_CACHE)
        {
            clock = cache;
        }
        lock_acquire(&clock->lock);
        if (!clock->valid_bit || !clock->reference_bit)
        {
            struct buffer_cache_entry *res = clock;
            lock_release(&res->lock);
            clock++;
            return res;
        }
        clock->reference_bit = false;
        lock_release(&clock->lock);
        clock++;
    }
}

// flush buffer cache
void buffer_cache_flush_entry(struct buffer_cache_entry *entry)
{
    int i;
    // if cache is using and dirty
    if (entry->valid_bit && entry->dirty_bit)
    {
        block_write(fs_device, entry->disk_sector, entry->buffer);
        for (i = 0; i < BLOCK_SECTOR_SIZE; ++i)
        {
            entry->buffer[i] = NULL;
        }
        entry->dirty_bit = false;
    }
}

// flush all buffer cache
void buffer_cache_flush_all()
{
    int i;
    for (i = 0; i < NUM_CACHE; ++i)
    {
        lock_acquire(&cache[i].lock);
        buffer_cache_flush_entry(&cache[i]);
        lock_release(&cache[i].lock);
    }
}