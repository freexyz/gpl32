void ComAir_DecFrameInit(int iFc,int iDf,int mode, int threshold , short PinCode, short SNR_CHK);
int  ComAir_DecFrameProc(short *Rd_PcmBuf,short VolumeCtrl);

    
void ComAir_DecFrameInit_CH2(int iFc,int iDf,int mode, int threshold , short PinCode, short SNR_CHK);
int  ComAir_DecFrameProc_CH2(short *Rd_PcmBuf,short VolumeCtrl); 