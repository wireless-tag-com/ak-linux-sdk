#ifndef __AK_MOTOR_H__
#define __AK_MOTOR_H__

#define AK_MOTOR_IOC_MAGIC 		'm'

/* 1. argument for start up */
/*
 * MOTOR_PARM:			set paramters
 */
#define MOTOR_PARM			_IOW(AK_MOTOR_IOC_MAGIC, 0x00, int)

/* 2. argument for real-time */
/*
 * MOTOR_SPEED_STEP:	set speed in step(hz)
 * MOTOR_SPEED_ANGLE:	set speed in angle
 * */
#define MOTOR_SPEED_STEP	_IOW(AK_MOTOR_IOC_MAGIC, 0x20, int)
#define MOTOR_SPEED_ANGLE	_IOW(AK_MOTOR_IOC_MAGIC, 0x21, int)

/* 3. base command */
/*
 * MOTOR_MOVE_LIMIT:	move clkwise or anticlkwise
 *						clkwise: val > 0, anticlkwise: val < 0
 * MOTOR_MOVE_NOLIMIT:	move clkwise or anticlkwise
 *						clkwise: val > 0, anticlkwise: val < 0
 * MOTOR_STOP:			set stop and wait for be stop finish
 * MOTOR_GET_STATUS:	get status
 * */
#define MOTOR_MOVE_LIMIT	_IOW(AK_MOTOR_IOC_MAGIC, 0x40, int)
#define MOTOR_MOVE_NOLIMIT	_IOW(AK_MOTOR_IOC_MAGIC, 0x41, int)
#define MOTOR_STOP			_IOW(AK_MOTOR_IOC_MAGIC, 0x42, int)
#define MOTOR_GET_STATUS	_IOW(AK_MOTOR_IOC_MAGIC, 0x43, int)

/* 4. extern command */
/*
 * MOTOR_RESET:			first go to ACTIVAL boundary then go to middle
 * MOTOR_MIDDLE:		turn to middle directly
 * MOTOR_CRUISE:		turn to the clkwise an anticlkwise until set stop
 * MOTOR_BOUNDARY:		move to ACTIVAL boundary
 * 						in clkwise or anticlkwise until be stop
 * */
#define MOTOR_RESET			_IOW(AK_MOTOR_IOC_MAGIC, 0x60, int)
#define MOTOR_MIDDLE		_IOW(AK_MOTOR_IOC_MAGIC, 0x61, int)
#define MOTOR_CRUISE		_IOW(AK_MOTOR_IOC_MAGIC, 0x62, int)
#define MOTOR_BOUNDARY		_IOW(AK_MOTOR_IOC_MAGIC, 0x63, int)

/*
 * motor_status:
 * @MOTOR_IS_STOP:		motor is stoped now
 * @MOTOR_IS_RUNNING:	motor is running now
 * */
enum motor_status {
	MOTOR_IS_STOP = 0,
	MOTOR_IS_RUNNING,
};  

/*
 * struct motor_parm - motor parameters
 * @pos:				the current position
 * @speed_step:			speed of step(hz)
 * @steps_one_circel:	steps one circel
 * @total_steps:		steps motor can run
 * @boundary_steps:		reserved boundary steps
 */
struct motor_parm {
	int pos;
	int speed_step;
	int steps_one_circle;
	int total_steps;
	int boundary_steps;
};

/*
 * motor_message - message
 * @status:				motor working status
 * @pos:				the current position
 * @speed_step:			speed of step(hz)
 * @speed_angle:		speed of angle
 * @steps_one_circel:	steps one circel
 * @total_steps:		steps motor can run
 * @boundary_steps:		reserved boundary steps
 * @attach_timer:		attach to hardware timer
 */
struct motor_message {
	enum motor_status status;
	int pos;
	int speed_step;
	int speed_angle;
	int steps_one_circle;
	int total_steps;
	int boundary_steps;
	int attach_timer;
};

/*
 * motor_reset_data - status after reset
 * @total_steps:		total steps
 * @cur_steps:			current step
 */
struct motor_reset_data {
	int total_steps;
	int cur_step;
};

#endif
