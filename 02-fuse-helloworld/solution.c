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
    (void) cfg;

    cfg->kernel_cache = 1;
    cfg->uid = getgid();
    cfg->set_gid = 1;
    cfg->gid = getgid();
    cfg->set_mode = 1;
    cfg->umask = ~S_IRUSR;

    content = (char*) malloc(64 * sizeof(char));
    if (!content)
        exit(-ENOMEM);

    return NULL;
}

int HelloWorldOpenDir((const char* path, struct fuse_file_info* ffi)) {
    (void) ffi;
    if (strcmp(path, "/") != 0)
        return -ENOENT;
    else
        return 0;
}

static int HelloWorldRead(onst char *path, void *buf, size_t size, off_t offset, struct fuse_file_info *ffi) {
    (void) ffi;

    if(strcmp(path+1, LINK) != 0)
        return -ENOENT;

    struct fuse_context *context = fuse_get_context();
    char *out = (char *)malloc(sizeof(char)*64);
    sprintf(out, "hello, %d\n", context->pid);
    int len = (int)strlen(out);
    if (offset < len)
    {
        if ((offset + (int)size) > len)
        {
            memcpy(buf, out, len - offset);
            return len - offset;
        }
        memcpy(buf, out, size);
        return size;
    }
    return 0;
}

static int HelloWorldReadDir(const char *path, void *buf, fuse_fill_dir_t fill, off_t offset, struct fuse_file_info *ffi, enum fuse_readdir_flags flags)
{
    (void)offset;
    (void)ffi;
    (void)flags;
    if (strcmp(path, "/") != 0)
        return -ENOENT;

    fill(buf, ".", NULL, 0, 0);
    fill(buf, "..", NULL, 0, 0);
    fill(buf, LINK, NULL, 0, 0);
    return 0;
}

static int HelloWorldOpen(const char *path, struct fuse_file_info *ffi) {
    if (strcmp(path+1, LINK) != 0)
        return -ENOENT;

    if ((ffi->flags & O_ACCMODE) != O_RDONLY)
        return -EROFS;

    return 0;
}

static int HelloWorldGetAttr(const char *path, struct stat *stbuf,
                         struct fuse_file_info *fi)
{
        (void) fi;
        int res = 0;
 
        memset(stbuf, 0, sizeof(struct stat));
        if (strcmp(path, "/") == 0) {
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
        } else if (strcmp(path+1, options.filename) == 0) {
                stbuf->st_mode = S_IFREG | 0444;
                stbuf->st_nlink = 1;
                stbuf->st_size = strlen(options.contents);
        } else
                res = -ENOENT;
 
        return res;
}

static int HelloWorldCreate(const char* path, mode_t mode, struct fuse_file_info *ffi) {
    (void) path;
    (void) mode;
    (void) ffi;
    return -EROFS;
}

static int HelloWorldWrite(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info *ffi) {
    (void) path;
    (void) buf;
    (void) size;
    (void) offset;
    (void) ffi;
    if (strcmp(path+1, NAME) == 0)
        return -EROFS;
    return -ENOENT;
}

static const struct fuse_operations hellofs_ops = {
	/* implement me */
	.read = HelloWorldRead,
	.readdir = HelloWorldReadDir,
	.getattr = HelloWorldGetAttr,
	.open = HelloWorldOpen,
	.create = HelloWorldCreate,
	.init = InitHello,
	.write = HelloWorldWrite,
	.opendir = HelloWorldOpenDir,
};

int helloworld(const char *mntp)
{
	char *argv[] = {"exercise", "-f", (char *)mntp, NULL};
	return fuse_main(3, argv, &hellofs_ops, NULL);
}
