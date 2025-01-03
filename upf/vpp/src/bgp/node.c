/*
 * node.c - skeleton vpp engine plug-in dual-loop node skeleton
 *
 * Copyright (c) <current-year> <your-organization>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <vlib/vlib.h>
#include <vnet/vnet.h>
#include <vnet/pg/pg.h>
#include <vppinfra/error.h>
#include <bgp/bgp.h>

typedef struct 
{
  u32 next_index;
  u32 sw_if_index;
  u8 new_src_mac[6];
  u8 new_dst_mac[6];
} bgp_trace_t;

#ifndef CLIB_MARCH_VARIANT
static u8 *
my_format_mac_address (u8 * s, va_list * args)
{
  u8 *a = va_arg (*args, u8 *);
  return format (s, "%02x:%02x:%02x:%02x:%02x:%02x",
		 a[0], a[1], a[2], a[3], a[4], a[5]);
}

/* packet trace format function */
static u8 * format_bgp_trace (u8 * s, va_list * args)
{
  CLIB_UNUSED (vlib_main_t * vm) = va_arg (*args, vlib_main_t *);
  CLIB_UNUSED (vlib_node_t * node) = va_arg (*args, vlib_node_t *);
  bgp_trace_t * t = va_arg (*args, bgp_trace_t *);
  
  s = format (s, "BGP: sw_if_index %d, next index %d\n",
              t->sw_if_index, t->next_index);
  s = format (s, "  new src %U -> new dst %U",
              my_format_mac_address, t->new_src_mac, 
              my_format_mac_address, t->new_dst_mac);
  return s;
}

vlib_node_registration_t bgp_node;

#endif /* CLIB_MARCH_VARIANT */

#define foreach_bgp_error \
_(SWAPPED, "Mac swap packets processed")

typedef enum {
#define _(sym,str) BGP_ERROR_##sym,
  foreach_bgp_error
#undef _
  BGP_N_ERROR,
} bgp_error_t;

#ifndef CLIB_MARCH_VARIANT
static char * bgp_error_strings[] = 
{
#define _(sym,string) string,
  foreach_bgp_error
#undef _
};
#endif /* CLIB_MARCH_VARIANT */

typedef enum 
{
  BGP_NEXT_INTERFACE_OUTPUT,
  BGP_N_NEXT,
} bgp_next_t;

#define foreach_mac_address_offset              \
_(0)                                            \
_(1)                                            \
_(2)                                            \
_(3)                                            \
_(4)                                            \
_(5)


VLIB_NODE_FN (bgp_node) (vlib_main_t * vm,
		  vlib_node_runtime_t * node,
		  vlib_frame_t * frame)
{
  u32 n_left_from, * from, * to_next;
  bgp_next_t next_index;
  u32 pkts_swapped = 0;
  // u32 pkts_logged = 0;

  from = vlib_frame_vector_args (frame);
  n_left_from = frame->n_vectors;
  next_index = node->cached_next_index;

  clib_warning("n_left_from: %u", n_left_from);
  while (n_left_from > 0)
    {
      u32 n_left_to_next;

      vlib_get_next_frame (vm, node, next_index,
			   to_next, n_left_to_next);
      clib_warning("n_left_from > 0");
      clib_warning("%u >= 4 && %u >= 2", n_left_from, n_left_to_next);
      while (n_left_from >= 4 && n_left_to_next >= 2)
      {
          u32 next0 = BGP_NEXT_INTERFACE_OUTPUT;
          u32 next1 = BGP_NEXT_INTERFACE_OUTPUT;
          u32 sw_if_index0, sw_if_index1;
          u8 tmp0[6], tmp1[6];
          ethernet_header_t *en0, *en1;
          u32 bi0, bi1;
	  vlib_buffer_t * b0, * b1;
    // ip4_header_t * ip0, * ip1;
    // icmp46_header_t * icmp0, * icmp1;
    clib_warning("while loop clib_warning n_left_from > 0);");
	  /* Prefetch next iteration. */
	  {
      clib_warning("Prefetch next iteration.");
	    vlib_buffer_t * p2, * p3;
            
	    p2 = vlib_get_buffer (vm, from[2]);
	    p3 = vlib_get_buffer (vm, from[3]);
            
	    vlib_prefetch_buffer_header (p2, LOAD);
	    vlib_prefetch_buffer_header (p3, LOAD);

	    CLIB_PREFETCH (p2->data, CLIB_CACHE_LINE_BYTES, STORE);
	    CLIB_PREFETCH (p3->data, CLIB_CACHE_LINE_BYTES, STORE);
	  }

      /* speculatively enqueue b0 and b1 to the current next frame */
	  to_next[0] = bi0 = from[0];
	  to_next[1] = bi1 = from[1];
	  from += 2;
	  to_next += 2;
	  n_left_from -= 2;
	  n_left_to_next -= 2;

	  b0 = vlib_get_buffer (vm, bi0);
	  b1 = vlib_get_buffer (vm, bi1);

          ASSERT (b0->current_data == 0);
          ASSERT (b1->current_data == 0);


      // //ICMP log packets
      // ip0 = vlib_buffer_get_current (b0);
      // ip1 = vlib_buffer_get_current (b1);

      // if (ip0->protocol == IP_PROTOCOL_ICMP)
      // {
      //   icmp0 = (icmp46_header_t *)((u8 *)ip0 + ip4_header_bytes (ip0));
      //   clib_warning("ICMP packet received: src %U dst %U type %u",
      //                format_ip4_address, &ip0->src_address,
      //                format_ip4_address, &ip0->dst_address,
      //                icmp0->type);
      //   pkts_logged++;

      //   // Check if it's an ICMP echo request
      //   if (icmp0->type == ICMP4_echo_request)
      //   {
      //       // Generate ICMP echo reply
      //       icmp0->type = ICMP4_echo_reply;
      //       ip4_address_t temp_addr = ip0->src_address;
      //       ip0->src_address = ip0->dst_address;
      //       ip0->dst_address = temp_addr;

      //       // Recalculate IP checksum
      //       ip0->checksum = 0;
      //       ip0->checksum = ip4_header_checksum (ip0);

      //       // Send the reply
      //       vlib_buffer_t *reply_buffer = vlib_buffer_copy (vm, b0);
      //       vlib_frame_t *reply_frame = vlib_get_frame_to_node (vm, next_index);
      //       u32 *reply_to_next = vlib_frame_vector_args (reply_frame);
      //       reply_to_next[0] = vlib_get_buffer_index (vm, reply_buffer);
      //       reply_frame->n_vectors = 1;
      //       vlib_put_frame_to_node (vm, next_index, reply_frame);
      //   }
      // }

      // if (ip1->protocol == IP_PROTOCOL_ICMP)
      // {
      //   icmp1 = (icmp46_header_t *)((u8 *)ip1 + ip4_header_bytes (ip1));
      //   clib_warning("ICMP packet received: src %U dst %U type %u",
      //                format_ip4_address, &ip1->src_address,
      //                format_ip4_address, &ip1->dst_address,
      //                icmp1->type);
      //   pkts_logged++;

      //   // Check if it's an ICMP echo request
      //   if (icmp1->type == ICMP4_echo_request)
      //   {
      //       // Generate ICMP echo reply
      //       icmp1->type = ICMP4_echo_reply;
      //       ip4_address_t temp_addr = ip1->src_address;
      //       ip1->src_address = ip1->dst_address;
      //       ip1->dst_address = temp_addr;

      //       // Recalculate IP checksum
      //       ip1->checksum = 0;
      //       ip1->checksum = ip4_header_checksum (ip1);

      //       // Send the reply
      //       vlib_buffer_t *reply_buffer = vlib_buffer_copy (vm, b1);
      //       vlib_frame_t *reply_frame = vlib_get_frame_to_node (vm, next_index);
      //       u32 *reply_to_next = vlib_frame_vector_args (reply_frame);
      //       reply_to_next[0] = vlib_get_buffer_index (vm, reply_buffer);
      //       reply_frame->n_vectors = 1;
      //       vlib_put_frame_to_node (vm, next_index, reply_frame);
      //   }
      // }

    // the other packets
          en0 = vlib_buffer_get_current (b0);
          en1 = vlib_buffer_get_current (b1);

    /* Print packet information */
    clib_warning("New packet received: buffer index %u", bi0);
    clib_warning("New packet received: buffer index %u", bi1);

/* Print Ethernet header */
ethernet_header_t *eth0 = vlib_buffer_get_current(b0);
ethernet_header_t *eth1 = vlib_buffer_get_current(b1);
clib_warning("Packet 0: src %U dst %U type %x",
              format_ethernet_address, eth0->src_address,
              format_ethernet_address, eth0->dst_address,
              clib_net_to_host_u16(eth0->type));
clib_warning("Packet 1: src %U dst %U type %x",
              format_ethernet_address, eth1->src_address,
              format_ethernet_address, eth1->dst_address,
              clib_net_to_host_u16(eth1->type));


/* This is not the fastest way to swap src + dst mac addresses */
#define _(a) tmp0[a] = en0->src_address[a];
          foreach_mac_address_offset;
#undef _
#define _(a) en0->src_address[a] = en0->dst_address[a];
          foreach_mac_address_offset;
#undef _
#define _(a) en0->dst_address[a] = tmp0[a];
          foreach_mac_address_offset;
#undef _

#define _(a) tmp1[a] = en1->src_address[a];
          foreach_mac_address_offset;
#undef _
#define _(a) en1->src_address[a] = en1->dst_address[a];
          foreach_mac_address_offset;
#undef _
#define _(a) en1->dst_address[a] = tmp1[a];
          foreach_mac_address_offset;
#undef _

          sw_if_index0 = vnet_buffer(b0)->sw_if_index[VLIB_RX];
          sw_if_index1 = vnet_buffer(b1)->sw_if_index[VLIB_RX];

          /* Send pkt back out the RX interface */
          vnet_buffer(b0)->sw_if_index[VLIB_TX] = sw_if_index0;
          vnet_buffer(b1)->sw_if_index[VLIB_TX] = sw_if_index1;

          pkts_swapped += 2;

          if (PREDICT_FALSE((node->flags & VLIB_NODE_FLAG_TRACE)))
            {
              if (b0->flags & VLIB_BUFFER_IS_TRACED) 
                {
                    bgp_trace_t *t = 
                      vlib_add_trace (vm, node, b0, sizeof (*t));
                    t->sw_if_index = sw_if_index0;
                    t->next_index = next0;
                    clib_memcpy (t->new_src_mac, en0->src_address,
                                 sizeof (t->new_src_mac));
                    clib_memcpy (t->new_dst_mac, en0->dst_address,
                                 sizeof (t->new_dst_mac));
                  }
                if (b1->flags & VLIB_BUFFER_IS_TRACED) 
                  {
                    bgp_trace_t *t = 
                      vlib_add_trace (vm, node, b1, sizeof (*t));
                    t->sw_if_index = sw_if_index1;
                    t->next_index = next1;
                    clib_memcpy (t->new_src_mac, en1->src_address,
                                 sizeof (t->new_src_mac));
                    clib_memcpy (t->new_dst_mac, en1->dst_address,
                                 sizeof (t->new_dst_mac));
                  }
              }
            
            /* verify speculative enqueues, maybe switch current next frame */
            vlib_validate_buffer_enqueue_x2 (vm, node, next_index,
                                             to_next, n_left_to_next,
                                             bi0, bi1, next0, next1);
        }

      while (n_left_from > 0 && n_left_to_next > 0)
	{
          u32 bi0;
	  vlib_buffer_t * b0;
          u32 next0 = BGP_NEXT_INTERFACE_OUTPUT;
          u32 sw_if_index0;
          u8 tmp0[6];
          ethernet_header_t *en0;

          /* speculatively enqueue b0 to the current next frame */
	  bi0 = from[0];
	  to_next[0] = bi0;
	  from += 1;
	  to_next += 1;
	  n_left_from -= 1;
	  n_left_to_next -= 1;

	  b0 = vlib_get_buffer (vm, bi0);
          /* 
           * Direct from the driver, we should be at offset 0
           * aka at &b0->data[0]
           */
          ASSERT (b0->current_data == 0);
          
          en0 = vlib_buffer_get_current (b0);

          /* This is not the fastest way to swap src + dst mac addresses */
#define _(a) tmp0[a] = en0->src_address[a];
          foreach_mac_address_offset;
#undef _
#define _(a) en0->src_address[a] = en0->dst_address[a];
          foreach_mac_address_offset;
#undef _
#define _(a) en0->dst_address[a] = tmp0[a];
          foreach_mac_address_offset;
#undef _

          sw_if_index0 = vnet_buffer(b0)->sw_if_index[VLIB_RX];

          /* Send pkt back out the RX interface */
          vnet_buffer(b0)->sw_if_index[VLIB_TX] = sw_if_index0;

          if (PREDICT_FALSE((node->flags & VLIB_NODE_FLAG_TRACE) 
                            && (b0->flags & VLIB_BUFFER_IS_TRACED))) {
            bgp_trace_t *t = 
               vlib_add_trace (vm, node, b0, sizeof (*t));
            t->sw_if_index = sw_if_index0;
            t->next_index = next0;
            clib_memcpy (t->new_src_mac, en0->src_address,
                         sizeof (t->new_src_mac));
            clib_memcpy (t->new_dst_mac, en0->dst_address,
                         sizeof (t->new_dst_mac));
            }
            
          pkts_swapped += 1;

          /* verify speculative enqueue, maybe switch current next frame */
	  vlib_validate_buffer_enqueue_x1 (vm, node, next_index,
					   to_next, n_left_to_next,
					   bi0, next0);
	}

      vlib_put_next_frame (vm, node, next_index, n_left_to_next);
    }

  vlib_node_increment_counter (vm, bgp_node.index, 
                               BGP_ERROR_SWAPPED, pkts_swapped);
  return frame->n_vectors;
}

/* *INDENT-OFF* */
#ifndef CLIB_MARCH_VARIANT
VLIB_REGISTER_NODE (bgp_node) = 
{
  .name = "bgp",
  .vector_size = sizeof (u32),
  .format_trace = format_bgp_trace,
  .type = VLIB_NODE_TYPE_INTERNAL,
  
  .n_errors = ARRAY_LEN(bgp_error_strings),
  .error_strings = bgp_error_strings,

  .n_next_nodes = BGP_N_NEXT,

  /* edit / add dispositions here */
  .next_nodes = {
        [BGP_NEXT_INTERFACE_OUTPUT] = "interface-output",
  },
};
#endif /* CLIB_MARCH_VARIANT */
/* *INDENT-ON* */
/*
 * fd.io coding-style-patch-verification: ON
 *
 * Local Variables:
 * eval: (c-set-style "gnu")
 * End:
 */
