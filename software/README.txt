WordClock-Source V0.12  (416)
===================================================================

Projektteam
~~~~~~~~~~~

   Vlad Tepesch (vlad_tepesch)
   Rene H. (promeus)
   Torsten Giese (wawibu)
   Frank M. (ukw)
   Simon Mahler (edimahler)

  
wichtige Ressourcen
~~~~~~~~~~~~~~~~~~~

Artikel
   http://www.mikrocontroller.net/articles/Word_Clock
bzw. Thread, der alles zum Ausl�sen gebracht hat:
   Beitrag "Brauche Hilfe beim Bau einer Uhr"
   http://www.mikrocontroller.net/topic/156661

   
   
Inhalt des Zip-Files
~~~~~~~~~~~~~~~~~~~~

Dateien in diesem ZIP-Archiv (nicht alphabetisch sortiert):
|
+-- README.txt                  Diese Datei ;)
+-- ChangeLog.txt               Beschreibt die wichtigsten �nderungen
+-- [art]                       Enth�lt Frontplattendesign
+-+ [AVRootloader]              Enth�lt Hagen Re's Bootloader
| +-- [AVR]                       Enth�lt den f�r die WordClock vorkonfigurieren AVR-Code f�r den 
| |                               Bootloader (das Binary ist im wcFirmware/out_mega168 - Ordner)
| +-- [Windows]                   Enth�lt das Flashtool inklusive der vorkonfigurierten ini-Datei
+-- [Schaltplan]                Schaltpl�ne und Layout zur Wordclock
+-+ [Handbuch]                  Handbuch, sowie Quelldateien
| +-- Handbuch_generic.pdf      Generisches Handbuch
| +-- Handbuch_RGB_Ambi_DCF.pdf Handbuch f�r RGB Version mit DCF uns Ambilight 
| +-- [Handbuch_Latex]            LaTeX Quellen und Skripte (siehe Abschnitt Handbuch)
+-+ [Software/wcFirmware]       Hauptverzeichnis des Quelcodes mit Projektdateien
  +-- [src]                       der gesamte Source Code mit *.c und *.h files
  +-- [doxy]                      zus�tzliche Dateien f�r Doxygen
  +-+ [out_mega168]               Enth�lt die Binaries. 
  | +-- AVRootloader.hex            Bootloader (nicht notwendig) siehe auch weiter unten.
  | +-- wc_m168_ger2.hex            Binary f�r 'alte' 2-sprachige deutsche Front
  | +-- wc_m168_ger3.hex            Binary f�r aktuelle 3-sprachige deutsche Front
  |                               Zu beachten ist, dass die Dateien der 
  |                               Eindeutigkeit halber umbenannt wurden. 
  |                               Wird die Software neu compiliert, entsteht ein neues Hex-File.
  +-- wordclock.aps               Avr Studio Projekt (wichtigstes Projektfile)
  +-- Doxyfile                    Projektdatei f�r  Doxygen (Generieren von Dokumentation aus Quellcode)
  +-- WordClock_VS.sln            Visual Stuio Solution
  +-- flash.bat                   zum Flashen aus VS - muss an eigenen Flasher angepasst werden.
  +-- WordClock_VS.vcproj         Visual Stuio Projekt datei
                                  Um aus dem Visual Studio heraus zu compilieren, muss vorher einmal mit
                                  dem AVR-Studio �bersetzt werden, um die Make-Files zu generieren.
                                  Um aus dem Studio heraus flashen zu k�nnen muss in den Projekt-Settings
                                  folgendes unter Debug eingetragen werden
                                  Command: flash.bat
                                  Command Arguments: m168 out_mega168/wordclock_mega168 WE
                                  ("start without debugging" (Strg+F5) startet dann den Flashvorgang)

Aufbau der WordClock
~~~~~~~~~~~~~~~~~~~~

Die wichtigsten Informationen, sowie eine recht ausf�hrliche Beschreibung 
des Aufbaus dindet sich unter:
   http://www.mikrocontroller.net/articles/Word_Clock

Ein ebenfalls dort verlinkes von bomibob in Stop-Motion gefertigtes Video 
zum Aufbau der Wordclock ist ebenfalls sehr sehenswert.
(http://www.youtube.com/watch?v=OYhtc-8StXA)

Das Benutzerhandbuch zur Inbetriebnahme und Verwendung der
WordClock liegt diesem ZIP-Verzeichnis dabei.

Noch ein Wort zum Flashen mit der Hex-Datei: die Fuse-Werte m�ssen
auf

  lfuse: 0xE2 hfuse: 0xDC efuse: 0xFD
eingestellt werden, damit die WordClock auch richtig "tickt" ;-)

Ein Schreiben des EEPROM ist nicht notwendig, dieser wird automatisch mit 
Defaultwerten beschrieben, wenn die gespeicherte Struktur nicht in Gr��e 
und SW-Version mit der Firmware �bereinstimmt. 
Dies hei�t im Umkehrschluss: 
wenn man im Code nur an den Werten etwas �ndert, sollte die Versionsnummer 
ver�ndert werden, oder der EEPROM beim Programmieren gel�scht werden, 
wenn man die EESAVE Fuse gesetzt hat.


Das Handbuch
~~~~~~~~~~~~
Das Handbuch wurde in LaTeX �berf�hrt. 
Der gro�e Vorteil ist, dass mit dem kleinen Pr�prozessor-Perl-Skript, 
das ausgegebene Benutzerhandbuch sich auf die aktuelle Quellcode-Konfiguration 
anpasst (wenn man das m�chte).
Stellt man in main.h INDIVIDUAL_CONFIG  auf 1 und Zb MONO_COLOR_CLOCK und DCF_PRESENT auf 1,
dann wird ein Handbuch speziell f�r die einfarbige Funkuhr-Version erzeugt.

Auch werden im INDIVIDUAL_CONFIG=1 Mode nur die IR-Commandos trainiert, die ben�tigt werden.

Was wird ben�tigt?
Eine LaTex-Installation (f�r Windows am besten MikTex)
avr-gcc ;-)
Perl (Module werden keine ben�tigt, es reicht also was kleines, wie tinyPerl)
im Perlscript latex_Preprocess.pl ganz oben m�ssen die Kommandos (ggf mit Pfad) eingetragen werden
my $gcc_cmd   = 'avr-gcc';
my $latex_cmd = 'F:\ootb\editoren\miktex-portable-2.8.3761\miktex\bin\pdflatex.exe';
Will man selbst Text schreiben, ist ein Editor von Vorteil (TexnicCenter ist f�r Windows mein Favorit).

build4print.bat erzeugt das Handbuch.
build4view.bat erzeugt eine Variante, in der die Links (urls, Inhaltsverzeichnis)
etwas auff�lliger dargestellt sind.

Die Adaption funktioniert, indem die Datei srclatexBridge.tex durch das Script bearbeitet 
und anschlie�end mit dem C-Pr�prozessor verarbeitet wird.
Das resultierende srclatexBridge_pp.tex wird schlie�lich von LaTex benutzt 
und enth�lt bestimmte Variablen (z.B. \newcommand{\WCautoSave}{1}), die die Texterzeugung steuern.
Sollte die Toolchain nicht funktionieren, kann man die Konfiguration srclatexBridge_pp.tex auch
h�ndisch anpassen und nur mit Latex arbeiten. Man sollte aber im Hinterkopf behalten, 
dass durch das Skript die Datei �berschrieben wird.

Das ganze ist noch etwas unahandlich - Verbesserungsvorschl�ge sind gerne gesehen.

Tip:
bei MikTeX in den Optionen den automatischen Paketdownload aktivieren, da sonst der Aufruf 
von Kommanozeile scheitert, wenn bestimmte Pakete noch nicht installiert sind.


Zum Bootloader
~~~~~~~~~~~~~~
Verwendung findet der Bootloader von Hagen Re.
--> http://www.mikrocontroller.net/articles/AVR-Bootloader_mit_Verschl%C3%BCsselung_von_Hagen_Re
Dieser Bootloader zeichnet sich durch einen gro�en Funktionsumfang, einfache Inbetriebnahme, 
sowie komfortable Benutzung aus.

Hagen Re war so freundlich und hat das OK zur Integration in die WordClock-Auslieferung 
gegeben. Dies hat den Vorteil, dass der fertig konfigurierte Bootloader, 
sowie die Windows vorkonfigurierte Flash-Anwendung beiliegen und nicht extra geladen 
werden m�ssen. 
Vielen Dank an Hagen f�r diesen tollen Bootloader!

Die vorkompilierten WordClock-Hexfiles sollten sowohl mit, als auch ohne Bootloader 
funktionionst�chtig sein.
Das senden eines 'R' per UART l�st ein Reset (und damit ein Starten des Bootloaders) aus.

Verwednung des Bootloaders:
- setzten der BOOTRST-Fuse 
  Damit ergibt sich folgende Konfiguration der Fuses:
    lfuse: 0xE2 hfuse: 0xDC efuse: 0xFC
- flashen des AVRootloader-hexfile auf bisherigem Weg
- Starten der AVRootloader.exe
- Auswahl des Com-Ports
- Baudrate auf 9600
- Sign auf WCMB (WordClock Main Board)
- "Connect to device" -> Button wird zu "Disconnect"
- Auswahl des Hexfiles
- Klick auf Program
  
Das Tool ist so konfiguiert, dass es von sich aus ein 'R' schickt, 
um die WordClock zu resetten und den Bootloader zu betreten.

  
  

So, dass war das wichtigste.
Viel Spa�, 
w�nscht das WordClock-Team
