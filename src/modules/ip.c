/*
 *  T50 - Experimental Mixed Packet Injector
 *
 *  Copyright (C) 2010 - 2015 - T50 developers
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <common.h>

/* Defined here 'cause we need them just here.
   And since we are using linux/ip.h header, they are not
   defined there! */
#define IP_MF 0x2000
#define IP_DF 0x4000

struct iphdr *ip_header(void *buffer, 
                        size_t packet_size, 
                        const struct config_options * __restrict__ co)
{
  struct iphdr *ip;

  assert(buffer != NULL);
  assert(co != NULL);

  ip = buffer;
  ip->version  = IPVERSION;
  ip->ihl      = sizeof(struct iphdr) / 4;

  /* FIXME: MAYBE TOS is filled by kernel through the SO_PRIORITY option and this is completly useless. */
  ip->tos      = co->ip.tos;

  ip->frag_off = htons(co->ip.frag_off ? (co->ip.frag_off >> 3) | IP_MF : IP_DF);

  /* FIXME: Is it necessary to fill tot_len when IP_HDRINCL is used? */
  ip->tot_len  = htons(packet_size);

  ip->id       = htons(__RND(co->ip.id));
  ip->ttl      = co->ip.ttl;
  ip->protocol = co->encapsulated ? IPPROTO_GRE : co->ip.protocol;
  ip->saddr    = htonl(INADDR_RND(co->ip.saddr));
  ip->daddr    = co->ip.daddr;    // Already BIG ENDIAN?
  ip->check    = 0;               // NOTE: it will be calculated by the kernel!

  return ip;
}
