#include <Windows.h>
#include <stdio.h>

typedef struct _SoundHeader {
		int ID;
		unsigned int Unknown1;
		unsigned int Unknown2;
		unsigned int Always0;
		unsigned int StartPosition;
		unsigned int Length;
		unsigned int Unknown3;
		unsigned int Frequency;
		unsigned char Name[16];
		unsigned char Divider[16];
} SoundHeader, *PSoundHeader;



int main(int argc, char** argv){
	int R;
	const int MaxSounds = 1024;
	SoundHeader AllSounds [MaxSounds] = {0};
	unsigned int BasePosition = 0;
	
	if(argc < 2){
		printf("Demux <sdp file>\n");
		exit(1);
	}
	
	HANDLE SDP = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if((int)SDP == -1){
		printf("E: Failed to open sdp file\n");
		exit(1);
	}
	
	for(int i=0; i<MaxSounds; i++){
		ReadFile(SDP, &(AllSounds[i]), sizeof(SoundHeader), ((LPDWORD) &R), 0);
		if(R != sizeof(SoundHeader)){
			printf("E: Failed to read\n");
			exit(1);
		}
		
		printf("Sound:\n");
		printf("	ID		: %08X (%i)\n", AllSounds[i].ID, AllSounds[i].ID);
		printf("	Unknown1	: %08X (%i)\n", AllSounds[i].Unknown1, AllSounds[i].Unknown1);
		printf("	Unknown2	: %08X (%i)\n", AllSounds[i].Unknown2, AllSounds[i].Unknown2);
		printf("	Always0		: %08X (%i)\n", AllSounds[i].Always0, AllSounds[i].Always0);
		printf("	StartPosition	: %08X (%i)\n", AllSounds[i].StartPosition, AllSounds[i].StartPosition);
		printf("	Length		: %08X (%i)\n", AllSounds[i].Length, AllSounds[i].Length);
		printf("	Unknown3	: %08X (%i)\n", AllSounds[i].Unknown3, AllSounds[i].Unknown3);
		printf("	Frequency	: %08X (%i)\n", AllSounds[i].Frequency, AllSounds[i].Frequency);
		printf("	Name		: %s\n", AllSounds[i].Name);
		printf("\n");
		
		if(AllSounds[i].ID < 0){
			break;
		}
		
		BasePosition += sizeof(SoundHeader);
	}
	
	printf("Base Position: %08X (%i)\n", BasePosition, BasePosition);
	
	char FileName[MAX_PATH];
	for(int i=1; i<MaxSounds; i++){
		SoundHeader *H = &(AllSounds[i]);
		if(H->ID == 0 || H->ID < 0){
			break;
		}
		
		printf("Extracting '%s' ...\n", H->Name);
		int P = BasePosition + H->StartPosition;
		SetFilePointer(SDP, P, 0, FILE_BEGIN);
		
		void* Data = malloc(H->Length);		
		if(Data == 0){
			printf("E: Failed to malloc\n");
			exit(1);
		}		
		
		ReadFile(SDP, Data, H->Length, ((LPDWORD) &R), 0);
		if(R != H->Length){
			printf("E: Failed to read\n");
			exit(1);
		}

/*		
		sprintf(FileName, "%s-%08X_%08X_%08X_%08X_%08X_%08X_%08X_%08X.raw",
			H->Name, H->Unknown1, H->Unknown2, H->Always0, H->Unknown2, H->StartPosition, H->Length, H->Unknown3, H->Frequency);
*/
		sprintf(FileName, "%s.adpcm", H->Name);
		HANDLE RAW = CreateFile(FileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
		if((int)RAW == -1){
			printf("E: Failed to open output\n");
			exit(1);
		}
		WriteFile(RAW, Data, H->Length, ((LPDWORD) &R), 0);
		if(R != H->Length){
			printf("E: Failed to write\n");
			exit(1);
		}
		
		free(Data);
		CloseHandle(RAW);

	}
}

