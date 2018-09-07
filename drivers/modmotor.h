#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/builtin.h"
#include <stdio.h>
#include <pbio/motorcontrol.h>
#include <pbio/motor.h>

// Class structure for DC Motors
typedef struct _motor_DCMotor_obj_t {
    mp_obj_base_t base;
    pbio_port_t port;
    pbio_motor_dir_t direction;
} motor_DCMotor_obj_t;

// Class structure for Encoded Motors
typedef struct _motor_EncodedMotor_obj_t {
    mp_obj_base_t base;
    uint8_t port;
    pbio_motor_dir_t direction;
    float_t gear_ratio;
} motor_EncodedMotor_obj_t;

extern const mp_obj_type_t motor_DCMotor_type;
extern const mp_obj_type_t motor_EncodedMotor_type;
