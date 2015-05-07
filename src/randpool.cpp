#include "randpool.h"

namespace randpool {
    int getEntropyPoolAvailable() {
        ofFile file;

        file.open("/proc/sys/kernel/random/entropy_avail", ofFile::ReadOnly, false);
        ofBuffer buff = file.readToBuffer();
        int retval = ofToInt( buff.getFirstLine() );
        file.close();

        return retval;
    }

    int getEntropyPoolSize() {
        ofFile file;

        file.open("/proc/sys/kernel/random/poolsize", ofFile::ReadOnly, false);
        ofBuffer buff = file.readToBuffer();
        int retval = ofToInt( buff.getFirstLine() );
        file.close();

        return retval;
    }

}; // ns
