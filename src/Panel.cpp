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

#include "Panel.h"

JSBool exec (JSContext* cx) { return Panel_initialize(cx); }

JSBool
Panel_initialize (JSContext* cx)
{
    JS_BeginRequest(cx);
    JS_EnterLocalRootScope(cx);

    jsval jsParent;
    JS_GetProperty(cx, JS_GetGlobalObject(cx), "ncurses", &jsParent);
    JSObject* parent = JSVAL_TO_OBJECT(jsParent);

    JSObject* object = JS_InitClass(
        cx, parent, NULL, &Panel_class,
        Panel_constructor, 1, NULL, Panel_methods, NULL, Panel_static_methods
    );

    if (object) {
        JS_LeaveLocalRootScope(cx);
        JS_EndRequest(cx);
        return JS_TRUE;
    }

    return JS_FALSE;
}

JSBool
Panel_constructor (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval)
{
    if (argc < 1) {
        JS_ReportError(cx, "Not enough parameters.");
        return JS_FALSE;
    }

    JS_BeginRequest(cx);
    JS_EnterLocalRootScope(cx);

    jsval property = JS_EVAL(cx, "ncurses.Window");
    JSObject* obj  = JSVAL_TO_OBJECT(property);

    JSObject* Window = JS_New(cx, obj, argc, argv);
    property         = OBJECT_TO_JSVAL(Window);
    JS_SetProperty(cx, object, "__window", property);

    PANEL* panel = new_panel((WINDOW*)JS_GetPrivate(cx, Window));
    JS_SetPrivate(cx, object, panel);
    
    update_panels();
    panels[panel] = object;
    
    JS_DefineProperty(
        cx, object, "below", JSVAL_NULL,
        Panel_below, NULL, JSPROP_GETTER|JSPROP_READONLY
    );
    JS_DefineProperty(
        cx, object, "above", JSVAL_NULL,
        Panel_above, NULL, JSPROP_GETTER|JSPROP_READONLY
    );

    JS_LeaveLocalRootScope(cx);
    JS_EndRequest(cx);

    return JS_TRUE;
}

void
Panel_finalize (JSContext* cx, JSObject* object)
{
    PANEL* panel = (PANEL*) JS_GetPrivate(cx, object);

    if (panel) {
        panels.erase(panel);
        del_panel(panel);
    }
}

JSBool
Panel_hide (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval)
{
    PANEL* panel = (PANEL*) JS_GetPrivate(cx, object);
    hide_panel(panel);

    JS_BeginRequest(cx);
    JS_EnterLocalRootScope(cx);

    jsval val = JSVAL_TRUE;
    JS_SetProperty(cx, object, "hidden", &val);

    JS_LeaveLocalRootScope(cx);
    JS_EndRequest(cx);

    return JS_TRUE;
}

JSBool
Panel_show (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval)
{
    PANEL* panel = (PANEL*) JS_GetPrivate(cx, object);
    show_panel(panel);

    JS_BeginRequest(cx);
    JS_EnterLocalRootScope(cx);

    jsval val = JSVAL_FALSE;
    JS_SetProperty(cx, object, "hidden", &val);

    return JS_TRUE;
}

JSBool
Panel_move (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval)
{
    JSObject* options;

    if (argc < 1) {
        JS_ReportError(cx, "Not enough parameters.");
        return JS_FALSE;
    }

    JS_BeginRequest(cx);
    JS_EnterLocalRootScope(cx);

    JS_ValueToObject(cx, argv[0], &options);
    jsval x, y;

    JS_GetProperty(cx, options, "x", &x);
    if (JSVAL_IS_VOID(x) || JSVAL_IS_NULL(x)) {
        JS_GetProperty(cx, options, "X", &x);
    }

    JS_GetProperty(cx, options, "y", &y);
    if (JSVAL_IS_VOID(y) || JSVAL_IS_NULL(y)) {
        JS_GetProperty(cx, options, "Y", &y);
    }

    if (!JSVAL_IS_INT(x) && !JSVAL_IS_INT(y)) {
        JS_ReportError(cx, "An option isn't an int.");

        JS_BeginRequest(cx);
        JS_EnterLocalRootScope(cx);
        return JS_FALSE;
    }

    if (!JSVAL_IS_INT(x)) {
        JS_GetProperty(cx, object, "Position", &x);
        JS_GetProperty(cx, JSVAL_TO_OBJECT(x), "X", &x);
    }

    if (!JSVAL_IS_INT(y)) {
        JS_GetProperty(cx, object, "Position", &y);
        JS_GetProperty(cx, JSVAL_TO_OBJECT(y), "Y", &y);
    }

    JS_LeaveLocalRootScope(cx);
    JS_EndRequest(cx);

    PANEL* panel = (PANEL*) JS_GetPrivate(cx, object);
    move_panel(panel, JSVAL_TO_INT(y), JSVAL_TO_INT(x));
    refresh();
    update_panels();
    doupdate();

    return JS_TRUE;
}

JSBool
Panel_toTop (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval)
{
    PANEL* panel = (PANEL*) JS_GetPrivate(cx, object);
    top_panel(panel);

    return JS_TRUE;
}

JSBool
Panel_toBottom (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval)
{
    PANEL* panel = (PANEL*) JS_GetPrivate(cx, object);
    bottom_panel(panel);

    return JS_TRUE;
}

JSBool
Panel_above (JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    PANEL* panel = (PANEL*) JS_GetPrivate(cx, obj);
    PANEL* above = panel_above(panel);

    *vp = OBJECT_TO_JSVAL(panels[above]);

    return JS_TRUE;
}

JSBool
Panel_below (JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    PANEL* panel = (PANEL*) JS_GetPrivate(cx, obj);
    PANEL* below = panel_below(panel);

    *vp = OBJECT_TO_JSVAL(panels[below]);

    return JS_TRUE;
}

JSBool
Panel_static_update (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval)
{
    update_panels();
    return JS_TRUE;
}

