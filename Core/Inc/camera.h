#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "stdint.h"

typedef struct
{
    // 相机的位置坐标x
    uint8_t camera_x;
    // 相机的位置坐标y
    uint8_t camera_y;

} camera_typedef;






void camera_init(camera_typedef *camera);



// 更新相机位置， 更具 selector的位置 和 iteam 的 length
void camera_update(void);

#endif