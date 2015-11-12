#ifndef FILESYS_FILE_H
#define FILESYS_FILE_H

#include "filesys/off_t.h"
#include "lib/kernel/list.h"

struct inode;

/* An open file. */
/* Assume that only one thread access this structure at once. */
struct file 
  {
    struct inode *inode;        /* File's inode. */
    off_t pos;                  /* Current position. */
    bool deny_write;            /* Has file_deny_write() been called? */
    
#ifdef USERPROG
    /* added by taeguk */
    int fd;                     /* File descriptor of file. */
    struct list_elem file_elem;
    //struct lock lock;
#endif
  };

/* Opening and closing files. */
struct file *file_open (struct inode *);
struct file *file_reopen (struct file *);
void file_close (struct file *);
struct inode *file_get_inode (struct file *);

/* Reading and writing. */
off_t file_read (struct file *, void *, off_t);
off_t file_read_at (struct file *, void *, off_t size, off_t start);
off_t file_write (struct file *, const void *, off_t);
off_t file_write_at (struct file *, const void *, off_t size, off_t start);

/* Preventing writes. */
void file_deny_write (struct file *);
void file_allow_write (struct file *);

/* File position. */
void file_seek (struct file *, off_t);
off_t file_tell (struct file *);
off_t file_length (struct file *);

/* added by taeguk for project 2-2 */
void file_acquire_lock (struct file *);
void file_release_lock (struct file *);

#endif /* filesys/file.h */
