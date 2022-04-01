#ifndef HotKeyNameHPP
#define HotKeyNameHPP
//------------------------------------------------------------------------------
typedef struct {
    const char *name;
    UINT  VK;
} HK_NAME;

HK_NAME hotkey_names[]={
                    {"<BACKSPACE>",       0x08},
                    {"<TAB>",             0x09},
//                  {"<CLEAR>",           0x0C},
                    {"<ENTER>",           0x0D},
//                  {"<SHIFT>",           0x10},
//                  {"<CONTROL>",         0x11},
//                  {"<MENU?>",           0x12},
                    {"<PAUSE>",           0x13},
                    {"<CAPSLOCK>",        0x14},
//                  {"<KANA>",            0x15},
//                  {"<HANGEUL>",         0x15},
//                  {"<HANGUL>",          0x15},
//                  {"<JUNJA>",           0x17},
//                  {"<FINAL>",           0x18},
//                  {"<HANJA>",           0x19},
//                  {"<KANJI>",           0x19},
                    {"<ESCAPE>",          0x1B},
//                  {"<CONVERT>",         0x1C},
//                  {"<NONCONVERT>",      0x1D},
//                  {"<ACCEPT>",          0x1E},
//                  {"<MODECHANGE>",      0x1F},
                    {"<SPACE>",           0x20},
                    {"<PAGE_UP>",         0x21},
                    {"<PAGE_DOWN>",       0x22},
                    {"<END>",             0x23},
                    {"<HOME>",            0x24},
                    {"<LEFT>",            0x25},
                    {"<UP>",              0x26},
                    {"<RIGHT>",           0x27},
                    {"<DOWN>",            0x28},
                    {"<SELECT>",          0x29},
                    {"<PRINT>",           0x2A},
                    {"<EXECUTE>",         0x2B},
                    {"<PRTSCR>",          0x2C},
                    {"<INSERT>",          0x2D},
                    {"<DELETE>",          0x2E},
                    {"<HELP>",            0x2F},
                    {"<0>",               0x30},
                    {"<1>",               0x31},
                    {"<2>",               0x32},
                    {"<3>",               0x33},
                    {"<4>",               0x34},
                    {"<5>",               0x35},
                    {"<6>",               0x36},
                    {"<7>",               0x37},
                    {"<8>",               0x38},
                    {"<9>",               0x39},
                    {"<A>",               0x41},
                    {"<B>",               0x42},
                    {"<C>",               0x43},
                    {"<D>",               0x44},
                    {"<E>",               0x45},
                    {"<F>",               0x46},
                    {"<G>",               0x47},
                    {"<H>",               0x48},
                    {"<I>",               0x49},
                    {"<J>",               0x4A},
                    {"<K>",               0x4B},
                    {"<L>",               0x4C},
                    {"<M>",               0x4D},
                    {"<N>",               0x4E},
                    {"<O>",               0x4F},
                    {"<P>",               0x50},
                    {"<Q>",               0x51},
                    {"<R>",               0x52},
                    {"<S>",               0x53},
                    {"<T>",               0x54},
                    {"<U>",               0x55},
                    {"<V>",               0x56},
                    {"<W>",               0x57},
                    {"<X>",               0x58},
                    {"<Y>",               0x59},
                    {"<Z>",               0x5A},
//                  {"<L_WIN>",           0x5B},
//                  {"<R_WIN>",           0x5C},
                    {"<MENU>",            0x5D},
                    {"<SLEEP>",           0x5F},
                    {"<NUMPAD_0>",        0x60},
                    {"<NUMPAD_1>",        0x61},
                    {"<NUMPAD_2>",        0x62},
                    {"<NUMPAD_3>",        0x63},
                    {"<NUMPAD_4>",        0x64},
                    {"<NUMPAD_5>",        0x65},
                    {"<NUMPAD_6>",        0x66},
                    {"<NUMPAD_7>",        0x67},
                    {"<NUMPAD_8>",        0x68},
                    {"<NUMPAD_9>",        0x69},
                    {"<NUMPAD_MUL>",      0x6A},
                    {"<NUMPAD_PLUS>",     0x6B},
                    {"<SEPARATOR>",       0x6C},
                    {"<NUMPAD_MINUS>",    0x6D},
                    {"<DECIMAL>",         0x6E},
                    {"<NUMPAD_DIV>",      0x6F},
                    {"<F1>",              0x70},
                    {"<F2>",              0x71},
                    {"<F3>",              0x72},
                    {"<F4>",              0x73},
                    {"<F5>",              0x74},
                    {"<F6>",              0x75},
                    {"<F7>",              0x76},
                    {"<F8>",              0x77},
                    {"<F9>",              0x78},
                    {"<F10>",             0x79},
                    {"<F11>",             0x7A},
                    {"<F12>",             0x7B},
                    {"<F13>",             0x7C},
                    {"<F14>",             0x7D},
                    {"<F15>",             0x7E},
                    {"<F16>",             0x7F},
                    {"<F17>",             0x80},
                    {"<F18>",             0x81},
                    {"<F19>",             0x82},
                    {"<F20>",             0x83},
                    {"<F21>",             0x84},
                    {"<F22>",             0x85},
                    {"<F23>",             0x86},
                    {"<F24>",             0x87},
                    {"<NUMLOCK>",         0x90},
                    {"<SCROLL>",          0x91},
//                  {"<OEM_NEC_EQUAL>",   0x92},
//                  {"<OEM_FJ_JISHO>",    0x92},
//                  {"<OEM_FJ_MASSHOU>",  0x93},
//                  {"<OEM_FJ_TOUROKU>",  0x94},
//                  {"<OEM_FJ_LOYA>",     0x95},
//                  {"<OEM_FJ_ROYA>",     0x96},
//                  {"<L_SHIFT>",         0xA0},
//                  {"<R_SHIFT>",         0xA1},
//                  {"<L_CONTROL>",       0xA2},
//                  {"<R_CONTROL>",       0xA3},
//                  {"<L_MENU>",          0xA4},
//                  {"<R_MENU>",          0xA5},
                    {"<WEB_BACK>",        0xA6},
                    {"<WEB_FORWARD>",     0xA7},
                    {"<WEB_REFRESH>",     0xA8},
                    {"<WEB_STOP>",        0xA9},
                    {"<WEB_SEARCH>",      0xAA},
                    {"<WEB_FAVORITES>",   0xAB},
                    {"<WEB_HOME>",        0xAC},
                    {"<VOLUME_MUTE>",     0xAD},
                    {"<VOLUME_DOWN>",     0xAE},
                    {"<VOLUME_UP>",       0xAF},
                    {"<MEDIA_NEXT>",      0xB0},
                    {"<MEDIA_PREV>",      0xB1},
                    {"<MEDIA_STOP>",      0xB2},
                    {"<MEDIA_PLAYPAUSE>", 0xB3},
                    {"<LAUNCH_MAIL>",     0xB4},
                    {"<LAUNCH_MEDIASEL>", 0xB5},
                    {"<LAUNCH_APP1>",     0xB6},
                    {"<LAUNCH_APP2>",     0xB7},
//                  {"<OEM_1>",           0xBA},
//                  {"<OEM_PLUS>",        0xBB},
//                  {"<OEM_COMMA>",       0xBC},
//                  {"<OEM_MINUS>",       0xBD},
//                  {"<OEM_PERIOD>",      0xBE},
//                  {"<OEM_2>",           0xBF},
//                  {"<OEM_3>",           0xC0},
//                  {"<OEM_4>",           0xDB},
//                  {"<OEM_5>",           0xDC},
//                  {"<OEM_6>",           0xDD},
//                  {"<OEM_7>",           0xDE},
//                  {"<OEM_8>",           0xDF},
//                  {"<OEM_AX>",          0xE1},
//                  {"<OEM_102>",         0xE2},
//                  {"<ICO_HELP>",        0xE3},
//                  {"<ICO_00>",          0xE4},
//                  {"<PROCESSKEY>",      0xE5},
//                  {"<ICO_CLEAR>",       0xE6},
//                  {"<PACKET>",          0xE7},
//                  {"<OEM_RESET>",       0xE9},
//                  {"<OEM_JUMP>",        0xEA},
//                  {"<OEM_PA1>",         0xEB},
//                  {"<OEM_PA2>",         0xEC},
//                  {"<OEM_PA3>",         0xED},
//                  {"<OEM_WSCTRL>",      0xEE},
//                  {"<OEM_CUSEL>",       0xEF},
//                  {"<OEM_ATTN>",        0xF0},
//                  {"<OEM_FINISH>",      0xF1},
//                  {"<OEM_COPY>",        0xF2},
//                  {"<OEM_AUTO>",        0xF3},
//                  {"<OEM_ENLW>",        0xF4},
//                  {"<OEM_BACKTAB>",     0xF5},
//                  {"<ATTN>",            0xF6},
//                  {"<CRSEL>",           0xF7},
//                  {"<EXSEL>",           0xF8},
//                  {"<EREOF>",           0xF9},
//                  {"<PLAY>",            0xFA},
//                  {"<ZOOM>",            0xFB},
//                  {"<NONAME>",          0xFC},
//                  {"<PA1>",             0xFD},
//                  {"<OEM_CLEAR>",       0xFE},
                    {"<POWER>",           0xFF},
                    {NULL,                0x00}
};

const char *getkeyname(unsigned int vk_code)
{
	for(int i=0;hotkey_names[i].name;i++)
		if(hotkey_names[i].VK==vk_code)
			return hotkey_names[i].name;
    return NULL;
}





#endif
