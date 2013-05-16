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
bzw. Thread, der alles zum Auslösen gebracht hat:
   Beitrag "Brauche Hilfe beim Bau einer Uhr"
   http://www.mikrocontroller.net/topic/156661

   
   
Inhalt des Zip-Files
~~~~~~~~~~~~~~~~~~~~

Dateien in diesem ZIP-Archiv (nicht alphabetisch sortiert):
|
+-- README.txt                  Diese Datei ;)
+-- ChangeLog.txt               Beschreibt die wichtigsten Änderungen
+-- [art]                       Enthält Frontplattendesign
+-+ [AVRootloader]              Enthält Hagen Re's Bootloader
| +-- [AVR]                       Enthält den für die WordClock vorkonfigurieren AVR-Code für den 
| |                               Bootloader (das Binary ist im wcFirmware/out_mega168 - Ordner)
| +-- [Windows]                   Enthält das Flashtool inklusive der vorkonfigurierten ini-Datei
+-- [Schaltplan]                Schaltpläne und Layout zur Wordclock
+-+ [Handbuch]                  Handbuch, sowie Quelldateien
| +-- Handbuch_generic.pdf      Generisches Handbuch
| +-- Handbuch_RGB_Ambi_DCF.pdf Handbuch für RGB Version mit DCF uns Ambilight 
| +-- [Handbuch_Latex]            LaTeX Quellen und Skripte (siehe Abschnitt Handbuch)
+-+ [Software/wcFirmware]       Hauptverzeichnis des Quelcodes mit Projektdateien
  +-- [src]                       der gesamte Source Code mit *.c und *.h files
  +-- [doxy]                      zusätzliche Dateien für Doxygen
  +-+ [out_mega168]               Enthält die Binaries. 
  | +-- AVRootloader.hex            Bootloader (nicht notwendig) siehe auch weiter unten.
  | +-- wc_m168_ger2.hex            Binary für 'alte' 2-sprachige deutsche Front
  | +-- wc_m168_ger3.hex            Binary für aktuelle 3-sprachige deutsche Front
  |                               Zu beachten ist, dass die Dateien der 
  |                               Eindeutigkeit halber umbenannt wurden. 
  |                               Wird die Software neu compiliert, entsteht ein neues Hex-File.
  +-- wordclock.aps               Avr Studio Projekt (wichtigstes Projektfile)
  +-- Doxyfile                    Projektdatei für  Doxygen (Generieren von Dokumentation aus Quellcode)
  +-- WordClock_VS.sln            Visual Stuio Solution
  +-- flash.bat                   zum Flashen aus VS - muss an eigenen Flasher angepasst werden.
  +-- WordClock_VS.vcproj         Visual Stuio Projekt datei
                                  Um aus dem Visual Studio heraus zu compilieren, muss vorher einmal mit
                                  dem AVR-Studio übersetzt werden, um die Make-Files zu generieren.
                                  Um aus dem Studio heraus flashen zu können muss in den Projekt-Settings
                                  folgendes unter Debug eingetragen werden
                                  Command: flash.bat
                                  Command Arguments: m168 out_mega168/wordclock_mega168 WE
                                  ("start without debugging" (Strg+F5) startet dann den Flashvorgang)

Aufbau der WordClock
~~~~~~~~~~~~~~~~~~~~

Die wichtigsten Informationen, sowie eine recht ausführliche Beschreibung 
des Aufbaus dindet sich unter:
   http://www.mikrocontroller.net/articles/Word_Clock

Ein ebenfalls dort verlinkes von bomibob in Stop-Motion gefertigtes Video 
zum Aufbau der Wordclock ist ebenfalls sehr sehenswert.
(http://www.youtube.com/watch?v=OYhtc-8StXA)

Das Benutzerhandbuch zur Inbetriebnahme und Verwendung der
WordClock liegt diesem ZIP-Verzeichnis dabei.

Noch ein Wort zum Flashen mit der Hex-Datei: die Fuse-Werte müssen
auf

  lfuse: 0xE2 hfuse: 0xDC efuse: 0xFD
eingestellt werden, damit die WordClock auch richtig "tickt" ;-)

Ein Schreiben des EEPROM ist nicht notwendig, dieser wird automatisch mit 
Defaultwerten beschrieben, wenn die gespeicherte Struktur nicht in Größe 
und SW-Version mit der Firmware übereinstimmt. 
Dies heißt im Umkehrschluss: 
wenn man im Code nur an den Werten etwas ändert, sollte die Versionsnummer 
verändert werden, oder der EEPROM beim Programmieren gelöscht werden, 
wenn man die EESAVE Fuse gesetzt hat.


Das Handbuch
~~~~~~~~~~~~
Das Handbuch wurde in LaTeX überführt. 
Der große Vorteil ist, dass mit dem kleinen Präprozessor-Perl-Skript, 
das ausgegebene Benutzerhandbuch sich auf die aktuelle Quellcode-Konfiguration 
anpasst (wenn man das möchte).
Stellt man in main.h INDIVIDUAL_CONFIG  auf 1 und Zb MONO_COLOR_CLOCK und DCF_PRESENT auf 1,
dann wird ein Handbuch speziell für die einfarbige Funkuhr-Version erzeugt.

Auch werden im INDIVIDUAL_CONFIG=1 Mode nur die IR-Commandos trainiert, die benötigt werden.

Was wird benötigt?
Eine LaTex-Installation (für Windows am besten MikTex)
avr-gcc ;-)
Perl (Module werden keine benötigt, es reicht also was kleines, wie tinyPerl)
im Perlscript latex_Preprocess.pl ganz oben müssen die Kommandos (ggf mit Pfad) eingetragen werden
my $gcc_cmd   = 'avr-gcc';
my $latex_cmd = 'F:\ootb\editoren\miktex-portable-2.8.3761\miktex\bin\pdflatex.exe';
Will man selbst Text schreiben, ist ein Editor von Vorteil (TexnicCenter ist für Windows mein Favorit).

build4print.bat erzeugt das Handbuch.
build4view.bat erzeugt eine Variante, in der die Links (urls, Inhaltsverzeichnis)
etwas auffälliger dargestellt sind.

Die Adaption funktioniert, indem die Datei srclatexBridge.tex durch das Script bearbeitet 
und anschließend mit dem C-Präprozessor verarbeitet wird.
Das resultierende srclatexBridge_pp.tex wird schließlich von LaTex benutzt 
und enthält bestimmte Variablen (z.B. \newcommand{\WCautoSave}{1}), die die Texterzeugung steuern.
Sollte die Toolchain nicht funktionieren, kann man die Konfiguration srclatexBridge_pp.tex auch
händisch anpassen und nur mit Latex arbeiten. Man sollte aber im Hinterkopf behalten, 
dass durch das Skript die Datei überschrieben wird.

Das ganze ist noch etwas unahandlich - Verbesserungsvorschläge sind gerne gesehen.

Tip:
bei MikTeX in den Optionen den automatischen Paketdownload aktivieren, da sonst der Aufruf 
von Kommanozeile scheitert, wenn bestimmte Pakete noch nicht installiert sind.


Zum Bootloader
~~~~~~~~~~~~~~
Verwendung findet der Bootloader von Hagen Re.
--> http://www.mikrocontroller.net/articles/AVR-Bootloader_mit_Verschl%C3%BCsselung_von_Hagen_Re
Dieser Bootloader zeichnet sich durch einen großen Funktionsumfang, einfache Inbetriebnahme, 
sowie komfortable Benutzung aus.

Hagen Re war so freundlich und hat das OK zur Integration in die WordClock-Auslieferung 
gegeben. Dies hat den Vorteil, dass der fertig konfigurierte Bootloader, 
sowie die Windows vorkonfigurierte Flash-Anwendung beiliegen und nicht extra geladen 
werden müssen. 
Vielen Dank an Hagen für diesen tollen Bootloader!

Die vorkompilierten WordClock-Hexfiles sollten sowohl mit, als auch ohne Bootloader 
funktionionstüchtig sein.
Das senden eines 'R' per UART löst ein Reset (und damit ein Starten des Bootloaders) aus.

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
Viel Spaß, 
wünscht das WordClock-Team
