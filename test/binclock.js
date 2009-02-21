#! /usr/bin/env ljs
require("ncurses");

var screen = ncurses.Screen.init({buffering: ncurses.Buffering.CBreak});
screen.show = true;

screen.showClock = function ()
{
    this.createContainer();

    for (let name in bcd) {
        let x;
        switch (name) {
            case   "hours": x = 15; break;
            case "minutes": x = 25; break;
            case "seconds": x = 36; break;
        }

        for (let i = 3; i >= 0; i--) {
            this.printString(bcd[name].u[i], {x: x, y: i+5, fg: ncurses.Colors.Blue});
        }

        if (name != "hours") {
            for (let i = 2; i >= 0; i--) {
                this.printString(bcd[name].d[i], {x: x-4, y: i+6, fg: ncurses.Colors.Blue});
            }
        }
        else {
            for (let i = 1; i >= 0; i--) {
                this.printString(bcd[name].d[i], {x: x-4, y: i+7, fg: ncurses.Colors.Blue});
            }
        }
    }

    for (let name in binary) {
        let x;
        switch (name) {
            case   "hours": x = 13; break;
            case "minutes": x = 23; break;
            case "seconds": x = 34; break;
        }

        if (name != "hours") {
            for (let i = 5; i >= 0; i--) {
                this.printString(binary[name][i], {x: x, y: i+10, fg: ncurses.Colors.Blue});
            }
        }
        else {
            for (let i = 4; i >= 0; i--) {
                this.printString(binary[name][i], {x: x, y: i+11, fg: ncurses.Colors.Blue});
            }
        }
    }

    var hours   = now.getHours().toPaddedString(2).toArray();
    var minutes = now.getMinutes().toPaddedString(2).toArray();
    var seconds = now.getSeconds().toPaddedString(2).toArray();

    this.printString(hours[0]+" "+hours[1], {x: 12, y: 17, fg: ncurses.Colors.Blue});
    this.printString(":", {x: 18, y: 17, fg: ncurses.Colors.Blue});
    this.printString(minutes[0]+" "+minutes[1], {x: 22, y: 17, fg: ncurses.Colors.Blue});
    this.printString(":", {x: 29, y: 17, fg: ncurses.Colors.Blue});
    this.printString(seconds[0]+" "+seconds[1], {x: 33, y: 17, fg: ncurses.Colors.Blue});

    this.refresh();
};

screen.createContainer = function ()
{
    this.printString("Hours",   {x: 11, y: 3, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString("Minutes", {x: 20, y: 3, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString("Seconds", {x: 31, y: 3, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});

    this.printString("8", {x: 8, y: 5, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString("4", {x: 8, y: 6, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString("2", {x: 8, y: 7, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString("1", {x: 8, y: 8, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});

    this.printString("B", {x: 40, y: 5, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString("C", {x: 40, y: 6, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString("D", {x: 40, y: 7, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});

    this.printString(" ".times(31), {x: 9, y: 4, bg: ncurses.Colors.Red});
    for each (let i in range(5, 9)) {
        this.printString(" ", {x: 9,  y: i, bg: ncurses.Colors.Red});
        this.printString(" ", {x: 39, y: i, bg: ncurses.Colors.Red});
    }
    this.printString(" ".times(31), {x: 9, y: 9, bg: ncurses.Colors.Red});

    this.printString("32", {x: 7, y: 10, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString("16", {x: 7, y: 11, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString( "8", {x: 8, y: 12, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString( "4", {x: 8, y: 13, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString( "2", {x: 8, y: 14, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString( "1", {x: 8, y: 15, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});

    this.printString("B", {x: 40, y: 10, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString("I", {x: 40, y: 11, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString("N", {x: 40, y: 12, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString("A", {x: 40, y: 13, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString("R", {x: 40, y: 14, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    this.printString("Y", {x: 40, y: 15, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});

    for each (let i in range(10, 15)) {
        this.printString(" ", {x: 9,  y: i, bg: ncurses.Colors.Red});
        this.printString(" ", {x: 39, y: i, bg: ncurses.Colors.Red});
    }
    this.printString(" ".times(31), {x: 9, y: 16, bg: ncurses.Colors.Red});

    this.printString(" ", {x: 9,  y: 17, bg: ncurses.Colors.Red});
    this.printString(" ", {x: 39, y: 17, bg: ncurses.Colors.Red});

    this.printString(" ".times(31), {x: 9, y: 18, bg: ncurses.Colors.Red});
}

screen.onResize = function () {
    if (this.Size.Width < 50 || this.Size.Height < 23) {
        this.show = false;
    }
    else {
        this.show = true;
        this.erase();
        this.showClock();
    }
};

screen.printString("Press C-c to close the program", {fg: ncurses.Colors.Red, at: ncurses.Attributes.Underline});
screen.refresh();

while (true) {
    now = new Date;

    binary = {
        seconds: now.getSeconds().toPaddedString(6, 2).toArray(),
        minutes: now.getMinutes().toPaddedString(6, 2).toArray(),
        hours  : now.getHours().toPaddedString(5, 2).toArray()
    };

    bcd = {
        seconds: {
            d: Math.floor(now.getSeconds()/10).toPaddedString(3, 2).toArray(),
            u: (now.getSeconds()%10).toPaddedString(4, 2).toArray()
        },

        minutes: {
            d: Math.floor(now.getMinutes()/10).toPaddedString(3, 2).toArray(),
            u: (now.getMinutes()%10).toPaddedString(4, 2).toArray()
        },

        hours: {
            d: Math.floor(now.getHours()/10).toPaddedString(2, 2).toArray(),
            u: (now.getHours()%10).toPaddedString(4, 2).toArray()
        }
    };

    if (screen.show) {
        screen.printString(screen.show, {x: 2, y: 2});
        screen.showClock();
    }
    else {
        screen.erase();
        screen.printString("LOL FAG", {x: 2, y: 2, fg: ncurses.Colors.Red, at: ncurses.Attributes.Standout});
    }

    sleep(1);
}

