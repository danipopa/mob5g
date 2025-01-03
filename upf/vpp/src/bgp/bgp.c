/*
 * bgp.c - skeleton vpp engine plug-in
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

#include <vnet/vnet.h>
#include <vnet/plugin/plugin.h>
#include <bgp/bgp.h>

#include <vlibapi/api.h>
#include <vlibmemory/api.h>
#include <vpp/app/version.h>
#include <stdbool.h>

#include <bgp/bgp.api_enum.h>
#include <bgp/bgp.api_types.h>

#define REPLY_MSG_ID_BASE bmp->msg_id_base
#include <vlibapi/api_helper_macros.h>

bgp_main_t bgp_main;

/* Action function shared between message handler and debug CLI */

int bgp_enable_disable (bgp_main_t * bmp, u32 sw_if_index,
                                   int enable_disable)
{
  vnet_sw_interface_t * sw;
  int rv = 0;

  /* Utterly wrong? */
  if (pool_is_free_index (bmp->vnet_main->interface_main.sw_interfaces,
                          sw_if_index))
    return VNET_API_ERROR_INVALID_SW_IF_INDEX;

  /* Not a physical port? */
  sw = vnet_get_sw_interface (bmp->vnet_main, sw_if_index);
  if (sw->type != VNET_SW_INTERFACE_TYPE_HARDWARE)
    return VNET_API_ERROR_INVALID_SW_IF_INDEX;

  bgp_create_periodic_process (bmp);

  vnet_feature_enable_disable ("device-input", "bgp",
                               sw_if_index, enable_disable, 0, 0);

  /* Send an event to enable/disable the periodic scanner process */
  vlib_process_signal_event (bmp->vlib_main,
                             bmp->periodic_node_index,
                             BGP_EVENT_PERIODIC_ENABLE_DISABLE,
                            (uword)enable_disable);
  return rv;
}

static clib_error_t *
bgp_enable_disable_command_fn (vlib_main_t * vm,
                                   unformat_input_t * input,
                                   vlib_cli_command_t * cmd)
{
  bgp_main_t * bmp = &bgp_main;
  u32 sw_if_index = ~0;
  int enable_disable = 1;

  int rv;

  while (unformat_check_input (input) != UNFORMAT_END_OF_INPUT)
    {
      if (unformat (input, "disable"))
        enable_disable = 0;
      else if (unformat (input, "%U", unformat_vnet_sw_interface,
                         bmp->vnet_main, &sw_if_index))
        ;
      else
        break;
  }

  if (sw_if_index == ~0)
    return clib_error_return (0, "Please specify an interface...");

  rv = bgp_enable_disable (bmp, sw_if_index, enable_disable);

  switch(rv)
    {
  case 0:
    break;

  case VNET_API_ERROR_INVALID_SW_IF_INDEX:
    return clib_error_return
      (0, "Invalid interface, only works on physical ports");
    break;

  case VNET_API_ERROR_UNIMPLEMENTED:
    return clib_error_return (0, "Device driver doesn't support redirection");
    break;

  default:
    return clib_error_return (0, "bgp_enable_disable returned %d",
                              rv);
    }
  return 0;
}

/* *INDENT-OFF* */
VLIB_CLI_COMMAND (bgp_enable_disable_command, static) =
{
  .path = "bgp enable-disable",
  .short_help =
  "bgp enable-disable <interface-name> [disable]",
  .function = bgp_enable_disable_command_fn,
};
/* *INDENT-ON* */

/* API message handler */
static void vl_api_bgp_enable_disable_t_handler
(vl_api_bgp_enable_disable_t * mp)
{
  vl_api_bgp_enable_disable_reply_t * rmp;
  bgp_main_t * bmp = &bgp_main;
  int rv;

  rv = bgp_enable_disable (bmp, ntohl(mp->sw_if_index),
                                      (int) (mp->enable_disable));

  REPLY_MACRO(VL_API_BGP_ENABLE_DISABLE_REPLY);
}

/* API definitions */
#include <bgp/bgp.api.c>

static clib_error_t * bgp_init (vlib_main_t * vm)
{
  bgp_main_t * bmp = &bgp_main;
  clib_error_t * error = 0;

  bmp->vlib_main = vm;
  bmp->vnet_main = vnet_get_main();

  /* Add our API messages to the global name_crc hash table */
  bmp->msg_id_base = setup_message_id_table ();

/* Add bgp node as a next node to ethernet-input */
  vlib_node_add_next (vm, vlib_get_node_by_name (vm, (u8 *)"ethernet-input")->index,
                      vlib_get_node_by_name (vm, (u8 *)"bgp")->index);


  return error;
}

VLIB_INIT_FUNCTION (bgp_init);

/* *INDENT-OFF* */
VNET_FEATURE_INIT (bgp, static) =
{
  .arc_name = "device-input",
  .node_name = "bgp",
  .runs_before = VNET_FEATURES ("ethernet-input"),
};
/* *INDENT-ON */

/* *INDENT-OFF* */
VLIB_PLUGIN_REGISTER () =
{
  .version = VPP_BUILD_VER,
  .description = "bgp plugin description goes here",
};
/* *INDENT-ON* */

/*
 * fd.io coding-style-patch-verification: ON
 *
 * Local Variables:
 * eval: (c-set-style "gnu")
 * End:
 */
