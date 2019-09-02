#ifndef _CMS_HPP_
#define _CMS_HPP_

template<typename T>
struct cms_default
{
    explicit cms_default(double epsilon, double delta)
    {}
private:
    unsigned long const w, d;
}

#endif
