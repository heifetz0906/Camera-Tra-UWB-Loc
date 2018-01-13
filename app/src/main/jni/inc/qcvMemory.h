/**=============================================================================

@file
ScratchBuff.h

@brief
Definition of Scratch Buffer class

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved Qualcomm Technologies Proprietary

Export of this technology or software is regulated by the U.S.
Government. Diversion contrary to U.S. law prohibited.

All ideas, data and information contained in or disclosed by
this document are confidential and proprietary information of
Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
By accepting this material the recipient agrees that this material
and the information contained therein are held in confidence and in
trust and will not be used, copied, reproduced in whole or in part,
nor its contents revealed in any manner to others without the express
written permission of Qualcomm Technologies Incorporated.

=============================================================================**/

#ifndef qcv_scratchbuff_h_
#define qcv_scratchbuff_h_

#include <cstddef>

#include "qcvTypes.h"

namespace qcv
{

#define QCV_MEMORY_DEBUG

/////////////////////////////////////////////////////////
// scratch buffer
/////////////////////////////////////////////////////////
class qcvMemory
{
public:

    qcvMemory(); //default constructor with entry buffer

    qcvMemory(int32_t sz, int32_t algn);

    qcvMemory(const qcvMemory& other); //copy 

    virtual ~qcvMemory();

    //set scratch buffere size and alignment
    void setSize(int32_t sz, int32_t algn);

    //researve and return the buffer from the scratch buffer
    void* reserveMem(int32_t length);

    //release memory after use
    void releaseMem(int32_t length);
    
    //reset the buffer count to 0
    void resetMem();
    
    int32_t getSize() const;
    uint32_t getRemainingSize() const;

    qcvMemory &operator=(const qcvMemory& other); // copy assignment

protected: 
    void clear();


protected:
    //scratch buffer
    uint8_t             *buffer;      //buffer    
    int32_t            size;   //buffer size per object
    int32_t            alignment;
    int32_t            count;  //buffer count
#ifdef QCV_MEMORY_DEBUG
    int32_t            maxUsageCount; //a counter for maximum usage of memory 
#endif
};



}   // namespace qcv

#endif // qcv_scratchbuff_h_
//


