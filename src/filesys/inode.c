#include "filesys/inode.h"
#include "filesys/cache.h"
#include "filesys/filesys.h"
#include "filesys/free-map.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include <debug.h>
#include <list.h>
#include <round.h>
#include <string.h>

/* Identifies an inode. */
#define INODE_MAGIC 0x494e4f44
#define DIRECT_BLOCK_ENTRIES 123
#define INDIRECT_BLOCK_ENTRIES 128
#define NORMAL_DIRECT 0
#define INDIRECT 1
#define DOUBLE_INDIRECT 2
#define SECTOR_MAGIC 0xFFFFFFFF

struct sector_location
{
    int directness;
    int index1;
    int index2;
};

struct inode_indirect_block
{
    block_sector_t map_table[INDIRECT_BLOCK_ENTRIES];
};

/* On-disk inode.
   Must be exactly BLOCK_SECTOR_SIZE bytes long. */
struct inode_disk
{
    off_t length;   /* File size in bytes. */
    unsigned magic; /* Magic number. */
    block_sector_t direct_map_table[DIRECT_BLOCK_ENTRIES];
    block_sector_t indirect_block_sec;
    block_sector_t double_indirect_block_sec;
    bool is_dir;
};

/* In-memory inode. */
struct inode
{
    struct list_elem elem; /* Element in inode list. */
    block_sector_t sector; /* Sector number of disk location. */
    int open_cnt;          /* Number of openers. */
    bool removed;          /* True if deleted, false otherwise. */
    int deny_write_cnt;    /* 0: writes ok, >0: deny writes. */
    struct lock lock;
};

static void locate_byte(off_t, struct sector_location *);
static bool register_sector(struct inode_disk *, block_sector_t, struct sector_location);
static bool inode_update_file_length(struct inode_disk *, off_t, off_t);
static void free_inode_sectors(struct inode_disk *);
static void init_indirect_block(struct inode_indirect_block *block);

/* Returns the block device sector that contains byte offset POS
   within INODE.
   Returns -1 if INODE does not contain data for a byte at offset
   POS. */
static block_sector_t byte_to_sector(const struct inode_disk *inode_disk, off_t pos)
{
    if (pos >= inode_disk->length)
    {
        return -1;
    }

    struct sector_location sec_loc;
    locate_byte(pos, &sec_loc);

    if (sec_loc.directness == NORMAL_DIRECT)
    {
        return inode_disk->direct_map_table[sec_loc.index1];
    }
    else if (sec_loc.directness == INDIRECT)
    {
        struct inode_indirect_block *ind_block = (struct inode_indirect_block *)malloc(BLOCK_SECTOR_SIZE);
        block_sector_t sector = inode_disk->indirect_block_sec;

        if (sector == SECTOR_MAGIC)
        {
            free(ind_block);
            return -1;
        }

        buffer_cache_read(sector, ind_block, 0, sizeof(struct inode_indirect_block), 0);
        block_sector_t result_sector = ind_block->map_table[sec_loc.index1];
        free(ind_block);

        return result_sector;
    }
    else if (sec_loc.directness == DOUBLE_INDIRECT)
    {
        struct inode_indirect_block *ind_block = (struct inode_indirect_block *)malloc(BLOCK_SECTOR_SIZE);
        block_sector_t sector = inode_disk->double_indirect_block_sec;

        if (sector == SECTOR_MAGIC)
        {
            free(ind_block);
            return -1;
        }

        buffer_cache_read(sector, ind_block, 0, sizeof(struct inode_indirect_block), 0);

        sector = ind_block->map_table[sec_loc.index1];
        if (sector == SECTOR_MAGIC)
        {
            free(ind_block);
            return -1;
        }

        buffer_cache_read(sector, ind_block, 0, sizeof(struct inode_indirect_block), 0);

        block_sector_t result_sector = ind_block->map_table[sec_loc.index2];
        free(ind_block);

        return result_sector;
    }
    return -1;
}

/* List of open inodes, so that opening a single inode twice
   returns the same `struct inode'. */
static struct list open_inodes;

/* Initializes the inode module. */
void inode_init(void)
{
    list_init(&open_inodes);
}

/* Initializes an inode with LENGTH bytes of data and
   writes the new inode to sector SECTOR on the file system
   device.
   Returns true if successful.
   Returns false if memory or disk allocation fails. */
bool inode_create(block_sector_t sector, off_t length, bool is_dir)
{
    struct inode_disk *disk_inode = NULL;

    ASSERT(length >= 0);

    /* If this assertion fails, the inode structure is not exactly
       one sector in size, and you should fix that. */
    ASSERT(sizeof *disk_inode == BLOCK_SECTOR_SIZE);

    disk_inode = calloc(1, sizeof *disk_inode);
    if (disk_inode != NULL)
    {
        init_indirect_block(disk_inode);
        disk_inode->magic = INODE_MAGIC;
        disk_inode->is_dir = is_dir;

        if (!inode_update_file_length(disk_inode, disk_inode->length, length))
        {
            free(disk_inode);
            return false;
        }

        buffer_cache_write(sector, disk_inode, 0, BLOCK_SECTOR_SIZE, 0);
        free(disk_inode);
        return true;
    }
    return false;
}

/* Reads an inode from SECTOR
   and returns a `struct inode' that contains it.
   Returns a null pointer if memory allocation fails. */
struct inode *inode_open(block_sector_t sector)
{
    struct list_elem *e;
    struct inode *inode;

    /* Check whether this inode is already open. */
    for (e = list_begin(&open_inodes); e != list_end(&open_inodes);
         e = list_next(e))
    {
        inode = list_entry(e, struct inode, elem);
        if (inode->sector == sector)
        {
            inode_reopen(inode);
            return inode;
        }
    }

    /* Allocate memory. */
    inode = malloc(sizeof *inode);
    if (inode == NULL)
        return NULL;

    /* Initialize. */
    list_push_front(&open_inodes, &inode->elem);
    inode->sector = sector;
    inode->open_cnt = 1;
    inode->deny_write_cnt = 0;
    inode->removed = false;

    lock_init(&inode->lock);

    return inode;
}

/* Reopens and returns INODE. */
struct inode *
inode_reopen(struct inode *inode)
{
    if (inode != NULL)
        inode->open_cnt++;
    return inode;
}

/* Returns INODE's inode number. */
block_sector_t
inode_get_inumber(const struct inode *inode)
{
    return inode->sector;
}

/* Closes INODE and writes it to disk.
   If this was the last reference to INODE, frees its memory.
   If INODE was also a removed inode, frees its blocks. */
void inode_close(struct inode *inode)
{
    /* Ignore null pointer. */
    if (inode == NULL)
        return;

    /* Release resources if this was the last opener. */
    if (--inode->open_cnt == 0)
    {
        /* Remove from inode list and release lock. */
        list_remove(&inode->elem);

        /* Deallocate blocks if removed. */
        if (inode->removed)
        {
            struct inode_disk inode_disk;
            buffer_cache_read(inode->sector, &inode_disk, 0, BLOCK_SECTOR_SIZE, 0);
            free_inode_sectors(&inode_disk);
            free_map_release(inode->sector, 1);
        }

        free(inode);
    }
}

/* Marks INODE to be deleted when it is closed by the last caller who
   has it open. */
void inode_remove(struct inode *inode)
{
    ASSERT(inode != NULL);
    inode->removed = true;
}

/* Reads SIZE bytes from INODE into BUFFER, starting at position OFFSET.
   Returns the number of bytes actually read, which may be less
   than SIZE if an error occurs or end of file is reached. */
off_t inode_read_at(struct inode *inode, void *buffer_, off_t size, off_t offset)
{
    struct inode_disk inode_disk;
    uint8_t *buffer = buffer_;
    off_t bytes_read = 0;

    lock_acquire(&inode->lock);
    buffer_cache_read(inode->sector, &inode_disk, 0, sizeof(struct inode_disk), 0);
    lock_release(&inode->lock);

    while (size > 0)
    {
        /* Disk sector to read, starting byte offset within sector. */
        block_sector_t sector_idx = byte_to_sector(&inode_disk, offset);
        int sector_ofs = offset % BLOCK_SECTOR_SIZE;

        /* Bytes left in inode, bytes left in sector, lesser of the two. */
        off_t inode_left = inode_disk.length - offset;
        int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
        int min_left = inode_left < sector_left ? inode_left : sector_left;

        /* Number of bytes to actually copy out of this sector. */
        int chunk_size = size < min_left ? size : min_left;
        if (chunk_size <= 0)
            break;

        buffer_cache_read(sector_idx, buffer, bytes_read, chunk_size, sector_ofs);

        /* Advance. */
        size -= chunk_size;
        offset += chunk_size;
        bytes_read += chunk_size;
    }
    return bytes_read;
}

/* Writes SIZE bytes from BUFFER into INODE, starting at OFFSET.
   Returns the number of bytes actually written, which may be
   less than SIZE if end of file is reached or an error occurs.
   (Normally a write at end of file would extend the inode, but
   growth is not yet implemented.) */
off_t inode_write_at(struct inode *inode, const void *buffer_, off_t size, off_t offset)
{
    struct inode_disk inode_disk;
    const uint8_t *buffer = buffer_;
    off_t bytes_written = 0;

    if (inode->deny_write_cnt)
        return 0;

    lock_acquire(&inode->lock);
    buffer_cache_read(inode->sector, &inode_disk, 0, sizeof(struct inode_disk), 0);

    if (inode_disk.length < offset + size)
    {
        inode_update_file_length(&inode_disk, inode_disk.length, offset + size);
        buffer_cache_write(inode->sector, &inode_disk, 0, BLOCK_SECTOR_SIZE, 0);
    }
    lock_release(&inode->lock);

    while (size > 0)
    {
        /* Sector to write, starting byte offset within sector. */
        block_sector_t sector_idx = byte_to_sector(&inode_disk, offset);
        int sector_ofs = offset % BLOCK_SECTOR_SIZE;

        /* Bytes left in inode, bytes left in sector, lesser of the two. */
        off_t inode_left = inode_disk.length - offset;
        int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
        int min_left = inode_left < sector_left ? inode_left : sector_left;

        /* Number of bytes to actually write into this sector. */
        int chunk_size = size < min_left ? size : min_left;
        if (chunk_size <= 0)
            break;

        buffer_cache_write(sector_idx, (void *)buffer, bytes_written, chunk_size, sector_ofs);

        /* Advance. */
        size -= chunk_size;
        offset += chunk_size;
        bytes_written += chunk_size;
    }
    return bytes_written;
}

/* Disables writes to INODE.
   May be called at most once per inode opener. */
void inode_deny_write(struct inode *inode)
{
    inode->deny_write_cnt++;
    ASSERT(inode->deny_write_cnt <= inode->open_cnt);
}

/* Re-enables writes to INODE.
   Must be called once by each inode opener who has called
   inode_deny_write() on the inode, before closing the inode. */
void inode_allow_write(struct inode *inode)
{
    ASSERT(inode->deny_write_cnt > 0);
    ASSERT(inode->deny_write_cnt <= inode->open_cnt);
    inode->deny_write_cnt--;
}

static void locate_byte(off_t pos, struct sector_location *sec_loc)
{
    pos /= BLOCK_SECTOR_SIZE;

    if (pos < DIRECT_BLOCK_ENTRIES)
    {
        sec_loc->directness = NORMAL_DIRECT;
        sec_loc->index1 = pos;
    }
    else if (pos < INDIRECT_BLOCK_ENTRIES + DIRECT_BLOCK_ENTRIES)
    {
        pos -= DIRECT_BLOCK_ENTRIES;
        sec_loc->directness = INDIRECT;
        sec_loc->index1 = pos;
    }
    else if (pos < INDIRECT_BLOCK_ENTRIES * INDIRECT_BLOCK_ENTRIES + INDIRECT_BLOCK_ENTRIES + DIRECT_BLOCK_ENTRIES)
    {
        pos -= INDIRECT_BLOCK_ENTRIES + DIRECT_BLOCK_ENTRIES;
        sec_loc->directness = DOUBLE_INDIRECT;
        sec_loc->index1 = pos / INDIRECT_BLOCK_ENTRIES;
        sec_loc->index2 = pos % INDIRECT_BLOCK_ENTRIES;
    }
    else
    {
        sec_loc->directness = -1;
    }
}

static void init_indirect_block(struct inode_indirect_block *block)
{
    int i;
    for (i = 0; i < INDIRECT_BLOCK_ENTRIES; ++i)
    {
        block->map_table[i] = SECTOR_MAGIC;
    }
}

static bool register_sector(struct inode_disk *inode_disk, block_sector_t new_sector, struct sector_location sec_loc)
{

    struct inode_indirect_block first, second;
    block_sector_t *sector;

    if (sec_loc.directness == NORMAL_DIRECT)
    {
        inode_disk->direct_map_table[sec_loc.index1] = new_sector;
        return true;
    }
    else if (sec_loc.directness == INDIRECT)
    {
        sector = &inode_disk->indirect_block_sec;
        if (*sector == SECTOR_MAGIC)
        {
            if (free_map_allocate(1, sector))
            {
                init_indirect_block(&second);
            }
            else
            {
                return false;
            }
        }
        else
        {
            buffer_cache_read(*sector, &second, 0, sizeof(struct inode_indirect_block), 0);
        }

        if (second.map_table[sec_loc.index1] == SECTOR_MAGIC)
        {
            second.map_table[sec_loc.index1] = new_sector;
        }

        buffer_cache_write(*sector, &second, 0, sizeof(struct inode_indirect_block), 0);
        return true;
    }
    else if (sec_loc.directness == DOUBLE_INDIRECT)
    {
        sector = &inode_disk->double_indirect_block_sec;
        if (*sector == SECTOR_MAGIC)
        {
            if (free_map_allocate(1, sector))
            {
                init_indirect_block(&first);
            }
            else
            {
                return false;
            }
        }
        else
        {
            buffer_cache_read(*sector, &first, 0, sizeof(struct inode_indirect_block), 0);
        }
        sector = &first.map_table[sec_loc.index1];

        if (*sector == SECTOR_MAGIC)
        {
            if (free_map_allocate(1, sector))
            {
                init_indirect_block(&second);
                if (second.map_table[sec_loc.index2] == SECTOR_MAGIC)
                {
                    second.map_table[sec_loc.index2] = new_sector;
                }
                buffer_cache_write(inode_disk->double_indirect_block_sec, &first, 0, sizeof(struct inode_indirect_block), 0);
                buffer_cache_write(*sector, &second, 0, sizeof(struct inode_indirect_block), 0);
            }
            else
            {
                return false;
            }
        }
        else
        {
            buffer_cache_read(*sector, &second, 0, sizeof(struct inode_indirect_block), 0);
            if (second.map_table[sec_loc.index2] == SECTOR_MAGIC)
            {
                second.map_table[sec_loc.index2] = new_sector;
            }
            buffer_cache_write(*sector, &second, 0, sizeof(struct inode_indirect_block), 0);
        }

        return true;
    }
    return false;
}

static bool inode_update_file_length(struct inode_disk *inode_disk, off_t start_pos, off_t end_pos)
{
    static char zeroes[BLOCK_SECTOR_SIZE];
    inode_disk->length = end_pos;

    start_pos = start_pos / BLOCK_SECTOR_SIZE * BLOCK_SECTOR_SIZE;
    end_pos = (end_pos - 1) / BLOCK_SECTOR_SIZE * BLOCK_SECTOR_SIZE;

    while (start_pos <= end_pos)
    {
        block_sector_t sector = byte_to_sector(inode_disk, start_pos);
        if (sector != SECTOR_MAGIC)
        {
            start_pos += BLOCK_SECTOR_SIZE;
            continue;
        }
        else
        {
            if (free_map_allocate(1, &sector))
            {
                struct sector_location sec_loc;
                locate_byte(start_pos, &sec_loc);
                if (!register_sector(inode_disk, sector, sec_loc))
                {
                    return false;
                }
                buffer_cache_write(sector, zeroes, 0, BLOCK_SECTOR_SIZE, 0);
                start_pos += BLOCK_SECTOR_SIZE;
            }
            else
            {
                return false;
            }
        }
    }
    return true;
}

static void free_inode_sectors(struct inode_disk *inode_disk)
{
    int i, j;
    if (inode_disk->double_indirect_block_sec != SECTOR_MAGIC)
    {
        struct inode_indirect_block *ind_block_1 = (struct inode_indirect_block *)malloc(BLOCK_SECTOR_SIZE);
        buffer_cache_read(inode_disk->double_indirect_block_sec, ind_block_1, 0, sizeof(struct inode_indirect_block), 0);
        i = 0;
        while (ind_block_1->map_table[i] != SECTOR_MAGIC)
        {
            j = 0;
            struct inode_indirect_block *ind_block_2 = (struct inode_indirect_block *)malloc(BLOCK_SECTOR_SIZE);
            buffer_cache_read(ind_block_2->map_table[j], ind_block_2, 0, sizeof(struct inode_indirect_block), 0);

            while (ind_block_2->map_table[j] != SECTOR_MAGIC)
            {
                free_map_release(ind_block_2->map_table[j], 1);
                j++;
            }
            free_map_release(ind_block_1->map_table[i], 1);
            free(ind_block_2);
            i++;
        }
        free(ind_block_1);
        return;
    }

    if (inode_disk->indirect_block_sec != SECTOR_MAGIC)
    {
        struct inode_indirect_block *ind_block = (struct inode_indirect_block *)malloc(BLOCK_SECTOR_SIZE);
        buffer_cache_read(inode_disk->indirect_block_sec, ind_block, 0, sizeof(struct inode_indirect_block), 0);
        i = 0;
        while (ind_block->map_table[i] != SECTOR_MAGIC)
        {
            free_map_release(ind_block->map_table[i], 1);
            i++;
        }
        free(ind_block);
        return;
    }

    i = 0;
    while (inode_disk->direct_map_table[i] != SECTOR_MAGIC)
    {
        free_map_release(inode_disk->direct_map_table[i], 1);
        i++;
    }
    return;
}

off_t inode_length(struct inode *inode)
{
    off_t res;
    struct inode_disk *disk_inode = (struct inode_disk *)malloc(BLOCK_SECTOR_SIZE);
    buffer_cache_read(inode->sector, disk_inode, 0, BLOCK_SECTOR_SIZE, 0);
    res = disk_inode->length;
    free(disk_inode);
    return res;
}

bool inode_is_dir(struct inode *inode)
{
    if (inode->removed)
    {
        return false;
    }
    bool res;
    struct inode_disk *disk_inode = (struct inode_disk *)malloc(BLOCK_SECTOR_SIZE);
    buffer_cache_read(inode->sector, disk_inode, 0, BLOCK_SECTOR_SIZE, 0);
    res = disk_inode->is_dir;
    free(disk_inode);
    return res;
}

int get_deny_write_cnt(struct inode *inode)
{
    return inode->deny_write_cnt;
}