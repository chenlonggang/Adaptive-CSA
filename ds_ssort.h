/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   Prototypes for the Deep Shallow Suffix Sort routines
   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */ 

#ifdef __cplusplus
extern "C" {
void ds_ssort(unsigned char *t, unsigned long *sa, long n);
int init_ds_ssort(int adist, int bs_ratio);
}
#endif

void ds_ssort(unsigned char *t, unsigned long *sa, long n);
int init_ds_ssort(int adist, int bs_ratio);
