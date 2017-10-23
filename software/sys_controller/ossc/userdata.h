//
// Copyright (C) 2015-2016  Markus Hiienkari <mhiienka@niksula.hut.fi>
//
// This file is part of Open Source Scan Converter project.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef USERDATA_H_
#define USERDATA_H_

#include "alt_types.h"
#include "sysconfig.h"
#include "controls.h"
#include "av_controller.h"
#include "avconfig.h"
#include "video_modes.h"
#include "flash.h"

#define MAX_PROFILE 11
#define INIT_CONFIG_SLOT MAX_USERDATA_ENTRY

typedef enum {
    UDE_INITCFG  = 0,
    UDE_PROFILE,
} ude_type;

typedef struct {
    char userdata_key[8];
    alt_u8 version_major;
    alt_u8 version_minor;
    ude_type type;
} __attribute__((packed, __may_alias__)) ude_hdr;

typedef struct {
    ude_hdr hdr;
    alt_u16 data_len;
    //alt_u8 last_profile;
    alt_u8 last_profile[3];
    alt_u8 profile_mode;
    avinput_t last_input;
    alt_u16 keys[REMOTE_MAX_KEYS];
} __attribute__((packed, __may_alias__)) ude_initcfg;

typedef struct {
    ude_hdr hdr;
    alt_u16 avc_data_len;
    alt_u16 vm_data_len;
    avconfig_t avc;
    mode_data_t vm[VIDEO_MODES_CNT];
} __attribute__((packed, __may_alias__)) ude_profile;

int write_userdata(alt_u8 entry);
int read_userdata(alt_u8 entry);

#endif
