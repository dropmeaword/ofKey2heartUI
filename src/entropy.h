#ifndef __ENTROPY_H__
#define __ENTROPY_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <asm/types.h>
#include <linux/random.h>


int kernel_rng_open();
void kernel_rng_poll();
int kernel_rng_add_entropy(unsigned char *data, int n, int n_bits);
void kernel_rng_close();


class Randpool {
    int fdesc;
public:
    Randpool() {
    }

    int open(string dev) {
        fdesc = ::open(dev.c_str(), O_WRONLY);
        if (fdesc == -1) {
            cout << "Failed to open %s" << dev << endl;
        }
        return fdesc;
    }

    /// @note needs to run with elevated privileges
    int add_entropy(unsigned char *data, int n, int n_bits) {
        struct {
            int entropy_count;
            int buf_size;
            char buf[512];
        } entropy;

        entropy.entropy_count = n_bits;
        entropy.buf_size      = n;
        memcpy(&entropy.buf, data, 512); //n);

        int ret = ::ioctl(fdesc, RNDADDENTROPY, &entropy);
        return ret; // -1 = failed
    }

    void close() {
        ::close(fdesc);
    }
};

#endif // __ENTROPY_H__
