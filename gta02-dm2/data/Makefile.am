EXTRA_DIST = \
	S99dm2			      \
	pointercal		      \
	loopback_earphone.state       \
	loopback_speaker.state 	      \
	play_wav_earphone.state       \
	bru93q.wav 		      \
	loopback_receiver_MIC2.state  \
	play_wav_receiver.state	      \
	dm2.in      		      \
	loopback_receiver.state       \
	play_wav_speaker.state	      \
	gta02_gsm_headset.state       \
	gta02_gsm_receiver.state      \
	gta02_music_headset.state     \
	gta02_music_speaker.state     \
	loopback_earmic_speaker.state \
	test_script		      \
	checkAP.awk 


bin_SCRIPTS = dm2

dm2: dm2.in
	rm -f $@
	sed \
		-e "s|\@bindir\@|$(bindir)|" \
		-e 's|@pkgdatadir@|$(pkgdatadir)|' $(srcdir)/$@.in > $@
	chmod +x $@

resourcedir = $(pkgdatadir)
resource_DATA =  \
		 loopback_earphone.state       \
	         loopback_speaker.state        \
	         play_wav_earphone.state       \
		 bru93q.wav  		       \
		 loopback_receiver_MIC2.state  \
	         play_wav_receiver.state       \
	         loopback_receiver.state       \
		 play_wav_speaker.state	       \
		 gta02_gsm_headset.state       \
	         gta02_gsm_receiver.state      \
	         gta02_music_headset.state     \
	         gta02_music_speaker.state     \
	         loopback_earmic_speaker.state \
		 test_script		       \
		 checkAP.awk 

