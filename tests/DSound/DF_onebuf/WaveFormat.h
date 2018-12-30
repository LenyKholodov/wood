struct SWaveFormat
{
	char RIFF[4];
	DWORD Size;
	char WAVE[4];
	char fmt[4];
	DWORD FormatSize;
//	WAVEFORMATEX format;
	WORD wFormatTag;
	WORD wChannels;
	DWORD  dwSamplesPerSec;
	DWORD  dwAvgBytesPerSec;
	WORD wBlockAlign;
	WORD wBitsPerSample;
	char DATA[4];
	DWORD DataSize;
};