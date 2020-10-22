#include "subnet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#if defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS)
char* strsep(char** stringp, char* sep)
{
    char* p, * s;
    if (NULL == stringp || NULL == *stringp || **stringp == '\0')
        return (NULL);

    s = *stringp;
    p = s + strcspn(s, sep);
    if ('\0' != *p) *p++ = '\0';
    *stringp = p;
    return s;
}
#endif


/******************************************************************************
 * Error checking and management                                              *
 ******************************************************************************/
bool is_valid_ip(const byte_t b0, const byte_t b1, const byte_t b2, 
    const byte_t b3, const byte_t cidr)
{
    /* checking CIDR validity */
    if (UNDEF_CIDR != cidr && INVAL_CIDR == cidr) return false;

    /* constraints check */
    const bool constraints = ((b0 >= MIN_BLOCK && b0 <= MAX_BLOCK) &&
                        (b1 >= MIN_BLOCK && b1 <= MAX_BLOCK) &&
                        (b2 >= MIN_BLOCK && b2 <= MAX_BLOCK) &&
                        (b3 >= MIN_BLOCK && b3 <= MAX_BLOCK));

    /* CIDR constraints check if defined */
    if (UNDEF_CIDR != cidr)
        return ((cidr >= MIN_CIDR && cidr <= MAX_CIDR) && constraints);
    else
        return constraints;
}

bool is_valid_struct(ipaddr_t ip)
{
    return is_valid_ip(ip.b0, ip.b1, ip.b2, ip.b3, ip.cidr);
}

bool is_valid_subnet_mask(const subnet_mask_t subnet_mask)
{
    return ((subnet_mask.b0 >= MIN_BLOCK && subnet_mask.b0 <= MAX_BLOCK) &&
			(subnet_mask.b1 >= MIN_BLOCK && subnet_mask.b1 <= MAX_BLOCK) &&
			(subnet_mask.b2 >= MIN_BLOCK && subnet_mask.b2 <= MAX_BLOCK) &&
			(subnet_mask.b3 >= MIN_BLOCK && subnet_mask.b3 <= MAX_BLOCK));
}

void invalid_fail()
{
    fprintf(stderr, "%s\n", "Error: invalid IP address or CIDR value");
    exit(EXIT_FAILURE);
}

void null_fail() {
    fprintf(stderr, "%s\n", "Error: null pointer passed as argument");
    exit(EXIT_FAILURE);
}

unsigned int count_set_bits(byte_t b0, byte_t b1, byte_t b2, byte_t b3)
{
    unsigned bit_count = 0;

    /* counting each bit set */
    while (b0) {
        b0 &= (unsigned) (b0 - 1);
        bit_count++;
    }

    while (b1) {
        b1 &= (unsigned) (b1 - 1);
        bit_count++;
    }

    while (b2) {
        b2 &= (unsigned) (b2 - 1);
        bit_count++;
    }

    while (b3) {
        b3 &= (unsigned) (b3 - 1);
        bit_count++;
    }

    return bit_count;
}

subnet_mask_t cidr_to_dotdecimal(const ipaddr_t ip_address)
{
    if (ip_address.cidr == 0)
        return (subnet_mask_t) {
                .b0 = 0,
                .b1 = 0,
                .b2 = 0,
                .b3 = 0,
        };

    /* total bit count is CIDR */
    const int bit_count = ip_address.cidr;

    /* creating new net mask and init. to 0 */
    subnet_mask_t netmask = {
            0,
            0,
            0,
            0,
    };

    unsigned int bits = 0;
    unsigned int i;
    for (i = MAX_CIDR - bit_count; i < MAX_CIDR; i++)
        bits |= (1U << i);

    netmask.b0 = (bits & (unsigned) 0xff000000) >> (unsigned) 24;
    netmask.b1 = (bits & (unsigned) 0xff0000) >> (unsigned) 16;
    netmask.b2 = (bits & (unsigned) 0xff00) >> (unsigned) 8;
    netmask.b3 = (bits & (unsigned) 0xff);

    return netmask;
}

/******************************************************************************
 * Utility functions for conversion and initialization of data                *
 ******************************************************************************/

ipaddr_t block_create_ip(const byte_t b0, const byte_t b1, const byte_t b2,
    const byte_t b3, const byte_t cidr)
{
    /* ip validity check */
    if (!is_valid_ip(b0, b1, b2, b3, cidr)) invalid_fail();

    /* generating new IP */
    return (ipaddr_t) { b0, b1, b2, b3, cidr };
}

ipaddr_t string_create_ip(const char ip_string[])
{
    /* separators */
    char* sep = "./";

    char* ip_str = (char*)ip_string;
    /* string null check */
    if (NULL == ip_string) null_fail();

    /* temp storage vars */
    unsigned short b0 = INIT, b1 = INIT, b2 = INIT, b3 = INIT, cidr = INIT;
    /* trying conversion */
    char* blocks[5];
    int i = 0;
    char *tok = NULL;

    /* manually parsing string, sscanf will fail parsing uints */
    while (i < 5) {
        tok = strsep(&ip_str, sep);
        blocks[i] = _strdup(tok);
        i++;
    }

    /* converting with strtoul */
    if ((b0 = strtoul(blocks[0], NULL, 10)) == 0 && errno == EINVAL)
        invalid_fail();
    if ((b1 = strtoul(blocks[1], NULL, 10)) == 0 && errno == EINVAL)
        invalid_fail();
    if ((b2 = strtoul(blocks[2], NULL, 10)) == 0 && errno == EINVAL)
        invalid_fail();
    if ((b3 = strtoul(blocks[3], NULL, 10)) == 0 && errno == EINVAL)
        invalid_fail();
    if (NULL != blocks[4]) {
        if ((cidr = strtoul(blocks[4], NULL, 10)) == 0 && errno == EINVAL)
            invalid_fail();
    }
    else
        cidr = UNDEF_CIDR;

    return (ipaddr_t) {b0, b1, b2, b3, cidr};
}

ipaddr_t ip_calculate_subnet(const ipaddr_t ip_address)
{
    /* invalid IP struct */
    if (!is_valid_struct(ip_address)) invalid_fail();

    /* performing calculations */
    const subnet_mask_t netmask = cidr_to_dotdecimal(ip_address);

    return (ipaddr_t) {
    	ip_address.b0& netmask.b0,
    	ip_address.b1& netmask.b1,
    	ip_address.b2& netmask.b2,
    	ip_address.b3& netmask.b3,
    	ip_address.cidr
    };
}

void ipaddr_to_string(const ipaddr_t ip_address, char ip_string[])
{
    /* string null check */
    if (NULL == ip_string) null_fail();
    /* ip validity check */
    if (!is_valid_struct(ip_address)) invalid_fail();

    /* formatted print on string */
    if (ip_address.cidr != UNDEF_CIDR) {
        sprintf(ip_string, "%hu.%hu.%hu.%hu/%hu",
                ip_address.b0,
                ip_address.b1,
                ip_address.b2,
                ip_address.b3,
                ip_address.cidr);
    } else {
        sprintf(ip_string, "%hu.%hu.%hu.%hu",
                ip_address.b0,
                ip_address.b1,
                ip_address.b2,
                ip_address.b3);
    }
}

void subnet_mask_to_string(const subnet_mask_t subnet_mask, char netmask_str[])
{
	/* string null check */
    if (NULL == netmask_str) null_fail();

	/* netmask validity check */
    if (!is_valid_subnet_mask(subnet_mask)) invalid_fail();

	/* string formatting */
    sprintf(netmask_str, "%hu.%hu.%hu.%hu",
			subnet_mask.b0,
			subnet_mask.b1,
			subnet_mask.b2,
			subnet_mask.b3);
}

byte_t dotdecimal_to_cidr(const subnet_mask_t subnet_mask)
{
	/* validity check */
    if (!is_valid_subnet_mask(subnet_mask)) invalid_fail();

	/* counting bits to establish CIDR value */
    return count_set_bits(
        subnet_mask.b0,
        subnet_mask.b1,
        subnet_mask.b2,
        subnet_mask.b3
    );
}
