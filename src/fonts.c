#include <fonts.h>

#include <locale.h>
#include <stdio.h>

#include <game.h>
#include <colors.h>

FC_Font* f_text;

bool fonts_init(void) {
    setlocale(LC_ALL, "ja_JP.utf8");
    FC_SetBufferSize(1024);

    f_text = FC_CreateFont();
    if(FC_LoadFont(f_text, renderer, "fonts/AozoraMinchoRegular.ttf", 24, uint_to_color(c_black), TTF_STYLE_NORMAL) == 0) {
        FC_FreeFont(f_text);
        return false;
    }

    return true;
}

void fonts_free(void) {
    FC_FreeFont(f_text);
}