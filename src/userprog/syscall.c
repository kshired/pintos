#include "userprog/syscall.h"
#include "console.h"
#include "devices/input.h"
#include "devices/shutdown.h"
#include "filesys/directory.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "filesys/inode.h"
#include "process.h"
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define SYS_CALL_NUMS 22
#define FD_START 3
#define FD_END 128
#define READDIR_MAX_LEN 14
#define PATH_MAX_LEN 256

struct lock file_lock;

static void syscall_handler(struct intr_frame *);
void syscall_init(void);

// utils
void chk_valid_vaddr(uint32_t *esp, int n);
void init(int *syscall_arg_num, int (*fp[SYS_CALL_NUMS])(uint32_t *));

// System calls
int halt(uint32_t *esp);
int exit(uint32_t *esp);
int exec(uint32_t *esp);
int wait(uint32_t *esp);
int write(uint32_t *esp);
int read(uint32_t *esp);

// project2
int create(uint32_t *esp);
int remove(uint32_t *esp);
int open(uint32_t *esp);
int close(uint32_t *esp);
int filesize(uint32_t *esp);
int seek(uint32_t *esp);
int tell(uint32_t *esp);

// Additional Implement System calls
int max_of_four_int(uint32_t *esp);
int fibonnaci(uint32_t *esp);

void chk_file(int f);

// project5
int chdir(uint32_t *esp);
int mkdir(uint32_t *esp);
int readdir(uint32_t *esp);
int isdir(uint32_t *esp);
int inumber(uint32_t *esp);

struct dir
{
    struct inode *inode; /* Backing store. */
    off_t pos;           /* Current position. */
};

/*
    1. Initialize syscall arg num for each syscall
    2. Initialize function pointer for each syscall
*/
void init(int *syscall_arg_num, int (*fp[SYS_CALL_NUMS])(uint32_t *))
{
    syscall_arg_num[SYS_HALT] = 0;
    fp[SYS_HALT] = halt;
    syscall_arg_num[SYS_EXIT] = 1;
    fp[SYS_EXIT] = exit;
    syscall_arg_num[SYS_EXEC] = 1;
    fp[SYS_EXEC] = exec;
    syscall_arg_num[SYS_WAIT] = 1;
    fp[SYS_WAIT] = wait;
    syscall_arg_num[SYS_WRITE] = 3;
    fp[SYS_WRITE] = write;
    syscall_arg_num[SYS_READ] = 3;
    fp[SYS_READ] = read;
    syscall_arg_num[SYS_FIBO] = 1;
    fp[SYS_FIBO] = fibonnaci;
    syscall_arg_num[SYS_MAX_FOUR_INT] = 4;
    fp[SYS_MAX_FOUR_INT] = max_of_four_int;

    // project 2
    syscall_arg_num[SYS_CREATE] = 2;
    fp[SYS_CREATE] = create;
    syscall_arg_num[SYS_REMOVE] = 1;
    fp[SYS_REMOVE] = remove;
    syscall_arg_num[SYS_OPEN] = 1;
    fp[SYS_OPEN] = open;
    syscall_arg_num[SYS_FILESIZE] = 1;
    fp[SYS_FILESIZE] = filesize;
    syscall_arg_num[SYS_SEEK] = 2;
    fp[SYS_SEEK] = seek;
    syscall_arg_num[SYS_TELL] = 1;
    fp[SYS_TELL] = tell;
    syscall_arg_num[SYS_CLOSE] = 1;
    fp[SYS_CLOSE] = close;

    // project 5
    syscall_arg_num[SYS_CHDIR] = 1;
    fp[SYS_CHDIR] = chdir;
    syscall_arg_num[SYS_MKDIR] = 1;
    fp[SYS_MKDIR] = mkdir;
    syscall_arg_num[SYS_READDIR] = 2;
    fp[SYS_READDIR] = readdir;
    syscall_arg_num[SYS_ISDIR] = 1;
    fp[SYS_ISDIR] = isdir;
    syscall_arg_num[SYS_INUMBER] = 1;
    fp[SYS_INUMBER] = inumber;
}

/*
    Check vaddr is user vaddr ( esp[0] ~ esp[n])
*/
void chk_valid_vaddr(uint32_t *esp, int n)
{
    int i;
    for (i = 0; i <= n; ++i)
    {
        if (!is_user_vaddr((void *)esp[i]))
        {
            exit(-1);
        }
    }
}

static void syscall_handler(struct intr_frame *f)
{
    // array for record the number of syscall's argument
    int syscall_arg_num[SYS_CALL_NUMS] = {
        0,
    };

    // function pointer array for record the function pointer for each syscall
    int (*fp[SYS_CALL_NUMS])(uint32_t *);

    // init syscall_arg_num and fp
    init(syscall_arg_num, fp);

    // esp ( stack pointer )
    uint32_t *esp = (uint32_t *)f->esp;

    // check address is valid
    chk_valid_vaddr(esp, syscall_arg_num[*esp]);
    if (fp[*esp] == NULL)
    {
        exit(-1);
    }
    /*
        Below syscalls don't have a return value.
    */
    if (*esp == SYS_HALT || *esp == SYS_EXIT || *esp == SYS_SEEK || *esp == SYS_CLOSE || *esp == SYS_MUNMAP)
    {
        fp[*esp](esp);
    }
    else // Below syscalls have a return value. Set value to eax.
    {
        f->eax = fp[*esp](esp);
    }
}

void syscall_init(void)
{
    lock_init(&file_lock);
    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

// halt : shutdown power off
int halt(uint32_t *esp)
{
    shutdown_power_off();
    return 0;
}

/*
    exit
    1. get current thread name from thread_current
    2. set status to thread
    3. thread exit
*/
int exit(uint32_t *esp)
{
    int status = -1;
    if (esp != -1)
    {
        status = (int)esp[1];
    }
    int i;
    char thread[256];
    for (i = 0; i < strlen(thread_name()) + 1; ++i)
    {
        if (thread_name()[i] == ' ')
        {
            break;
        }
    }
    strlcpy(thread, thread_name(), i + 1);
    printf("%s: exit(%d)\n", thread, status);
    thread_current()->exit_status = status;

    for (i = FD_START; i < FD_END; ++i)
    {
        if (thread_current()->file_descriptor[i] != NULL)
        {
            struct inode *node = file_get_inode(thread_current()->file_descriptor[i]);
            if (node != NULL && get_deny_write_cnt(node) > 0)
            {
                if (inode_is_dir(node))
                {
                    dir_close(thread_current()->file_descriptor[i]);
                }
                else
                {
                    file_close(thread_current()->file_descriptor[i]);
                }
            }
            thread_current()->file_descriptor[i] = NULL;
        }
    }
    if (thread_current()->working_dir)
        dir_close(thread_current()->working_dir);
    thread_exit();
    return 0;
}

/*
    wait : process_wait(pid)
*/
int wait(uint32_t *esp)
{
    return process_wait((int)esp[1]);
}

/*
    exec : process_execute(file_name)
*/
int exec(uint32_t *esp)
{
    return process_execute((char *)esp[1]);
}

/*
    write
    1. check file descriptor
    2. if fd == STDOUT : write to buffer and return size
    3. else : return -1
*/
int write(uint32_t *esp)
{

    int fd = (int)esp[1];
    void *buffer = (void *)esp[2];
    unsigned size = (int)esp[3];
    if (fd == STDOUT)
    {
        lock_acquire(&file_lock);
        putbuf(buffer, size);
        lock_release(&file_lock);
        return size;
    }
    else if (fd >= FD_START && fd < FD_END)
    {
        chk_file(fd);
        lock_acquire(&file_lock);
        int val = file_write(thread_current()->file_descriptor[fd], buffer, size);
        lock_release(&file_lock);
        return val;
    }
    else
    {
        exit(-1);
    }
    return -1;
}

/*
    read
    1. check file descriptor
    2. if fd == STDIN : input_getc and return size
    3. else : return -1
*/
int read(uint32_t *esp)
{
    int fd = (int)esp[1];
    void *buffer = (void *)esp[2];
    unsigned size = (int)esp[3];
    int i;

    if (fd == 0)
    {
        lock_acquire(&file_lock);
        for (i = 0; i < size; i++)
        {
            if (((char *)buffer)[i] == '\0')
            {
                break;
            }
        }
        lock_release(&file_lock);
    }
    else if (fd >= FD_START && fd < FD_END)
    {
        chk_file(fd);
        lock_acquire(&file_lock);
        int val = file_read(thread_current()->file_descriptor[fd], buffer, size);
        lock_release(&file_lock);
        return val;
    }
    else
    {
        exit(-1);
    }
    return i;
}

/*
    fibonnaci
    1. n : index of fibonnaci sequence
    2. return n-th fibonnaci value
*/
int fibonnaci(uint32_t *esp)
{
    int n = (int)esp[1];
    int i;
    int res = 0;
    int tmp1 = 0;
    int tmp2 = 1;
    for (i = 0; i < n; ++i)
    {
        tmp1 = res + tmp2;
        res = tmp2;
        tmp2 = tmp1;
    }
    return res;
}

/*
    max_of_four_int
    1. esp[1] ~ esp[4] : n1 ~ n4
    2. get max value from n1 to n4
    3. return max value
*/
int max_of_four_int(uint32_t *esp)
{
    int i;
    int m = 0;
    for (i = 1; i <= 4; ++i)
    {
        if (m < (int)esp[i])
        {
            m = (int)esp[i];
        }
    }
    return m;
}

int create(uint32_t *esp)
{
    char *file = (char *)esp[1];
    unsigned size = (unsigned)esp[2];

    if (file == NULL)
    {
        exit(-1);
    }
    int val = filesys_create(file, size);
    return val;
}

int remove(uint32_t *esp)
{
    char *file = (char *)esp[1];
    if (file == NULL)
    {
        exit(-1);
    }
    int val = filesys_remove(file);
    return val;
}

int open(uint32_t *esp)
{
    char *file = (char *)esp[1];
    int i;
    if (file == NULL)
    {
        exit(-1);
    }
    lock_acquire(&file_lock);
    struct file *f = filesys_open(file);
    lock_release(&file_lock);
    if (f == NULL)
    {
        return -1;
    }

    for (i = FD_START; i < FD_END && thread_current()->file_descriptor[i] != NULL; ++i)
    {
    }
    if (i >= FD_END)
    {
        return -1;
    }
    if (strcmp(thread_name(), file) == 0)
    {
        file_deny_write(f);
    }

    thread_current()->file_descriptor[i] = f;

    return i;
}

int close(uint32_t *esp)
{
    int f = (int)esp[1];
    chk_file(f);
    struct file *fp;
    fp = thread_current()->file_descriptor[f];
    file_close(fp);
    thread_current()->file_descriptor[f] = NULL;
    return 0;
}

int filesize(uint32_t *esp)
{
    int f = (int)esp[1];
    chk_file(f);
    int val = file_length(thread_current()->file_descriptor[f]);
    return val;
}

int seek(uint32_t *esp)
{
    int f = (int)esp[1];
    chk_file(f);
    unsigned pos = (unsigned)esp[2];
    file_seek(thread_current()->file_descriptor[f], pos);
    return 0;
}

int tell(uint32_t *esp)
{
    int f = (int)esp[1];
    chk_file(f);

    return file_tell(thread_current()->file_descriptor[f]);
}

void chk_file(int f)
{
    if (f < FD_START && f >= FD_END)
    {
        exit(-1);
    }
    if (thread_current()->file_descriptor[f] == NULL)
    {
        exit(-1);
    }
}

// project 5
int chdir(uint32_t *esp)
{
    char *path = (char *)esp[1];
    return filesys_change_dir(path);
}

int mkdir(uint32_t *esp)
{
    char *dir = (char *)esp[1];

    return filesys_create_dir(dir);
}

int readdir(uint32_t *esp)
{
    int f = (int)esp[1];
    char *name = (char *)esp[2];
    int i = 0;
    bool result = true;

    chk_file(f);

    struct file *file = thread_current()->file_descriptor[f];

    struct inode *inode = file_get_inode(file);
    if (inode == NULL)
    {
        return false;
    }
    if (!inode_is_dir(inode))
    {
        return false;
    }

    struct dir *dir = dir_open(inode);

    dir->pos = file_tell(file);
    result = dir_readdir(dir, name);
    file_seek(file, dir->pos);

    return result;
}

int isdir(uint32_t *esp)
{
    int f = (int)esp[1];
    chk_file(f);

    return inode_is_dir(file_get_inode(thread_current()->file_descriptor[f]));
}

int inumber(uint32_t *esp)
{
    int f = (int)esp[1];
    chk_file(f);

    return inode_get_inumber(file_get_inode(thread_current()->file_descriptor[f]));
}