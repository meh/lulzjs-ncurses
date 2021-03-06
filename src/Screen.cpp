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

#include "Screen.h"

void __Screen_updateSize (JSContext* cx, JSObject* object);
void __Screen_updateACS (JSContext* cx);

static JSContext* signalCx;
static JSObject*  signalObject;
void __Screen_resize (int signum) {
    endwin(); initscr(); refresh();
    __Screen_updateSize(signalCx, signalObject);
    jsval ret; JS_CallFunctionName(signalCx, signalObject, "onResize", 0, NULL, &ret);
    refresh();
}

JSBool exec (JSContext* cx) { return Screen_initialize(cx); }

JSBool
Screen_initialize (JSContext* cx)
{
    jsval jsParent;
    JS_GetProperty(cx, JS_GetGlobalObject(cx), "ncurses", &jsParent);
    JSObject* parent = JSVAL_TO_OBJECT(jsParent);

    JSObject* object = JS_DefineObject(
        cx, parent,
        Screen_class.name, &Screen_class, NULL,
        JSPROP_PERMANENT|JSPROP_READONLY|JSPROP_ENUMERATE
    );
    if (object) {
        JS_DefineFunctions(cx, object, Screen_methods);

        return JS_TRUE;
    }

    return JS_FALSE;
}

JSBool
Screen_init (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval)
{
    JSObject* options;

    if (stdscr) {
        JS_ReportError(cx, "You can have only one Screen per program.");
        return JS_FALSE;
    }

    ScreenInformation* data = new ScreenInformation;
    JS_SetPrivate(cx, object, data);

    signalCx     = cx;
    signalObject = object;
    signal(SIGWINCH, __Screen_resize);

    initscr();
    __Screen_updateACS(cx);

    JSObject* Size   = JS_NewObject(cx, NULL, NULL, NULL);
    jsval     jsSize = OBJECT_TO_JSVAL(Size);
    JS_SetProperty(cx, object, "Size", &jsSize);
    __Screen_updateSize(cx, object);

    if (has_colors()) {
        start_color();
        use_default_colors();
    }

    jsval property = JS_EVAL(cx, "ncurses.Window");
    JS_GetProperty(cx, JSVAL_TO_OBJECT(property), "prototype", &property);
    JSObject* proto  = JSVAL_TO_OBJECT(property);

    JSObject* Window = JS_NewObject(cx, &Window_class, proto, NULL); 
    property = OBJECT_TO_JSVAL(Window);
    JS_SetProperty(cx, object, "__window", &property);
    JS_SetPrivate(cx, Window, stdscr);
    property = JSVAL_FALSE;
    JS_SetProperty(cx, Window, "border", &property);

    if (argc > 0) {
        JS_ValueToObject(cx, argv[0], &options);
    }
    else {
        options = JS_NewObject(cx, NULL, NULL, NULL);
    }
    
    if (argc > 1 && JS_OBJECT_IS(cx, argv[1], "Function")) {
        JS_SetProperty(cx, object, "onResize", &argv[1]);
    }
    else {
        jsval jsfunc;
        JS_GetProperty(cx, JS_GetGlobalObject(cx), "Function", &jsfunc);
        JS_GetProperty(cx, JSVAL_TO_OBJECT(jsfunc), "empty", &jsfunc);
        JS_SetProperty(cx, object, "onResize", &jsfunc);
    }

    jsval option;

    // Echoing of inputted keys
    JS_GetProperty(cx, options, "echo", &option);
    JSBool js_echo; JS_ValueToBoolean(cx, option, &js_echo);
    if (JSVAL_IS_VOID(option) || !js_echo) {
        noecho();
        option = JSVAL_FALSE;
    }
    else {
        echo();
        option = JSVAL_TRUE;
    }
    data->echo = JSVAL_TO_BOOLEAN(option);
    JS_SetProperty(cx, object, "echo", &option);

    // Buffering type
    JS_GetProperty(cx, options, "buffering", &option);
    jsint js_buffering = JS_ParseInt(cx, option, 0);
    switch (js_buffering) {
        case  2: raw(); break;
        case  3: cbreak(); break;
        default: option = INT_TO_JSVAL(1); break;
    }
    data->buffering = JSVAL_TO_INT(option);
    JS_SetProperty(cx, object, "buffering", &option);

    // Keypad initialization
    JS_GetProperty(cx, options, "keypad", &option);
    JSBool js_keypad; JS_ValueToBoolean(cx, option, &js_keypad);
    if (JSVAL_IS_VOID(option) || js_keypad) {
        keypad(stdscr, TRUE);
        option = JSVAL_TRUE;
    }
    else {
        option = JSVAL_FALSE;
    }
    data->keypad = JSVAL_TO_BOOLEAN(option);
    JS_SetProperty(cx, object, "keypad", &option);

    // Cursor state
    JS_GetProperty(cx, options, "cursor", &option);
    jsint cursor = JS_ParseInt(cx, option, 0);
    curs_set(cursor);
    option = INT_TO_JSVAL(cursor);
    JS_SetProperty(cx, object, "cursor", &option);

    *rval = OBJECT_TO_JSVAL(object);

    return JS_TRUE;
}

void
Screen_finalize (JSContext* cx, JSObject* object)
{
    ScreenInformation* data = (ScreenInformation*) JS_GetPrivate(cx, object);

    if (data) {
        switch (data->buffering) {
            case 2: noraw(); break;
            case 3: nocbreak(); break;
        }

        if (data->keypad) {
            keypad(stdscr, FALSE);
        }

        if (!data->echo) {
            echo();
        }

        curs_set(1);
        endwin();

        delete data;
    }
}

JSBool
Screen_update (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval)
{
    doupdate();
    return JS_TRUE;
}

JSBool
Screen_cursorMode (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval)
{
    jsint val;

    if (argc < 1 || !JS_ConvertArguments(cx, argc, argv, "i", &val)) {
        JS_ReportError(cx, "Not enough parameters.");
        return JS_FALSE;
    }

    curs_set(val);

    return JS_TRUE;
}

void
__Screen_updateSize (JSContext* cx, JSObject* object)
{
    int height, width;
    getmaxyx(stdscr, height, width);

    jsval jsSize; JS_GetProperty(cx, object, "Size", &jsSize);
    JSObject* Size = JSVAL_TO_OBJECT(jsSize);

    jsval property;
    property = INT_TO_JSVAL(height);
    JS_SetProperty(cx, Size, "Height", &property);
    property = INT_TO_JSVAL(width);
    JS_SetProperty(cx, Size, "Width", &property);
}

void
__Screen_updateACS (JSContext* cx)
{
    jsval property;

    JS_GetProperty(cx, JS_GetGlobalObject(cx), "ncurses", &property);
    JSObject* AlternativeChars   = JS_NewObject(cx, NULL, NULL, NULL);
    jsval     jsAlternativeChars = OBJECT_TO_JSVAL(AlternativeChars);
    JS_SetProperty(cx, JSVAL_TO_OBJECT(property), "AlternativeChars", &jsAlternativeChars);
        property = INT_TO_JSVAL(ACS_BLOCK);
        JS_SetProperty(cx, AlternativeChars, "Block", &property);
        property = INT_TO_JSVAL(ACS_BOARD);
        JS_SetProperty(cx, AlternativeChars, "Board", &property);
        property = INT_TO_JSVAL(ACS_BTEE);
        JS_SetProperty(cx, AlternativeChars, "BottomT", &property);
        property = INT_TO_JSVAL(ACS_BULLET);
        JS_SetProperty(cx, AlternativeChars, "Bullet", &property);
        property = INT_TO_JSVAL(ACS_CKBOARD);
        JS_SetProperty(cx, AlternativeChars, "CheckerBoard", &property);
        property = INT_TO_JSVAL(ACS_DARROW);
        JS_SetProperty(cx, AlternativeChars, "DownArrow", &property);
        property = INT_TO_JSVAL(ACS_DEGREE);
        JS_SetProperty(cx, AlternativeChars, "Degree", &property);
        property = INT_TO_JSVAL(ACS_DIAMOND);
        JS_SetProperty(cx, AlternativeChars, "Diamond", &property);
        property = INT_TO_JSVAL(ACS_GEQUAL);
        JS_SetProperty(cx, AlternativeChars, "GreaterEqual", &property);
        property = INT_TO_JSVAL(ACS_HLINE);
        JS_SetProperty(cx, AlternativeChars, "HorizontalLine", &property);
        property = INT_TO_JSVAL(ACS_LANTERN);
        JS_SetProperty(cx, AlternativeChars, "Lantern", &property);
        property = INT_TO_JSVAL(ACS_LARROW);
        JS_SetProperty(cx, AlternativeChars, "LeftArrow", &property);
        property = INT_TO_JSVAL(ACS_LEQUAL);
        JS_SetProperty(cx, AlternativeChars, "LessEqual", &property);
        property = INT_TO_JSVAL(ACS_LLCORNER);
        JS_SetProperty(cx, AlternativeChars, "LowerLeftCorner", &property);
        property = INT_TO_JSVAL(ACS_LRCORNER);
        JS_SetProperty(cx, AlternativeChars, "LowerRightCorner", &property);
        property = INT_TO_JSVAL(ACS_LTEE);
        JS_SetProperty(cx, AlternativeChars, "LeftT", &property);
        property = INT_TO_JSVAL(ACS_NEQUAL);
        JS_SetProperty(cx, AlternativeChars, "NotEqual", &property);
        property = INT_TO_JSVAL(ACS_PI);
        JS_SetProperty(cx, AlternativeChars, "GreekPi", &property);
        property = INT_TO_JSVAL(ACS_PLMINUS);
        JS_SetProperty(cx, AlternativeChars, "PlusMinus", &property);
        property = INT_TO_JSVAL(ACS_PLUS);
        JS_SetProperty(cx, AlternativeChars, "Plus", &property);
        property = INT_TO_JSVAL(ACS_RARROW);
        JS_SetProperty(cx, AlternativeChars, "RightArrow", &property);
        property = INT_TO_JSVAL(ACS_RTEE);
        JS_SetProperty(cx, AlternativeChars, "RightT", &property);
        property = INT_TO_JSVAL(ACS_S1);
        JS_SetProperty(cx, AlternativeChars, "Scan1", &property);
        property = INT_TO_JSVAL(ACS_S3);
        JS_SetProperty(cx, AlternativeChars, "Scan3", &property);
        property = INT_TO_JSVAL(ACS_S7);
        JS_SetProperty(cx, AlternativeChars, "Scan7", &property);
        property = INT_TO_JSVAL(ACS_S9);
        JS_SetProperty(cx, AlternativeChars, "Scan9", &property);
        property = INT_TO_JSVAL(ACS_STERLING);
        JS_SetProperty(cx, AlternativeChars, "Pound", &property);
        property = INT_TO_JSVAL(ACS_TTEE);
        JS_SetProperty(cx, AlternativeChars, "TopT", &property);
        property = INT_TO_JSVAL(ACS_UARROW);
        JS_SetProperty(cx, AlternativeChars, "UpperArrow", &property);
        property = INT_TO_JSVAL(ACS_ULCORNER);
        JS_SetProperty(cx, AlternativeChars, "UpperLeftCorner", &property);
        property = INT_TO_JSVAL(ACS_URCORNER);
        JS_SetProperty(cx, AlternativeChars, "UpperRightCorner", &property);
        property = INT_TO_JSVAL(ACS_VLINE);
        JS_SetProperty(cx, AlternativeChars, "VerticalLine", &property);
        property = INT_TO_JSVAL(ACS_STERLING);
        JS_SetProperty(cx, AlternativeChars, "Pound", &property);
        property = INT_TO_JSVAL(ACS_TTEE);
        JS_SetProperty(cx, AlternativeChars, "TopT", &property);
        property = INT_TO_JSVAL(ACS_UARROW);
        JS_SetProperty(cx, AlternativeChars, "UpperArrow", &property);
        property = INT_TO_JSVAL(ACS_ULCORNER);
        JS_SetProperty(cx, AlternativeChars, "UpperLeftCorner", &property);
        property = INT_TO_JSVAL(ACS_URCORNER);
        JS_SetProperty(cx, AlternativeChars, "UpperRightCorner", &property);
        property = INT_TO_JSVAL(ACS_VLINE);
        JS_SetProperty(cx, AlternativeChars, "VerticalLine", &property);
}

