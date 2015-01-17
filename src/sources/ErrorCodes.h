#ifndef bpm_status

#define bpm_status uint8

#define BPM_OK	1

#define BPM_BAD_PARAMETER	2	// something in function call is funky

#define BPM_OPEN_FAIL	3	// File I/O errors
#define BPM_CLOSE_FAIL	4
#define BPM_READ_FAIL	5
#define BPM_WRITE_FAIL	6

#define BPM_BAD_ALLOC	7	// Memory errors
#define BPM_BAD_FREE	8
#define BPM_MAX_IMAGES	9
#define BPM_MAX_LAYERS	10

#define BPM_INTERNAL_ERROR 11	// more general purpose errors
#define BPM_OUT_OF_BOUNDS 12	// a MAX or MIN was reached

#endif