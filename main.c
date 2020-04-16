#include <stdio.h>
#include "subnet.h"

int main() {
    char ip[64];
    printf("Insert string IP address (fmt x.x.x.x/x): ");
    fgets(ip, sizeof(ip), stdin);
    ipaddr_t ip_struct = string_create_ip(ip);
    ipaddr_t subnet = ip_calculate_subnet(ip_struct);

    printf("IP Address: %u.%u.%u.%u/%u\n",
            ip_struct.b0,
            ip_struct.b1,
            ip_struct.b2,
            ip_struct.b3,
            ip_struct.cidr);

    printf("Subnet: %u.%u.%u.%u/%u",
            subnet.b0,
            subnet.b1,
            subnet.b2,
            subnet.b3,
            subnet.cidr);

    return 0;
}