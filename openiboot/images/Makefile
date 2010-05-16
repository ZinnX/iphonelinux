all:	ConsolePNG.h ConsoleSelectedPNG.h HeaderPNG.h iPhoneOSPNG.h iPhoneOSSelectedPNG.h AndroidOSSelectedPNG.h AndroidOSPNG.h SettingsPNG.h SettingsSelectedPNG.h 1PNG.h 2PNG.h 4PNG.h 6PNG.h 8PNG.h 0PNG.h 10PNG.h 3PNG.h 5PNG.h 7PNG.h 9PNG.h SetHeaderPNG.h SavePNG.h SaveSelectedPNG.h StarPNG.h RStarPNG.h SaveExitPNG.h SaveExitSelectedPNG.h

bin2c:	bin2c.c
	$(CC) bin2c.c -o $@

%PNG.h:	%.png bin2c
	./bin2c data$*PNG < $< > $@

clean:
	-rm *.o
	-rm bin2c
	-rm *.h
