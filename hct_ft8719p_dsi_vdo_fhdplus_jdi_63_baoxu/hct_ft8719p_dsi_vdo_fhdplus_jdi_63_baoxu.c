
/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
	#include <string.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	//#include <mach/mt_gpio.h>
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(1080)
#define FRAME_HEIGHT 										(2340)

#define LCM_ID_FT8719P                						(0x8719)

#define REGFLAG_DELAY             							0xAB
#define REGFLAG_END_OF_TABLE      							0xAA   // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE									0

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


//static unsigned int lcm_esd_test = FALSE;      ///only for ESD test

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)										lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
#define set_gpio_lcd_enp(cmd)                               lcm_util.set_gpio_lcd_enp_bias(cmd)
#define set_gpio_lcd_enn(cmd)                               lcm_util.set_gpio_lcd_enn_bias(cmd)
#define hct_lcm_power_settings(mode, value, mdelay1, mdelay2) 					lcm_util.hct_lcm_power_settings(mode, value, mdelay1, mdelay2)
#define set_gpio_tp_incell_rst(cmd)                         lcm_util.set_gpio_tp_incell_rst(cmd)

 struct LCM_setting_table {
	unsigned int cmd;
    unsigned char count;
    unsigned char para_list[64];
};

#if defined(BUILD_LK)
#else
extern int fts_write_reg(u8 addr, u8 value);
static char tpgesture_status = 1;
#endif
extern int RT5081_db_pos_neg_setting(void);
extern int RT5081_db_pos_neg_disable(void);
extern int PMU_db_pos_neg_setting_delay(int ms);
extern int PMU_db_pos_neg_setting_delay_hct(int ms, int vol);
extern int PMU_db_pos_neg_disable_delay(int ms);
static void push_table(struct LCM_setting_table *table, unsigned int count,
		unsigned char force_update)
{
	unsigned int i;

	for (i = 0; i < count; i++) {

		unsigned cmd;
		cmd = table[i].cmd;

		switch (cmd) {

		case REGFLAG_DELAY:
			MDELAY(table[i].count);
			break;

		case REGFLAG_END_OF_TABLE:
			break;

		default:
			dsi_set_cmdq_V2(cmd, table[i].count,
					table[i].para_list, force_update);
		}
	}

}
static struct LCM_setting_table lcm_initialization_setting[] = {
{0x00,1,{0x00}},
{0xFF,3,{0x87,0x19,0x01}},
{0x00,1,{0x80}},
{0xFF,2,{0x87,0x19}},
{0x00,1,{0xA1}},//A1,A2=1080  A3,A4=2340
{0xB3,5,{0x04,0x38,0x09,0x24,0xC0}},
{0x00,1,{0xA6}},
{0xB3,1,{0xF8}},
{0x00,1,{0xCA}},
{0xC0,1,{0x80}},
{0x00,1,{0xE8}},
{0xC0,1,{0x40}},
{0x00,1,{0x85}},
{0xA7,1,{0x00}},
{0x00,1,{0xCC}},
{0xC0,1,{0x10}},
{0x00,1,{0x86}},
{0xC0,6,{0x01,0x07,0x01,0x01,0x1D,0x06}},
{0x00,1,{0x96}},
{0xC0,6,{0x01,0x07,0x01,0x01,0x1D,0x06}},
{0x00,1,{0xA6}},
{0xC0,6,{0x01,0x07,0x01,0x01,0x1D,0x06}},
{0x00,1,{0xD0}},
{0xC3,8,{0x45,0x00,0x00,0x05,0x45,0x00,0x00,0x05}},
{0x00,1,{0xE0}},
{0xC3,8,{0x45,0x00,0x00,0x05,0x45,0x00,0x00,0x05}},//TP monitor differ low
{0x00,1,{0x82}},
{0xA7,2,{0x33,0x02}},
{0x00,1,{0x80}},
{0xC2,16,{0x84,0x01,0x3A,0x3A,0x81,0x00,0x01,0x8F,0x82,0x00,0x01,0x8E,0x81,0x00,0x01,0x8F}},
{0x00,1,{0x90}},
{0xC2,16,{0x02,0x01,0x05,0x05,0x01,0x00,0x01,0x81,0x02,0x00,0x01,0x81,0x03,0x00,0x01,0x81}},
{0x00,1,{0xA0}},
{0xC2,15,{0x84,0x04,0x00,0x05,0x85,0x83,0x04,0x00,0x05,0x85,0x82,0x04,0x00,0x05,0x85}},
{0x00,1,{0xB0}},
{0xC2,15,{0x81,0x04,0x00,0x05,0x85,0x04,0x04,0x00,0x01,0x8E,0x05,0x04,0x00,0x01,0x90}},
{0x00,1,{0xE0}},
{0xC2,8,{0x33,0x33,0x43,0x77,0x00,0x00,0x00,0x00}},
{0x00,1,{0xE8}},
{0xC2,6,{0x12,0x00,0x05,0x02,0x05,0x05}},
{0x00,1,{0x80}},
{0xCB,16,{0xC1,0xC1,0x00,0xC1,0xC1,0x00,0x00,0xC1,0xFE,0x00,0xC1,0x00,0xFD,0xC1,0x00,0xC0}},
{0x00,1,{0x90}},
{0xCB,16,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{0x00,1,{0xA0}},
{0xCB,4,{0x00,0x00,0x00,0x00}},
{0x00,1,{0xB0}},
{0xCB,4,{0x55,0x55,0x95,0x55}},
{0x00,1,{0x80}},
{0xCC,16,{0x00,0x00,0x00,0x00,0x25,0x29,0x22,0x24,0x24,0x29,0x29,0x01,0x12,0x02,0x08,0x08}},
{0x00,1,{0x90}},
{0xCC,8,{0x06,0x06,0x18,0x18,0x17,0x17,0x16,0x16}},
{0x00,1,{0x80}},
{0xCD,16,{0x00,0x00,0x00,0x00,0x25,0x29,0x22,0x24,0x24,0x29,0x29,0x01,0x12,0x02,0x09,0x09}},
{0x00,1,{0x90}},
{0xCD,8,{0x07,0x07,0x18,0x18,0x17,0x17,0x16,0x16}},
{0x00,1,{0xA0}},
{0xCC,16,{0x00,0x00,0x00,0x00,0x25,0x29,0x24,0x23,0x24,0x29,0x29,0x01,0x02,0x12,0x07,0x07}},
{0x00,1,{0xB0}},
{0xCC,8,{0x09,0x09,0x18,0x18,0x17,0x17,0x16,0x16}},
{0x00,1,{0xA0}},
{0xCD,16,{0x00,0x00,0x00,0x00,0x25,0x29,0x24,0x23,0x24,0x29,0x29,0x01,0x02,0x12,0x06,0x06}},
{0x00,1,{0xB0}},
{0xCD,8,{0x08,0x08,0x18,0x18,0x17,0x17,0x16,0x16}},
{0x00,1,{0x80}},
{0xC0,6,{0x00,0x7A,0x00,0x6C,0x00,0x10}},
{0x00,1,{0xA0}},
{0xC0,6,{0x01,0x09,0x00,0x3A,0x00,0x10}},
{0x00,1,{0xB0}},
{0xC0,5,{0x00,0x7A,0x02,0x11,0x10}},
{0x00,1,{0xC1}},
{0xC0,8,{0x00,0xB2,0x00,0x8C,0x00,0x77,0x00,0xD2}},
{0x00,1,{0xD7}},
{0xC0,6,{0x00,0x77,0x00,0x6E,0x00,0x10}},
{0x00,1,{0xA5}},
{0xC1,4,{0x00,0x2A,0x00,0x02}},
{0x00,1,{0x82}},
{0xCE,13,{0x01,0x09,0x00,0xD8,0x00,0xD8,0x00,0x90,0x00,0x90,0x0D,0x0E,0x09}},
{0x00,1,{0x90}},
{0xCE,8,{0x00,0x82,0x0D,0x5C,0x00,0x82,0x80,0x09}},
{0x00,1,{0xA0}},// 
{0xCE,3,{0x10,0x00,0x00}},
{0x00,1,{0xB0}},//rescan 2lane（与TCON default有差异）
{0xCE,3,{0x22,0x00,0x00}},
{0x00,1,{0xD1}},
{0xCE,7,{0x00,0x0A,0x01,0x01,0x00,0x5D,0x01}},
{0x00,1,{0xE1}},
{0xCE,11,{0x08,0x02,0x15,0x02,0x15,0x02,0x15,0x00,0x2B,0x00,0x5F}},
{0x00,1,{0xF1}},
{0xCE,9,{0x16,0x0B,0x0F,0x01,0x00,0x00,0xFE,0x01,0x0A}},
{0x00,1,{0xB0}},
{0xCF,4,{0x00,0x00,0x6C,0x70}},
{0x00,1,{0xB5}},
{0xCF,4,{0x04,0x04,0xA4,0xA8}},
{0x00,1,{0xC0}},
{0xCF,4,{0x08,0x08,0xCA,0xCE}},
{0x00,1,{0xC5}},
{0xCF,4,{0x00,0x00,0x08,0x0C}},
{0x00,1,{0x90}},
{0xC0,6,{0x00,0x7A,0x00,0x6C,0x00,0x10}},
{0x00,1,{0x80}},
{0xCE,2,{0x01,0x80}},
{0x00,1,{0x98}},
{0xCE,2,{0x00,0x04}},
{0x00,1,{0xC0}},
{0xCE,3,{0x00,0x00,0x00}},
{0x00,1,{0xD0}},
{0xCE,1,{0x91}},
{0x00,1,{0xE0}},
{0xCE,1,{0x88}},
{0x00,1,{0xF0}},
{0xCE,1,{0x80}},
{0x00,1,{0x82}},
{0xCF,1,{0x06}},
{0x00,1,{0x84}},
{0xCF,1,{0x06}},
{0x00,1,{0x87}},
{0xCF,1,{0x06}},
{0x00,1,{0x89}},
{0xCF,1,{0x06}},
{0x00,1,{0x8A}},
{0xCF,1,{0x07}},
{0x00,1,{0x8B}},
{0xCF,1,{0x00}},
{0x00,1,{0x8C}},
{0xCF,1,{0x06}},
{0x00,1,{0x92}},
{0xCF,1,{0x06}},
{0x00,1,{0x94}},
{0xCF,1,{0x06}},
{0x00,1,{0x97}},
{0xCF,1,{0x06}},
{0x00,1,{0x99}},
{0xCF,1,{0x06}},
{0x00,1,{0x9A}},
{0xCF,1,{0x07}},
{0x00,1,{0x9B}},
{0xCF,1,{0x00}},
{0x00,1,{0x9C}},
{0xCF,1,{0x06}},
{0x00,1,{0xA0}},
{0xCF,1,{0x24}},
{0x00,1,{0xA2}},
{0xCF,1,{0x06}},
{0x00,1,{0xA4}},
{0xCF,1,{0x06}},
{0x00,1,{0xA7}},
{0xCF,1,{0x06}},
{0x00,1,{0xA9}},
{0xCF,1,{0x06}},
{0x00,1,{0xB4}},
{0xCF,1,{0x00}},
{0x00,1,{0xC4}},
{0xCF,1,{0x00}},
{0x00,1,{0x82}},
{0xC5,2,{0x50,0x50}},
{0x00,1,{0x84}},
{0xC5,2,{0x32,0x32}},
{0x00,1,{0x00}},
{0xE1,40,{0x06,0x06,0x09,0x0f,0x39,0x18,0x20,0x26,0x30,0xc8,0x38,0x3f,0x45,0x4a,0x05,0x4f,0x57,0x5f,0x66,0x20,0x6d,0x74,0x7c,0x84,0x63,0x8e,0x94,0x9b,0xa2,0x91,0xab,0xb6,0xc5,0xce,0xb6,0xda,0xec,0xf7,0xff,0xcf}},
{0x00,1,{0x00}},
{0xE2,40,{0x06,0x06,0x09,0x0f,0x39,0x18,0x20,0x26,0x30,0xc8,0x38,0x3f,0x45,0x4a,0x05,0x4f,0x57,0x5f,0x66,0x20,0x6d,0x74,0x7c,0x84,0x63,0x8e,0x94,0x9b,0xa2,0x91,0xab,0xb6,0xc5,0xce,0xb6,0xda,0xec,0xf7,0xff,0xcf}},
{0x00,1,{0x00}},
{0xE3,40,{0x06,0x06,0x09,0x0f,0x39,0x18,0x20,0x26,0x30,0xc8,0x38,0x3f,0x45,0x4a,0x05,0x4f,0x57,0x5f,0x66,0x20,0x6d,0x74,0x7c,0x84,0x63,0x8e,0x94,0x9b,0xa2,0x91,0xab,0xb6,0xc5,0xce,0xb6,0xda,0xec,0xf7,0xff,0xcf}},
{0x00,1,{0x00}},
{0xE4,40,{0x06,0x06,0x09,0x0f,0x39,0x18,0x20,0x26,0x30,0xc8,0x38,0x3f,0x45,0x4a,0x05,0x4f,0x57,0x5f,0x66,0x20,0x6d,0x74,0x7c,0x84,0x63,0x8e,0x94,0x9b,0xa2,0x91,0xab,0xb6,0xc5,0xce,0xb6,0xda,0xec,0xf7,0xff,0xcf}},
{0x00,1,{0x00}},
{0xE5,40,{0x06,0x06,0x09,0x0f,0x39,0x18,0x20,0x26,0x30,0xc8,0x38,0x3f,0x45,0x4a,0x05,0x4f,0x57,0x5f,0x66,0x20,0x6d,0x74,0x7c,0x84,0x63,0x8e,0x94,0x9b,0xa2,0x91,0xab,0xb6,0xc5,0xce,0xb6,0xda,0xec,0xf7,0xff,0xcf}},
{0x00,1,{0x00}},
{0xE6,40,{0x06,0x06,0x09,0x0f,0x39,0x18,0x20,0x26,0x30,0xc8,0x38,0x3f,0x45,0x4a,0x05,0x4f,0x57,0x5f,0x66,0x20,0x6d,0x74,0x7c,0x84,0x63,0x8e,0x94,0x9b,0xa2,0x91,0xab,0xb6,0xc5,0xce,0xb6,0xda,0xec,0xf7,0xff,0xcf}},
{0x00,1,{0x00}},
{0xD8,2,{0x2B,0x2B}},
{0x00,1,{0x00}},
{0xD9,3,{0x00,0x8C,0x8C}},
{0x00,1,{0xA3}},
{0xC5,1,{0x1E}},
{0x00,1,{0xA9}},
{0xC5,1,{0x23}},
{0x00,1,{0x85}},
{0xC4,1,{0x1E}},
{0x00,1,{0x8C}},
{0xC3,3,{0x03,0x00,0x30}},
{0x00,1,{0x86}},
{0xC5,3,{0x00,0x60,0x0C}},
{0x00,1,{0x83}},
{0xA4,1,{0x23}},
{0x00,1,{0xB0}},
{0xF5,1,{0x00}},
{0x00,1,{0xC1}},
{0xB6,1,{0x09,0x89,0x68}},
{0x00,1,{0x89}},
{0xF5,1,{0x5A}},//VGH power off drop
{0x00,1,{0x96}},
{0xF5,1,{0x5A}},//VGH power off drop
{0x00,1,{0x80}},
{0xA7,1,{0x03}},//RGB-BGR
{0x00,1,{0xB0}},
{0xF3,2,{0x01,0xFE}},
{0x00,1,{0x00}},
{0xFF,3,{0xFF,0xFF,0xFF}},
{0x35, 1,{0x00}}, 
{0x11, 1,{0x00}},
{REGFLAG_DELAY, 120, {0}},
{0x29, 1,{0x00}},
{REGFLAG_DELAY, 20, {0}},
{REGFLAG_END_OF_TABLE, 0x00, {}}
};



/*
static struct LCM_setting_table lcm_sleep_out_setting[] = {
	// Sleep Out
	{0x11, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},

	// Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 10, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
*/

static struct LCM_setting_table lcm_sleep_in_setting[] = {
	// Display off sequence
	
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 60, {}},

	// Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 60, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_deep_sleep_in_setting[] = {

	//deepsleep
 	{0x00,1,{0x00}},
	{0xFF,3,{0x87,0x19,0x01}},
	{0x00,1,{0x80}},
	{0xFF,2,{0x87,0x19}},
	{0x00,1,{0x00}},
 	{0xF7,4,{0x5A,0xA5,0x95,0x27}},
 	{REGFLAG_DELAY, 60, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS * util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS * params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type = LCM_TYPE_DSI;

	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	// enable tearing-free
	params->dbi.te_mode = LCM_DBI_TE_MODE_DISABLED;
	params->dbi.te_edge_polarity = LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode = CMD_MODE;
#else
	params->dsi.mode   = SYNC_PULSE_VDO_MODE;//SYNC_EVENT_VDO_MODE;//BURST_VDO_MODE;////
#endif

	// DSI
	/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_FOUR_LANE;
	
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
	
	
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	
#if (LCM_DSI_CMD_MODE)
	params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
	params->dsi.word_count=FRAME_WIDTH*3;	//DSI CMD mode need set these two bellow params, different to 6577
#else
	params->dsi.intermediat_buffer_num = 0;	//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
#endif

  // Video mode setting

    params->dsi.packet_size=256;

    params->physical_width = 70;
    params->physical_height = 138;
 
    params->dsi.vertical_sync_active                =4;//2已修改

    params->dsi.vertical_backporch                  =12 ;//16 25 30 35 12 8 8已修改	

    params->dsi.vertical_frontporch                 =112;//已修改

    params->dsi.vertical_active_line                = FRAME_HEIGHT; 

 

    params->dsi.horizontal_sync_active              =4 ;//56 30已修改

    params->dsi.horizontal_backporch                =16; //104 85已修改

    params->dsi.horizontal_frontporch               =16 ;//20 20已修改

    params->dsi.horizontal_active_pixel             = FRAME_WIDTH;

 
    params->physical_width = 72;
    params->physical_height = 143;
 

    params->dsi.PLL_CLOCK=560;//已修改

	params->dsi.cont_clock = 1;
	params->dsi.clk_lp_per_line_enable = 0;//
	params->dsi.ssc_disable = 1;	
	
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
    params->dsi.lcm_esd_check_table[0].cmd          = 0x0a;
    params->dsi.lcm_esd_check_table[0].count        = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;
	
		                                         /////add esd check regist by focal///
	params->dsi.lcm_esd_check_table[1].cmd          = 0x0D;
	params->dsi.lcm_esd_check_table[1].count        = 1;
	params->dsi.lcm_esd_check_table[1].para_list[0] = 0x00;
	
	params->dsi.lcm_esd_check_table[2].cmd          = 0xAC;
	params->dsi.lcm_esd_check_table[2].count        = 1;
	params->dsi.lcm_esd_check_table[2].para_list[0] = 0x20;


}

static void lcm_init(void)
{
	set_gpio_lcd_enn(1);  //1.8
	MDELAY(10);
	PMU_db_pos_neg_setting_delay_hct(10, 58);
	MDELAY(5);

	SET_RESET_PIN(1);
	MDELAY(5);
	SET_RESET_PIN(0);
	MDELAY(5);
	SET_RESET_PIN(1);
	MDELAY(120);

	push_table(lcm_initialization_setting,sizeof(lcm_initialization_setting) /sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
	//unsigned int array[5];
 	//array[0] = 0x00011500;// read id return two byte,version and id
	//dsi_set_cmdq(array, 1, 1);
	//MDELAY(100);
	/*array[0] = 0x01FE1500;
	dsi_set_cmdq(array, 1, 1);
	MDELAY(5);
	array[0] = 0x00461500;
	dsi_set_cmdq(array, 1, 1);
	MDELAY(5);*/

#ifndef BUILD_LK
	int ret;
	if(1 == tpgesture_status){
	    printk("tpgesture_status is open_! !!!\n");
    	}else{
		/* TP enter sleep mode */
	    ret = fts_write_reg(0xA5, 0x03);
	    if (ret < 0)
	        printk("set TP to sleep mode fail, ret=%d", ret);
	}

	push_table(lcm_sleep_in_setting,sizeof(lcm_sleep_in_setting) /sizeof(struct LCM_setting_table), 1);
	
	if(0 == tpgesture_status) {
		push_table(lcm_deep_sleep_in_setting,sizeof(lcm_deep_sleep_in_setting) /sizeof(struct LCM_setting_table), 1);
	}

	if(1 == tpgesture_status){
	    printk("tpgesture_status is open_2 !!!\n");
    	}else{
		SET_RESET_PIN(0);
		MDELAY(10);
		PMU_db_pos_neg_disable_delay(10);
		MDELAY(10);
		//set_gpio_lcd_enn(0);
	    	//MDELAY(10);
	}
#endif
}

static unsigned int lcm_compare_id(void);
static void lcm_resume(void)
{
#ifndef BUILD_LK
	if(1 == tpgesture_status) {
		SET_RESET_PIN(1);
		MDELAY(10);
		SET_RESET_PIN(0);
		MDELAY(10);
		SET_RESET_PIN(1);
		MDELAY(100);
		push_table(lcm_initialization_setting,sizeof(lcm_initialization_setting) /sizeof(struct LCM_setting_table), 1);
	} else {
		lcm_init();
	}
#endif
}
extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);
#define AUX_IN0_LCD_ID  2
#define ADC_MIN_VALUE   0xA00

static unsigned int lcm_compare_id(void)
{

	int array[4];
     char buffer[5];
     char id_high=0;
     char id_low=0;
     int id=0;
	int adcdata[4] = {0};
    int rawdata = 0;
    int ret = 0;

	set_gpio_lcd_enn(1);  //1.8
    MDELAY(10);
	PMU_db_pos_neg_setting_delay_hct(10, 58);
    MDELAY(5);
 
	SET_RESET_PIN(1);
	MDELAY(5);
	SET_RESET_PIN(0);
    MDELAY(5);
	SET_RESET_PIN(1);
	MDELAY(120);
	ret = IMM_GetOneChannelValue(AUX_IN0_LCD_ID, adcdata, &rawdata);
	if(rawdata<ADC_MIN_VALUE)
	{ 

     array[0]=0x00043700;
     dsi_set_cmdq(array, 1, 1);
 
     read_reg_v2(0xA1, buffer,4);
     id_high = buffer[2]; ///////////////////////0x87


     id_low = buffer[3]; ////////////////////////0x19
	 
     id =(id_high << 8) | id_low;
 
     #if defined(BUILD_LK)
     printf("FT8719P compare-LK:0x%02x,0x%02x,0x%02x\n", id_high, id_low, id);
     #else
     printk("FT8719P compare:0x%02x,0x%02x,0x%02x,\n", id_high, id_low, id);
     #endif
 
     return (id == LCM_ID_FT8719P)?1:0;
	 }
else
       {
               return 0;
       }
	   }

LCM_DRIVER hct_ft8719p_dsi_vdo_fhdplus_jdi_63_baoxu = 
{
	.name			= "hct_ft8719p_dsi_vdo_fhdplus_jdi_63_baoxu",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,	
	.compare_id     = lcm_compare_id,	

#if (LCM_DSI_CMD_MODE)
    //.update         = lcm_update,
#endif	//wqtao
};

