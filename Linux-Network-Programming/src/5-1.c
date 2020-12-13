#include <stdio.h>

char *byteorder()
{
    union // union, occupy mem of the largest member; all vals share it
    {
        short value;
        char union_bytes[sizeof(short)];
    } test;
    test.value = 0x0102;
    /*
    sizeof(short) = 2bytes,
    sizeof(char) = 1byte
    That means to split `value` to two parts, 0x01 and 0x02
    - for big endian, bigger 0x01 would be in low addr
    - for little endian, bigger 0x01 would be in high addr

    */
    if ((test.union_bytes[0] == 1) && (test.union_bytes[1] == 2))
    {
        return "big endian";
    }
    else if ((test.union_bytes[0] == 2) && (test.union_bytes[1] == 1))
    {
        return "little endian";
    }
    else
    {
        return "unknown";
    }
}

void main()
{
    printf("%s\n", byteorder());
}