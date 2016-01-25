#ifndef SAMPLES_H_
#define SAMPLES_H_

#define MAXNUMBEROFSAMPLES 40
#define MAXSAMPLEINFOLENGTH 100
#define MAXFILENAMELENGTH 20
#define MAXBUTTONIDLENGTH 10

typedef struct {
  char *buttonID;
  char sampleName[MAXFILENAMELENGTH];
} Button;

Button buttons[MAXNUMBEROFSAMPLES]; // Info about buttons

char *sampleFilePaths[MAXNUMBEROFSAMPLES];
#endif
