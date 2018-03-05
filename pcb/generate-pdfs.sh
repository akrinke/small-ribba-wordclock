#!/bin/sh

PCB="board.pcb"
PCB_OPTIONS="-x ps --no-align-marks --no-show-legend --scale 1.0 --multi-file --media A4"
PS2PDF_OPTIONS="-sPAPERSIZE=a4"

pcb ${PCB_OPTIONS} --psfile board ${PCB}
pcb ${PCB_OPTIONS} --mirror --psfile board-mirrored ${PCB}

ps2pdf ${PS2PDF_OPTIONS} board-mirrored.bottom.ps wordclock-bottom.pdf
ps2pdf ${PS2PDF_OPTIONS} board.fab.ps wordclock-drill.pdf
ps2pdf ${PS2PDF_OPTIONS} board.topassembly.ps wordclock-topassembly.pdf
ps2pdf ${PS2PDF_OPTIONS} board.bottomassembly.ps wordclock-bottomassembly-180.pdf
qpdf --rotate=180:1 wordclock-bottomassembly-180.pdf wordclock-bottomassembly.pdf

rm board.*.ps board-mirrored.*.ps wordclock-bottomassembly-180.pdf
