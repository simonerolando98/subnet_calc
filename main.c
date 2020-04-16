#include <stdio.h>
#include "subnet.h"

int main() {
    char ip[64];
    printf("Insert string IP address (fmt x.x.x.x/x): ");
    fgets(ip, sizeof(ip), stdin);
    ipaddr_t ip_struct = string_create_ip(ip);
    ipaddr_t netmask = cidr_to_dotdecimal(ip_struct);
    ipaddr_t subnet = ip_calculate_subnet(ip_struct);

    char result[IP_LENGTH];
    ipaddr_to_string(ip_struct, result);
    printf("Input IP Address: %s\n", result);
    ipaddr_to_string(netmask, result);
    printf("Subnet Mask (dot.decimal): %s\n", result);
    ipaddr_to_string(subnet, result);
    printf("Subnet: %s\n", result);

    return 0;
}