lua_enum_pair vkcode_enum[]={
{"lbutton",0x01},
{"rbutton",0x02},
{"cancel",0x03},
{"mbutton",0x04},    /* NOT contiguous with L & RBUTTON */

{"xbutton1",0x05},    /* NOT contiguous with L & RBUTTON */
{"xbutton2",0x06}, 

{"back",0x08},
{"tab",0x09},


{"clear",0x0C},
{"return_",0x0D},


{"shift",0x10},
{"control",0x11},
{"menu",0x12},
{"pause",0x13},
{"capital",0x14},

{"kana",0x15},
{"hangeul",0x15},  /* old name - should be here for compatibility */
{"hangul",0x15},
{"ime_on",0x16},
{"junja",0x17},
{"final",0x18},
{"hanja",0x19},
{"kanji",0x19},
{"ime_off",0x1A},

{"escape",0x1B},

{"convert",0x1C},
{"nonconvert",0x1D},
{"accept",0x1E},
{"modechange",0x1F},

{"space",0x20},
{"prior",0x21},
{"next",0x22},
{"end",0x23},
{"home",0x24},
{"left",0x25},
{"up",0x26},
{"right",0x27},
{"down",0x28},
{"select",0x29},
{"print",0x2A},
{"execute",0x2B},
{"snapshot",0x2C},
{"insert",0x2D},
{"delete",0x2E},
{"help",0x2F},


{"lwin",0x5B},
{"rwin",0x5C},
{"apps",0x5D},

{"sleep",0x5F},

{"numpad0",0x60},
{"numpad1",0x61},
{"numpad2",0x62},
{"numpad3",0x63},
{"numpad4",0x64},
{"numpad5",0x65},
{"numpad6",0x66},
{"numpad7",0x67},
{"numpad8",0x68},
{"numpad9",0x69},
{"multiply",0x6A},
{"add",0x6B},
{"separator",0x6C},
{"subtract",0x6D},
{"decimal",0x6E},
{"divide",0x6F},
{"f1",0x70},
{"f2",0x71},
{"f3",0x72},
{"f4",0x73},
{"f5",0x74},
{"f6",0x75},
{"f7",0x76},
{"f8",0x77},
{"f9",0x78},
{"f10",0x79},
{"f11",0x7A},
{"f12",0x7B},
{"f13",0x7C},
{"f14",0x7D},
{"f15",0x7E},
{"f16",0x7F},
{"f17",0x80},
{"f18",0x81},
{"f19",0x82},
{"f20",0x83},
{"f21",0x84},
{"f22",0x85},
{"f23",0x86},
{"f24",0x87},

{"navigation_view",0x88}, // reserved
{"navigation_menu",0x89}, // reserved
{"navigation_up",0x8A}, // reserved
{"navigation_down",0x8B}, // reserved
{"navigation_left",0x8C}, // reserved
{"navigation_right",0x8D}, // reserved
{"navigation_accept",0x8E}, // reserved
{"navigation_cancel",0x8F}, // reserved

{"numlock",0x90},
{"scroll",0x91},

{"oem_nec_equal",0x92},   // '=' key on numpad

{"oem_fj_jisho",0x92},   // 'Dictionary' key
{"oem_fj_masshou",0x93},   // 'Unregister word' key
{"oem_fj_touroku",0x94},   // 'Register word' key
{"oem_fj_loya",0x95},   // 'Left OYAYUBI' key
{"oem_fj_roya",0x96},   // 'Right OYAYUBI' key

{"lshift",0xA0},
{"rshift",0xA1},
{"lcontrol",0xA2},
{"rcontrol",0xA3},
{"lmenu",0xA4},
{"rmenu",0xA5},

{"browser_back",0xA6},
{"browser_forward",0xA7},
{"browser_refresh",0xA8},
{"browser_stop",0xA9},
{"browser_search",0xAA},
{"browser_favorites",0xAB},
{"browser_home",0xAC},

{"volume_mute",0xAD},
{"volume_down",0xAE},
{"volume_up",0xAF},
{"media_next_track",0xB0},
{"media_prev_track",0xB1},
{"media_stop",0xB2},
{"media_play_pause",0xB3},
{"launch_mail",0xB4},
{"launch_media_select",0xB5},
{"launch_app1",0xB6},
{"launch_app2",0xB7},

{"oem_1",0xBA},   // ';:' for US
{"oem_plus",0xBB},   // '+' any country
{"oem_comma",0xBC},   // ',' any country
{"oem_minus",0xBD},   // '-' any country
{"oem_period",0xBE},   // '.' any country
{"oem_2",0xBF},   // '/?' for US
{"oem_3",0xC0},   // '`~' for US

{"gamepad_a",0xC3}, // reserved
{"gamepad_b",0xC4}, // reserved
{"gamepad_x",0xC5}, // reserved
{"gamepad_y",0xC6}, // reserved
{"gamepad_right_shoulder",0xC7}, // reserved
{"gamepad_left_shoulder",0xC8}, // reserved
{"gamepad_left_trigger",0xC9}, // reserved
{"gamepad_right_trigger",0xCA}, // reserved
{"gamepad_dpad_up",0xCB}, // reserved
{"gamepad_dpad_down",0xCC}, // reserved
{"gamepad_dpad_left",0xCD}, // reserved
{"gamepad_dpad_right",0xCE}, // reserved
{"gamepad_menu",0xCF}, // reserved
{"gamepad_view",0xD0}, // reserved
{"gamepad_left_thumbstick_button",0xD1}, // reserved
{"gamepad_right_thumbstick_button",0xD2}, // reserved
{"gamepad_left_thumbstick_up",0xD3}, // reserved
{"gamepad_left_thumbstick_down",0xD4}, // reserved
{"gamepad_left_thumbstick_right",0xD5}, // reserved
{"gamepad_left_thumbstick_left",0xD6}, // reserved
{"gamepad_right_thumbstick_up",0xD7}, // reserved
{"gamepad_right_thumbstick_down",0xD8}, // reserved
{"gamepad_right_thumbstick_right",0xD9}, // reserved
{"gamepad_right_thumbstick_left",0xDA}, // reserved

{"oem_4",0xDB},  //  '[{' for US
{"oem_5",0xDC},  //  '\|' for US
{"oem_6",0xDD},  //  ']}' for US
{"oem_7",0xDE},  //  ''"' for US
{"oem_8",0xDF},

{"oem_ax",0xE1},  //  'AX' key on Japanese AX kbd
{"oem_102",0xE2},  //  "<>" or "\|" on RT 102-key kbd.
{"ico_help",0xE3},  //  Help key on ICO
{"ico_00",0xE4},  //  00 key on ICO

{"processkey",0xE5},
{"ico_clear",0xE6},

{"packet",0xE7},
{"oem_reset",0xE9},
{"oem_jump",0xEA},
{"oem_pa1",0xEB},
{"oem_pa2",0xEC},
{"oem_pa3",0xED},
{"oem_wsctrl",0xEE},
{"oem_cusel",0xEF},
{"oem_attn",0xF0},
{"oem_finish",0xF1},
{"oem_copy",0xF2},
{"oem_auto",0xF3},
{"oem_enlw",0xF4},
{"oem_backtab",0xF5},

{"attn",0xF6},
{"crsel",0xF7},
{"exsel",0xF8},
{"ereof",0xF9},
{"play",0xFA},
{"zoom",0xFB},
{"noname",0xFC},
{"pa1",0xFD},
{"oem_clear",0xFE},
{NULL,-1}
};