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

Object.extend(ncurses.Panel.prototype, {
    refresh: function () {
        this.__window.refresh();
    },

    resize: function (obj) {
        this.__window.resize(obj);
        ncurses.Screen.refresh();
        ncurses.Panel.update();
    },

    printChar: function (ch, options) {
        if (options) this.__window.printChar(ch, options);
        else         this.__window.printChar(ch);
    },

    getChar: function (options) {
        return (options 
            ? this.__window.getChar(options)
            : this.__window.getChar());
    },

    printString: function (str, options) {
        if (options) this.__window.printString(str, options);
        else         this.__window.printString(str);
    }
});

Object.addGetters(ncurses.Panel, {
    Size: function () {
        return this.__window.Size;
    },

    Position: function () {
        return this.__window.Position;
    }
});
