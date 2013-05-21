# How to Export PCB Layout to PDF

In pcb: File -> Export layout... and select "ps".

Select:

+ drill-helper
+ align-marks
+ outline
+ mirror
+ fill-page
+ auto-mirror
+ drill-copper
+ show-legend
+ media: A4

-> OK

Convert postscript file to PDF using:

`$ ps2pdf -sPAPERSIZE=a4 board.ps`

