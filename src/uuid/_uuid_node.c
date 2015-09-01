/*
 * The MIT License (MIT)
 *
 * Copyright 2015 Sony Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>
#include <sys/socket.h>
#include <net/if.h>
#include <assert.h>

#if __APPLE__
# include <ifaddrs.h>
# include <net/if_dl.h>
# include <net/if_types.h>
#else // linux, unix and posix
# include <unistd.h>
# include <sys/ioctl.h>
# include <netinet/in.h>
# define MAX_IFS (20)
#endif

#include "_uuid_node.h"

int _uuid_get_node(uuid_node_t node) {
    int r = -1;

    memset(node, 0, sizeof(uuid_node_t));

#if __APPLE__
    struct ifaddrs *ifa_list, *ifa;
    struct sockaddr_dl *dl;

    if (getifaddrs(&ifa_list) < 0) {
        return r;
    }
    for (ifa = ifa_list; ifa; ifa = ifa->ifa_next) {
        dl = (struct sockaddr_dl*)ifa->ifa_addr;
        if (dl->sdl_family == AF_LINK && dl->sdl_type == IFT_ETHER) {
            memcpy(node, LLADDR(dl), sizeof(uuid_node_t));
            r = 0;
            break;
        }
    }
    freeifaddrs(ifa_list);
#else
    struct ifreq *ifr, *ifend;
    struct ifreq ifreq;
    struct ifconf ifc;
    struct ifreq ifs[MAX_IFS];
    int fd;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return r;
    }
    ifc.ifc_len = sizeof(ifs);
    ifc.ifc_req = ifs;
    if (ioctl(fd, SIOCGIFCONF, &ifc) < 0) {
        close(fd);
        return r;
    }
    ifend = ifs + (ifc.ifc_len / sizeof(struct ifreq));
    for (ifr = ifc.ifc_req; ifr < ifend; ifr++) {
        if (ifr->ifr_addr.sa_family == AF_INET &&
            strncasecmp(ifr->ifr_name, "lo", 2) != 0) {
            strncpy(ifreq.ifr_name, ifr->ifr_name, sizeof(ifreq.ifr_name));
            if (ioctl(fd, SIOCGIFHWADDR, &ifreq) < 0) {
                close(fd);
                return r;
            }
            memcpy(node, ifreq.ifr_hwaddr.sa_data, sizeof(uuid_node_t));
            r = 0;
            break;
        }
    }
    close(fd);
#endif

    return r;
}
