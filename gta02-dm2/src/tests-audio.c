#include "dm2.h"

/* in dm2.c */
void audio_path(int path);

static void audio_play(void)
{
	// system("madplay 1.mp3");
	system("aplay /usr/share/dm2/bru93q.wav");
	// system("cat /usr/share/dm2/bru93q.wav > /dev/dsp");
}

static void audio_loopback_earmic_speaker(void)
{
	audio_path(LOOPBACK_EARMIC_SPEAKER);
	//audio_play();
}

static void do_audio_loopback_earmic_speaker_test(void)
{

	do_fork(audio_loopback_earmic_speaker);
	countdown(AUDIO_TEST_TIME , TRUE);
	audio_path(SPEAKER);
}

static void audio_loopback_speaker(void)
{
	audio_path(LOOPBACK_SPEAKER);
	//audio_play();
}

static void do_audio_loopback_speaker_test(void)
{

	do_fork(audio_loopback_speaker);
	countdown(AUDIO_TEST_TIME , TRUE);
	audio_path(SPEAKER);
}

static void audio_loopback_earphone(void)
{
	audio_path(LOOPBACK_EARPHONE);
	//audio_play();
}

static void audio_loopback_receiver(void)
{
	audio_path(LOOPBACK_RECEIVER);
	//audio_play();
}

static void do_audio_loopback_earphone_test(void)
{

	do_fork(audio_loopback_earphone);
	countdown(AUDIO_TEST_TIME , TRUE);
	audio_path(SPEAKER);
}

static void do_audio_loopback_receiver_test(void)
{
	do_fork(audio_loopback_receiver);
	countdown(AUDIO_TEST_TIME , TRUE);
	audio_path(SPEAKER);
}

static void audio_play_speaker(void)
{
	audio_path(SPEAKER);
	audio_play();
}

static void do_audio_speaker_test(void)
{

	do_fork(audio_play_speaker);
	countdown(AUDIO_PLAY_TIME , TRUE);
}

static void audio_play_receiver(void)
{
	audio_path(RECEIVER);
	audio_play();
}

static void do_audio_receiver_test(void)
{

	do_fork(audio_play_receiver);
	countdown(AUDIO_PLAY_TIME , TRUE);
}

static void audio_play_earphone(void)
{
	audio_path(EARPHONE);
	audio_play();
}

static void do_audio_earphone_test(void)
{

	do_fork(audio_play_earphone);
	countdown(AUDIO_TEST_TIME , TRUE);
}


test_t audio_tests[] = {
	{
		"Receiver",
		do_audio_receiver_test,
		FALSE,
		NULL,
		NULL
	},
	{
		"Speaker",
		do_audio_speaker_test,
		FALSE,
		NULL,
		NULL
	},
	/*{
		"Mic->Speaker",
		do_audio_loopback_speaker_test,
		FALSE,
		NULL,
		NULL
	}, */
	/*{
		"Mic->Receiver",
		do_audio_loopback_receiver_test,
		FALSE,
		NULL,
		NULL
	}, */
	{
		"Mic->Earphone",
		do_audio_loopback_earphone_test,
		FALSE,
		NULL,
		NULL
	},
	{
		"EarMic->Speaker",
		do_audio_loopback_earmic_speaker_test,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};
