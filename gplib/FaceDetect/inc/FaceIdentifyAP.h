#ifndef __FACE_IDENTIFY_AP_H__
#define __FACE_IDENTIFY_AP_H__

#include "define.h"

/**
* @brief Calculate needed memory
* @return needed memory size for face identification
*/
int FaceIdentify_MemCalc(void);

/**
* @brief Train user's feature
* @param[in] gray: captured image
* @param[in] ownerULBP: ulbp of the owner
* @param[in] train_i: index of the training data
* @param[in] fiMem: the allocated memory and the assigned memory address for this func
* @param[in] fdMemsize: memory needed for face detection
* @param[in] userROI[0], faceROI: detected face region
* @param[in] userROI[1], lEyeROI: detected left eye region
* @param[in] userROI[2], rEyeROI: detected right eye region
*/
void FaceIdentify_Train(const gpImage* gray, const gpRect* userROI, void* _ownerULBP, const int train_i, void* fiMem);

/**
* @brief Verify user's identity
* @param[in] gray: captured image
* @param[in] ownerULBP: ulbp of the owner
* @param[in] scoreThreshold: threshold of confidence score
* @param[in] fiMem: the allocated memory and the assigned memory address for this func
* @param[in] userROI[0], faceROI: detected face region
* @param[in] userROI[1], lEyeROI: detected left eye region
* @param[in] userROI[2], rEyeROI: detected right eye region
* @return 1 if the user is classified as a genuine user, 0 if classified as imposter
*/
int FaceIdentify_Verify(gpImage* gray, const gpRect* userROI, void* ownerULBP, int scoreThreshold, void* fiMem);

#endif