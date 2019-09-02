#ifndef _CMS_HPP_
#define _CMS_HPP_

#include <cmath>

#include <omp.h>

template<typename T>
struct cms_default
{
    explicit cms_default(double epsilon, double delta)
    {
        w = ceil(2 / epsilon / epsilon);
        d = ceil(log(1 / delta));
        hashes = new hash[d];
    }
private:
    unsigned long const w, d;
    hash const * const hashes;
}

#endif
