#! /usr/bin/env ljs
require("ncurses");

var screen = ncurses.Screen.init({buffering: ncurses.Buffering.Raw});

var panels = []
panels.push(new ncurses.Panel({x: 2, y: 2, width: 30, height: 10, border: true}));
panels.push(new ncurses.Panel({x: 4, y: 5, width: 30, height: 10, border: true}));
panels[0].printString("Width: {Width}, Height: {Height}   ".format(panels[0].Size), {x: 1, y: 1});
panels[1].printString("PENIS", {x: 10, y: 4});
panels[0].toTop();
panels[0].getChar();
panels[0].below.getChar();
panels[0].resize({height: 1});
panels[0].printString("Width: {Width}, Height: {Height}   ".format(panels[0].Size), {x: 1, y: 1});
panels[0].getChar();
