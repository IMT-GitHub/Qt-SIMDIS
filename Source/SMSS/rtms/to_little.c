unsigned long 
to_little(unsigned long x)
{
        return ((x << 24) | ((x & 0xff00) << 8) | ((x >> 8) & 0xff00) | (x >> 24));
}

unsigned short
to_little2(unsigned short x)
{
        return ((x << 8) | (x >> 8));
}

