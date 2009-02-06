/****************************************************************************
* This file is part of lulzJS-ncurses                                       *
* Copyleft meh.                                                             *
*                                                                           *
* lulzJS-ncurses is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation, either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* lulzJS-ncurses is distributed in the hope that it will be useful.         *
* but WITHOUT ANY WARRANTY; without even the implied warranty o.            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See th.             *
* GNU General Public License for more details.                              *
*                                                                           *
* You should have received a copy of the GNU General Public License         *
* along with lulzJS-ncurses.  If not, see <http://www.gnu.org/licenses/>.   *
****************************************************************************/

#ifndef _LULZJS_NCURSES_PANEL_H
#define _LULZJS_NCURSES_PANEL_H

#include "common.h"
#include <map>

std::map <PANEL*, JSObject*> panels;

extern JSBool exec (JSContext* cx);
extern JSBool Panel_initialize (JSContext* cx);

extern JSBool Panel_constructor (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval);
extern void   Panel_finalize (JSContext* cx, JSObject* object); 

static JSClass Panel_class = {
    "Panel", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, Panel_finalize
};

extern JSBool Panel_hide (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval);
extern JSBool Panel_show (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval);
extern JSBool Panel_move (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval);

extern JSBool Panel_toTop (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval);
extern JSBool Panel_toBottom (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval);

extern JSBool Panel_above (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval);
extern JSBool Panel_below (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval);

extern JSBool Panel_static_update (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval);

static JSFunctionSpec Panel_methods[] = {
    {"hide", Panel_hide, 0, 0, 0},
    {"show", Panel_show, 0, 0, 0},
    {"move", Panel_move, 0, 0, 0},

    {"toTop",    Panel_toTop,    0, 0, 0},
    {"toBottom", Panel_toBottom, 0, 0, 0},
    {NULL}
};

static JSFunctionSpec Panel_static_methods[] = { 
    {"update", Panel_static_update, 0, 0, 0},
    {NULL}
};

#endif
