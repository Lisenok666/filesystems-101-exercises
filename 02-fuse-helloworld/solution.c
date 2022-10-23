#include <solution.h>

#include <fuse.h>
//my include
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

const char *LINK = "hello";

static void *HelloWorldInit(struct fuse_conn_info *conn, struct fuse_config *cfg) {
    (void) conn;
    cfg->kernel_cache = 1;
    return NULL;
}

int HellloWorldOpenDir((const char* path, struct fuse_file_info* ffi)) {
    (void) ffi;
    if (strcmp(path, "/") != 0)
        return -ENOENT;
    else
        return 0;
}

static int HelloWorldRead(onst char *path, void *buf, size_t size, off_t offset, struct fuse_file_info *ffi) {
    (void) ffi;
    size_t len;

    if(strcmp(path+1, LINK) != 0)
        return -ENOENT;

    struct fuse_context *con = fuse_get_context();
    char *out = (char *)malloc(sizeof(char)*64);
    sprintf(out, "hello, %d\n", con->pid);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, out, size);
        return  size;
    }
    else {
        size = 0;
        return size;
    }
}

static int HelloWorldReadDir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
                              struct fuse_file_info *ffi, enum fuse_readdir_flags flags) {
    (void) ffi;
    (void) offset;
    (void) flags;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    filler(buf, LINK, NULL, 0, 0);

    return 0;
}

static int HelloWorldgOpen(const char *path, struct fuse_file_info *ffi) {
    if (strcmp(path+1, LINK) != 0)
        return -ENOENT;

    if ((ffi->flags & O_ACCMODE) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int HelloWorldgGetAttr(const char *path, struct stat *stbuf, struct fuse_file_info *ffi) {
    (void) ffi;
    int res_return = 0;

    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    else if (strcmp(path+1, LINK) == 0) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = 64;
    }
    else
        res_return = -ENOENT;

    return res_return;
}

static int HelloWorldgCreate(const char* path, mode_t mode, struct fuse_file_info *ffi) {
    (void) ffi;
    (void) path;
    (void) mode;
    return -EROFS;
}

static int HelloWorldgWrite(const char* path, const char* buf, size_t size, off_t offset,
                                        struct fuse_file_info *ffi) {
    (void) path;
    (void) buf;
    (void) size;
    (void) offset;
    (void) ffi;
    if (strcmp(path+1, NAME) != 0)
        return -ENOENT;
    return -EROFS;
}

static const struct fuse_operations hellofs_ops = {
	/* implement me */
	.read = HelloWorldRead,
	.readdir = HelloWorldReadDir,
	.getattr = HelloWorldgGetAttr,
	.open = HelloWorldgOpen,
	.create = HelloWorldgCreate,
	.init = InitHello,
	.write = HelloWorldgWrite,
	.opendir = HellloWorldOpenDir,
};

int helloworld(const char *mntp)
{
	char *argv[] = {"exercise", "-f", (char *)mntp, NULL};
	return fuse_main(3, argv, &hellofs_ops, NULL);
}
