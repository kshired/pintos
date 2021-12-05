#include "filesys/filesys.h"
#include "filesys/cache.h"
#include "filesys/directory.h"
#include "filesys/file.h"
#include "filesys/free-map.h"
#include "filesys/inode.h"
#include "threads/thread.h"
#include <debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READDIR_MAX_LEN 14
#define PATH_MAX_LEN 256

/* Partition that contains the file system. */
struct block *fs_device;

static void do_format(void);

/* Initializes the file system module.
   If FORMAT is true, reformats the file system. */
void filesys_init(bool format)
{
    fs_device = block_get_role(BLOCK_FILESYS);
    if (fs_device == NULL)
        PANIC("No file system device found, can't initialize file system.");

    buffer_cache_init();

    inode_init();
    free_map_init();

    if (format)
        do_format();

    thread_current()->cur_dir = dir_open_root();
    free_map_open();
}

/* Shuts down the file system module, writing any unwritten data
   to disk. */
void filesys_done(void)
{
    buffer_cache_terminate();
    free_map_close();
}

/* Creates a file named NAME with the given INITIAL_SIZE.
   Returns true if successful, false otherwise.
   Fails if a file named NAME already exists,
   or if internal memory allocation fails. */
bool filesys_create(const char *name, off_t initial_size)
{
    block_sector_t inode_sector = 0;
    char *parse_name = (char *)malloc(sizeof(char) * (PATH_MAX_LEN + 1));
    struct dir *dir = parse_path(name, parse_name);

    bool success = (dir != NULL && free_map_allocate(1, &inode_sector) && inode_create(inode_sector, initial_size, false) && dir_add(dir, parse_name, inode_sector));
    if (!success && inode_sector != 0)
        free_map_release(inode_sector, 1);
    dir_close(dir);
    free(parse_name);
    return success;
}

/* Opens the file with the given NAME.
   Returns the new file if successful or a null pointer
   otherwise.
   Fails if no file named NAME exists,
   or if an internal memory allocation fails. */
struct file *filesys_open(const char *name)
{
    char *parse_name = (char *)malloc(sizeof(char) * (PATH_MAX_LEN + 1));
    struct dir *dir = parse_path(name, parse_name);
    struct inode *inode = NULL;

    if (dir != NULL)
    {
        dir_lookup(dir, parse_name, &inode);
    }
    dir_close(dir);
    free(parse_name);
    return file_open(inode);
}

/* Deletes the file named NAME.
   Returns true if successful, false on failure.
   Fails if no file named NAME exists,
   or if an internal memory allocation fails. */
bool filesys_remove(const char *name)
{
    char *parse_name = (char *)malloc(sizeof(char) * (PATH_MAX_LEN + 1));
    char *temp = (char *)malloc(sizeof(char) * (PATH_MAX_LEN + 1));
    struct dir *dir = parse_path(name, parse_name);
    bool success = false;
    struct inode *inode;

    dir_lookup(dir, parse_name, &inode);

    if (inode_is_dir(inode))
    {
        struct dir *new_dir = NULL;
        new_dir = dir_open(inode);
        if (new_dir)
        {
            if (!dir_readdir(new_dir, temp))
            {
                success = dir != NULL && dir_remove(dir, parse_name);
            }
            dir_close(new_dir);
        }
    }
    else
    {
        success = dir != NULL && dir_remove(dir, parse_name);
    }

    dir_close(dir);

    free(parse_name);
    free(temp);
    return success;
}

static void add_dot(block_sector_t cur, block_sector_t parent)
{
    struct dir *dir = dir_open(inode_open(cur));
    dir_add(dir, ".", cur);
    dir_add(dir, "..", parent);
    dir_close(dir);
}

/* Formats the file system. */
static void do_format(void)
{
    printf("Formatting file system...");
    free_map_create();
    if (!dir_create(ROOT_DIR_SECTOR, 16))
        PANIC("root directory creation failed");

    add_dot(ROOT_DIR_SECTOR, ROOT_DIR_SECTOR);

    free_map_close();
    printf("done.\n");
}

struct dir *parse_path(char *name, char *file_name)
{
    struct dir *dir = NULL;
    if (!name || !file_name || strlen(name) == 0)
        return NULL;

    char *path = (char *)malloc(sizeof(char) * (PATH_MAX_LEN + 1));
    strlcpy(path, name, PATH_MAX_LEN);

    if (path[0] == '/')
    {
        dir = dir_open_root();
    }
    else
    {
        dir = dir_reopen(thread_current()->cur_dir);
    }

    char *token, *next_token, *save_ptr;
    token = strtok_r(path, "/", &save_ptr);
    next_token = strtok_r(NULL, "/", &save_ptr);

    if (dir == NULL)
    {
        return NULL;
    }

    if (!inode_is_dir(dir_get_inode(dir)))
    {
        return NULL;
    }

    for (; token != NULL && next_token != NULL; token = next_token, next_token = strtok_r(NULL, "/", &save_ptr))
    {
        struct inode *inode = NULL;
        if (!dir_lookup(dir, token, &inode) || !inode_is_dir(inode))
        {
            dir_close(dir);
            return NULL;
        }
        dir_close(dir);
        dir = dir_open(inode);
    }

    if (token == NULL)
    {
        strlcpy(file_name, ".", PATH_MAX_LEN);
    }
    else
    {
        strlcpy(file_name, token, PATH_MAX_LEN);
    }

    free(path);
    return dir;
}

bool filesys_create_dir(char *name)
{
    block_sector_t inode_sector = 0;
    char *parse_name = (char *)malloc(sizeof(char) * (PATH_MAX_LEN + 1));
    struct dir *dir = parse_path(name, parse_name);

    bool success = (dir != NULL && free_map_allocate(1, &inode_sector) && dir_create(inode_sector, 16) && dir_add(dir, parse_name, inode_sector));

    if (success)
    {
        add_dot(inode_sector, inode_get_inumber(dir_get_inode(dir)));
        free(parse_name);
        dir_close(dir);
        return true;
    }
    else
    {
        if (inode_sector)
        {
            free_map_release(inode_sector, 1);
        }
        free(parse_name);
        dir_close(dir);
        return false;
    }

    return false;
}

bool filesys_change_dir(char *path)
{
    char *path_copy = (char *)malloc(sizeof(char) * (PATH_MAX_LEN + 1));
    strlcpy(path_copy, path, PATH_MAX_LEN);
    strlcat(path_copy, "/0", PATH_MAX_LEN);

    char *name = (char *)malloc(sizeof(char) * (PATH_MAX_LEN + 1));
    struct dir *dir = parse_path(path_copy, name);

    if (dir == NULL)
    {
        free(path_copy);
        free(name);
        return false;
    }

    free(path_copy);
    free(name);
    dir_close(thread_current()->cur_dir);
    thread_current()->cur_dir = dir;
    return true;
}