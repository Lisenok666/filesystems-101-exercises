#include <solution.h>
#include <errno.h>
#include <assert.h>
#include <sys/stat.h>
#include <liburing.h>
#include <stdlib.h>

#define ENTRIES 8
#define QD	4
#define BS	(256*1024)

struct io_data {
    int read;
    off_t first_offset, offset;
    size_t first_len;
    struct iovec iov;
};

static int get_file_size(int fd, off_t *size)
{
    struct stat st;

    if (fstat(fd, &st) < 0)
        return -1;
    if (S_ISREG(st.st_mode)) {
        *size = st.st_size;
        return 0;
    }
    return -1;
}

static int write_query(int out, struct io_uring *ring, struct io_data *data)
{
    data->read = 0;
    data->offset = data->first_offset;

    data->iov.iov_base = data + 1;
    data->iov.iov_len = data->first_len;


    struct io_uring_sqe *sqe;

    sqe = io_uring_get_sqe(ring);
    assert(sqe);

    if (data->read)
        io_uring_prep_readv(sqe, in, &data->iov, 1, data->offset);
    else
        io_uring_prep_writev(sqe, out, &data->iov, 1, data->offset);

    io_uring_sqe_set_data(sqe, data);
    io_uring_submit(ring);
}

static int read_query(int in, struct io_uring *ring, off_t size, off_t offset)
{
    struct io_uring_sqe *sqe;
    struct io_data *data;

    data = malloc(size + sizeof(*data));
    if (!data)
        return 1;

    sqe = io_uring_get_sqe(ring);
    if (!sqe) {
        free(data);
        return 1;
    }

    data->read = 1;
    data->offset = data->first_offset = offset;

    data->iov.iov_base = data + 1;
    data->iov.iov_len = size;
    data->first_len = size;

    io_uring_prep_readv(sqe, in, &data->iov, 1, offset);
    io_uring_sqe_set_data(sqe, data);
    return 0;
}

static int copy_file(int in, int out, struct io_uring *ring, off_t in_size)
{
    unsigned reads, writes;
    struct io_uring_cqe *cqe;
    off_t write_left, offset;
    int ret;
    struct io_data *data;
    int write_left_block;
    write_left = in_size;
    reads = writes = offset = 0;

    while (in_size || write_left)
    {
        write_left_block = 0;
        unsigned read_check = reads;
        for (int i = 0; i < QD; i++)
        {
            off_t size = in_size;
            if (size > BS)
                size = BS;
            if (size == 0)
                break;
            if (read_query(in, ring, size, offset))
                break;
            write_left_block += size;
            in_size -= size;
            offset += size;
            reads++;
        }

        if (read_check != reads)
        {
            ret = io_uring_submit(ring);
            if (ret < 0)
                return -errno;
        }

        while (write_left_block)
        {
            ret = io_uring_wait_cqe(ring, &cqe);
            if (ret != 0)
                return -errno;
            data = io_uring_cqe_get_data(cqe);

            if (data->read)
            {
                ret = write_query(out, ring, date);
                if (ret != 0)
                    return ret;
                write_left -= data->first_len;
                write_left_block -= data->first_len;
                reads--;
                writes++;
            }
            else
            {
                free(data);
                writes--;
            }
            io_uring_cqe_seen(ring, cqe);
        }
        for (unsigned i = 0; i < writes; i++)
        {
            ret = io_uring_wait_cqe(ring, &cqe);
            if (ret != 0)
                return -errno;
            data = io_uring_cqe_get_data(cqe);
            if (!data)
                break;
            free(data);
            io_uring_cqe_seen(ring, cqe);
        }
        writes = 0;
    }
    return 0;
}

int copy(int in, int out)
{
	(void) in;
	(void) out;

    struct io_uring ring;
    off_t insize;
	int answer = 0;

    if (io_uring_queue_init(ENTRIES, &ring, 0) < 0)
        return -errno;

    if (get_file_size(in, &insize))
        return -errno;

    answer = copy_file(in, out, &ring, insize);
    io_uring_queue_exit(&ring);
	return answer;
}
