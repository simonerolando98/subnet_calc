#ifndef SUBNET_CALC_SUBNET_H
#define SUBNET_CALC_SUBNET_H

/**
 * This file defines an interface and some types to perform basic subnetting
 * calculations using low level C operations to maximize efficiency and code
 * final size.
 */

/**
 * Useful constants for type checking and comparison
 */

/* CIDR standard values */
#define UNDEF_CIDR 0xFA
#define INVAL_CIDR 0xFF

/* min block and max block values */
#define MIN_BLOCK 0x00
#define MAX_BLOCK 0xFF

/* min CIDR and max CIDR values */
#define MIN_CIDR 0x00
#define MAX_CIDR 0x20

/* IP string safe length */
#define IP_LENGTH 0x40

/* default init value */
#define INIT 0x00

/* byte data type definition */
typedef unsigned short byte_t;

/* IP address definition */
typedef struct {
    byte_t b0;
    byte_t b1;
    byte_t b2;
    byte_t b3;
    byte_t cidr;
} ipaddr_t;

/* subnet type alias */
typedef struct {
    byte_t b0;
    byte_t b1;
    byte_t b2;
    byte_t b3;
} subnet_mask_t;

/******************************************************************************
 * Utility functions for conversion and initialization of data                *
 ******************************************************************************/

/**
 * Create a new IP address struct.
 * @param b0 first address byte (unsigned char).
 * @param b1 second address byte (unsigned char).
 * @param b2 third address byte (unsigned char).
 * @param b3 fourth address byte (unsigned char).
 * @param cidr CIDR (Classless Inter-Domain Routing) (unsigned char).
 * @return an ipaddr_t struct which contains the correctly formatted IP.
 */
ipaddr_t block_create_ip(byte_t b0, byte_t b1, byte_t b2, byte_t b3,
        byte_t cidr);

/**
 * Create a new IP address struct from a standard format IP string.
 * @param ip_string an IP string using standard format (ex. 192.168.1.7/24 | 192.168.1.7
 * @return an ipaddr_t struct which contains the correctly formatted IP.
 */
ipaddr_t string_create_ip(const char ip_string[]);

/**
 * Calculate and return a subnet mask based on the provided IP. If no CIDR is
 * provided, calculation will fail with an error.
 * @param ip_address an ipaddr_t struct containing the correctly formed IP address.
 * @return a subnet_mask_t struct containing the calculated subnet mask.
 */
ipaddr_t ip_calculate_subnet(ipaddr_t ip_address);

/**
 * Return a dot.decimal subnet mask calculated via CIDR rules.
 * @param ip_address the ipaddr_t struct containing an IP address with CIDR notation
 * or the CIDR field which must differ from UNDEF_CIDR.
 * @return a subnet_mask_t struct containing the dot.decimal subnet mask.
 */
subnet_mask_t cidr_to_dotdecimal(ipaddr_t ip_address);

/**
 * Convert an ipaddr_t IP Address struct to a printable string (char[]).
 * @param ip_address the ipaddr_t struct containing a valid IP address.
 * @param ip_string the pre-allocated byte char string.
 */
void ipaddr_to_string(ipaddr_t ip_address, char ip_string[]);

/**
 * Convert a subnet_mask_t Subnet Mask struct to a printable string (char[]).
 * @param subnet_mask a subnet_mask_t struct containing the dotdecimal netmask.
 * @param netmask_str pre-allocated byte char string.
 */
void subnet_mask_to_string(subnet_mask_t subnet_mask, char netmask_str[]);

/**
 * Convert a dot decimal notation subnet mask to CIDR value.
 * @param subnet_mask an ipaddr_t struct containing the IP address with CIDR.
 * @return a byte_t containing the correct CIDR value 
 */
byte_t dotdecimal_to_cidr(subnet_mask_t subnet_mask);

#endif
