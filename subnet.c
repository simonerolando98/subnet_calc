#include "subnet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

/******************************************************************************
 * Error checking and management                                              *
 ******************************************************************************/
bool is_valid_ip(byte_t b0, byte_t b1, byte_t b2, byte_t b3,
        byte_t cidr)
{
    return ((UNDEF_CIDR != cidr && (cidr < MIN_CIDR || cidr > MAX_CIDR) &&
        INVAL_CIDR != cidr) || b0 < MIN_BLOCK || b0 > MAX_BLOCK ||
        b1 < MIN_BLOCK || b1 > MAX_BLOCK || b2 < MIN_BLOCK ||
        b2 > MAX_BLOCK || b3 < MIN_BLOCK || b3 > MAX_BLOCK);
}

bool is_valid_struct(ipaddr_t ip)
{
    return is_valid_ip(ip.b0, ip.b1, ip.b2, ip.b3, ip.cidr);
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

subnet_mask_t cidr_to_subnet_mask(ipaddr_t ip_address)
{
    if (ip_address.cidr == 0) return (subnet_mask_t) {
        .b0 = 0,
        .b1 = 0,
        .b2 = 0,
        .b3 = 0,
        .cidr = 0
    };

    /* total bit count is CIDR */
    int bit_count = ip_address.cidr;

    /* creating new net mask and init. to 0 */
    subnet_mask_t netmask = {
            0,
            0,
            0,
            0,
            0,
    };

    /* converting struct to byte array */
    byte_t *sub_array = (byte_t*)&netmask;

    int i = 0;
    while (bit_count > -1) {
        sub_array[i] |= (byte_t) abs(bit_count - 8);
        bit_count -= 8;
        i++;
    }

    return netmask;
}

/******************************************************************************
 * Utility functions for conversion and initialization of data                *
 ******************************************************************************/

/**
 * Create a new IP address struct
 * @param b0 first address byte (unsigned char).
 * @param b1 second address byte (unsigned char).
 * @param b2 third address byte (unsigned char).
 * @param b3 fourth address byte (unsigned char).
 * @param cidr CIDR (Classless Inter-Domain Routing) (unsigned char).
 * @return an ipaddr_t struct which contains the correctly formatted IP.
 */
ipaddr_t block_create_ip(byte_t b0, byte_t b1, byte_t b2, byte_t b3,
        byte_t cidr)
{
    /* ip validity check */
    if (!is_valid_ip(b0, b1, b2, b3, cidr)) invalid_fail();

    /* generating new IP */
    ipaddr_t ip = { b0, b1, b2, b3, cidr };
    return ip;
}

/**
 * Create a new IP address struct from a standard format IP string.
 * @param ip_string an IP string using standard format (ex. 192.168.1.7/24 | 192.168.1.7
 * @return an ipaddr_t struct which contains the correctly formatted IP.
 */
ipaddr_t string_create_ip(const char ip_string[])
{
    const char* sep = "./";
    char* ip_str = (char*)ip_string;
    /* string null check */
    if (NULL == ip_string) null_fail();

    /* temp storage vars */
    unsigned short b0, b1, b2, b3, cidr = UNDEF_CIDR;
    /* trying conversion */
    char* blocks[5];
    int i = 0;
    char* tok = ip_str;

    /* manually parsing string, sscanf will fail parsing uints */
    while (i < 5) {
        tok = strsep(&ip_str, sep);
        blocks[i] = strdup(tok);
        i++;
    }

    /* converting with format read */
    int result = 0;
    result += sscanf(blocks[0], "%hu", &b0);
    result += sscanf(blocks[1], "%hu", &b1);
    result += sscanf(blocks[2], "%hu", &b2);
    result += sscanf(blocks[3], "%hu", &b3);
    result += sscanf(blocks[4], "%hu", &cidr);

    /* based on conversion result, take action */
    ipaddr_t ip = {};
    if (result == 5) {
        /* building with CIDR */
        ip.b0 = b0;
        ip.b1 = b1;
        ip.b2 = b2;
        ip.b3 = b3;
        ip.cidr = cidr;
    } else if (result == 4) {
        /* building without CIDR */
        ip.b0 = b0;
        ip.b1 = b1;
        ip.b2 = b2;
        ip.b3 = b3;
        ip.cidr = UNDEF_CIDR;
    } else {
        invalid_fail();
    }

    return ip;
}

/**
 * Calculate and return a subnet mask based on the provided IP. If no CIDR is
 * provided, calculation will fail with an error.
 * @param ip_address an ipaddr_t struct containing the correctly formed IP address.
 * @return a subnet_mask_t struct containing the calculated subnet mask.
 */
ipaddr_t ip_calculate_subnet(const ipaddr_t ip_address)
{
    /* invalid IP struct */
    if (!is_valid_struct(ip_address)); //invalid_fail();

    /* performing calculations */
    subnet_mask_t netmask = cidr_to_subnet_mask(ip_address);

    ipaddr_t subnet = {
            ip_address.b0 & netmask.b0,
            ip_address.b1 & netmask.b1,
            ip_address.b2 & netmask.b2,
            ip_address.b3 & netmask.b3,
            ip_address.cidr
    };

    return subnet;
}


