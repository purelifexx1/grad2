/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tim.h"
#include "math.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "dma.h"
#include "usart.h"
#include "common_def.h"
#include "ringbuffer.h"
#include "robot_scara.h"
#include "robot_lowlayer.h"
#include "command_respond.h"
#include "kinematic.h"
#include "communicate_payload.h"
#include "usbd_cdc_if.h"
#include "fsmc.h"
#include "gpio.h"
#include "gcode_handler.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

extern RINGBUFFER_TypeDef 			usb_rx_ringbuff;
extern RINGBUFFER_TypeDef 			cmd_tx_ringbuff;
extern const char 					*DETAIL_STATUS[NUM_OF_STATUS];
extern SCARA_PositionTypeDef		positionPrevios;
extern SCARA_PositionTypeDef		positionCurrent;
extern SCARA_PositionTypeDef		positionNext;
//extern SCARA_PositionTypeDef		positionTrue;
extern SCARA_Gcode_Cor_TypeDef		Gcode_Cor[1000];
extern int32_t						current_key_speed1 = 1;
extern double						up_z_height;
extern double 						down_z_height;
extern int32_t						total_num_of_point;
extern uint8_t						pnp_move_option = CMD_MOVE_LINE;
extern uint8_t 						continuous_update = 0;
extern uint8_t 						object_tail_pointer = 0;
extern uint8_t 						object_head_pointer = 0;
extern SCARA_Pick_And_Place_Package Object[8];
double *testing_array;
extern TIM_HandleTypeDef htim7;
osMailQId commandMailHandle;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId USB_RX_Check_Handle;
osMutexId usbTxMutexHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void Start_USB_RX_Task(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* definition and creation of usbTxMutex */
  osMutexDef(usbTxMutex);
  usbTxMutexHandle = osMutexCreate(osMutex(usbTxMutex));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
	  /* Create the queue(s) */
	  /* definition and creation of commandMail */
	  osMailQDef(commandMail, 1, DUTY_Command_TypeDef);
	  commandMailHandle = osMailCreate(osMailQ(commandMail), NULL);

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityHigh, 0, 2060);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of USB_RX_Check_ */
  osThreadDef(USB_RX_Check_, Start_USB_RX_Task, osPriorityNormal, 0, 1024);
  USB_RX_Check_Handle = osThreadCreate(osThread(USB_RX_Check_), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  HAL_GPIO_WritePin(USB_SIGN_GPIO_Port, USB_SIGN_Pin, GPIO_PIN_SET); // Pull-up Resistor

  osEvent 				ret_mail;
  DUTY_Command_TypeDef 	duty_cmd;
  DUTY_Command_TypeDef 	*dataMail;
  uint8_t 				isNewDuty = FALSE;


  // Report buffer;
  uint8_t				update_pos_counter = 0;
  int8_t 				test_value[4];
  uint8_t				respond[80];
  int32_t				respond_lenght;
  //uint8_t				position[135];
  //uint8_t				infor[145];
//  int32_t				infor_lenght;
//  uint8_t				task_usb[150];
//  int32_t				task_usb_lenght;
  int32_t 				total_respond_length;
//  int32_t 				detail_ptr;
  uint8_t				detail_array[80];

  uint8_t 				testing_value = 0;
//  double 				value1 = 0;
//  double 				value2 = 0;
//  double  			value3 = 0;
//  uint8_t				respond_packed[50];
//  int32_t				respond_packed_lenght;
  //uint8_t				infor_packed[150];
  //int32_t				infor_packed_lenght;
  //uint8_t				usb_buff[350];
  //int32_t				usb_lenght;
  // Debug variables
  //uint32_t total_pulse = 0;

  // Robot variables
  SCARA_MethodTypeDef		current_method;
  SCARA_ModeTypeDef			current_mode;
  SCARA_DutyStateTypeDef 	current_duty_state;
  SCARA_ScanStateTypeDef	current_scan_state;
  SCARA_KeyStateTypeDef		current_key_state;
  SCARA_KeyTypeDef			current_key;
  SCARA_PositionTypeDef 	update_position;
  int32_t					current_key_speed;
  SCARA_MethodChangeState	method_state = SCARA_METHOD_IDLE;
  double						run_time;


  SCARA_Pick_And_Place_State  operation_state;

  double state_time = 0;
  int run_point = 0;
  const SCARA_Slot_TypeDef SLot_Cordinate[NUM_OF_OBJECT] = {
  		{254.112f, -137.7f, 0.0f} ,
  		{256.021f, -107.46f, 0.0f} ,
  		{257.2121f, -76.9947f, 0.0f} ,
  		{258.221f, -47.1851f, 0.0f} ,
		{260.695f, -17.075f, 0.0f} ,
		{260.4f, 13.66f, 0.0f}
  };
  const double placement_spacing = -34.0f;
  uint8_t Slot_Placement[NUM_OF_OBJECT] = {
		  0, 0, 0, 0, 0, 0
  };

  LOG_REPORT("free_rtos.c: PROGRAM START...", __LINE__);

  // Init value
  current_method = scaraGetMethod();
  current_mode	 = scaraGetMode();
  current_duty_state	 = scaraGetDutyState();

  // Start up robot
  scaraStartup();
  osDelay(10);

//   positionNext.Theta1 = -PI/3;
//   positionNext.Theta2 = PI/3;
//   positionNext.D3 = 10;
//   positionNext.Theta4 = 0;
//   positionNext.t = 0;

  positionNext.Theta1 = -1.4345;
  positionNext.Theta2 = 1.7213f;
  positionNext.D3 = 9.826;
  positionNext.Theta4 = 0.28676;
  positionNext.t = 0;

  kinematicForward(&positionNext);
  /* Infinite loop */
//Start Timer 7
	  HAL_TIM_Base_Start_IT(&htim7);

  for(;;)
  {
	  /*---------Wait for Timer Trigger-----------*/
	  osSignalWait(0x01, osWaitForever); // Very Important
	  /* 1--- Reset Value ---*/
	  respond_lenght		= 0;
//	  respond_packed_lenght = 0;
//	  infor_lenght			= 0;
//	  infor_packed_lenght	= 0;
	  //task_usb_lenght		= 0;
	  //usb_lenght			= 0;
	  total_respond_length  = 0;
//	  detail_ptr 			= 0;
	  // Update new position
//	  memcpy(&positionPrevios, &positionCurrent, sizeof(SCARA_PositionTypeDef));
	  memcpy(&positionCurrent, &positionNext, sizeof(SCARA_PositionTypeDef));

	  /* 2--- Check New Duty Phase ---*/
	  // Check mail
	  ret_mail = osMailGet(commandMailHandle, 0);
	  if ( ret_mail.status == osEventMail) {
		   dataMail = ret_mail.value.p;
		   memcpy( &duty_cmd, dataMail, sizeof(DUTY_Command_TypeDef));
		   isNewDuty = TRUE;
		   osMailFree(commandMailHandle, dataMail);/* free memory allocated for mail */
	  }
	  if(isNewDuty) {
		  if (duty_cmd.change_method == TRUE && method_state == SCARA_METHOD_IDLE) {
			  if (SCARA_METHOD_MANUAL == duty_cmd.robot_method) {
				  // Need add check condition idle in each method
				  current_method = SCARA_METHOD_MANUAL;
				  current_duty_state = SCARA_DUTY_STATE_READY;
				  detail_array[0] = MANUAL_METHOD;
				  respond_lenght = commandRespond(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
				total_respond_length += respond_lenght;
			  } else if (SCARA_METHOD_SEMI_AUTO == duty_cmd.robot_method) {
				  current_method = SCARA_METHOD_SEMI_AUTO;
				  current_duty_state = SCARA_DUTY_STATE_READY;
				  detail_array[0] = SEMI_AUTO_METHOD;
				  respond_lenght = commandRespond(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
				  total_respond_length += respond_lenght;
			  } else if (SCARA_METHOD_GCODE == duty_cmd.robot_method) {
				  current_duty_state = SCARA_DUTY_STATE_INIT;
				  current_method = SCARA_METHOD_GCODE;
				  detail_array[0] = GCODE_METHOD;
				  respond_lenght = commandRespond(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
				  total_respond_length += respond_lenght;
			  }else if(SCARA_METHOD_TEST == duty_cmd.robot_method){
				  test_value[0] = 0;
				  test_value[1] = 0;
				  test_value[2] = 0;
				  test_value[3] = 0;
				  current_method = SCARA_METHOD_TEST;
				  current_duty_state = SCARA_DUTY_STATE_READY;
				  detail_array[0] = TEST_METHOD;
				  respond_lenght = commandRespond(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
				  total_respond_length += respond_lenght;
			  }else if(SCARA_METHOD_PICK_AND_PLACE == duty_cmd.robot_method){
				  current_method = SCARA_METHOD_PICK_AND_PLACE;
				  current_duty_state = SCARA_DUTY_STATE_INIT;
				  detail_array[0] = PICK_AND_PLACE_METHOD;
				  respond_lenght = commandRespond(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
				  total_respond_length += respond_lenght;
			  }
		  } else if(duty_cmd.change_method == FALSE && method_state == SCARA_METHOD_IDLE) {
			  	  // Check current method & cmd method
			  	  if (current_method == duty_cmd.robot_method) {
				  	  switch( duty_cmd.robot_method) {
				  	  case SCARA_METHOD_MANUAL:
				  	  {
				  		  if (current_key_state == SCARA_KEY_STATE_READY) {
				  			  current_key = duty_cmd.keyboard;
				  			  current_key_state = SCARA_KEY_STATE_INIT;// Init new path
				  			  current_key_speed = duty_cmd.key_speed;
				  		  } else if (current_key == duty_cmd.keyboard
				  				  	  && current_key_state == SCARA_KEY_STATE_FLOW
									  && current_key_speed == duty_cmd.key_speed) {
				  			  current_key_state = SCARA_KEY_STATE_INIT;// Continue old path
				  		  }
				  	  }
				  	  break;
				  	  case SCARA_METHOD_SEMI_AUTO:
				  	  {
						  switch( duty_cmd.robot_mode) {
						  case SCARA_MODE_STOP:
							  {
								  current_mode	 = SCARA_MODE_STOP;
								detail_array[0] = STOP_NOW;
								respond_lenght = commandRespond(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
								total_respond_length += respond_lenght;
//								  LOG_REPORT("ROBOT STOP !!!", __LINE__);
							  }
							  break;

						  case SCARA_MODE_SCAN:
							  {
								  if (SCARA_MODE_DUTY == current_mode && SCARA_DUTY_STATE_READY == current_duty_state) {
									  current_mode = SCARA_MODE_SCAN;
									  current_scan_state = SCARA_SCAN_STATE_INIT;
									detail_array[0] = START_SCAN;
									respond_lenght = commandRespond(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
									total_respond_length += respond_lenght;
								  } else {
									detail_array[0] = BUSY;
									respond_lenght = commandRespond(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
									total_respond_length += respond_lenght;
									  LOG_REPORT("SCAN FAIL: BUSY", __LINE__);
								  }
							  }
							  break;

						  case SCARA_MODE_DUTY:
							  {
								  if (SCARA_MODE_DUTY == current_mode && SCARA_DUTY_STATE_READY == current_duty_state) {
									  if (scaraIsScanLimit()) {
										  current_mode	 = SCARA_MODE_DUTY;
										  current_duty_state	 = SCARA_DUTY_STATE_INIT;
									  } else {
										detail_array[0] = NOT_SCAN;
										respond_lenght = commandRespond(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
										total_respond_length += respond_lenght;
										  LOG_REPORT("MOVE FAIL:NOT SCAN", __LINE__);
									  }
								  } else {
									detail_array[0] = BUSY;
									respond_lenght = commandRespond(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
									total_respond_length += respond_lenght;
//									  LOG_REPORT("MOVE FAIL:BUSY", __LINE__);
								  }
							  }
							  break;
						  default:
							  {
								  LOG_REPORT("CMD Error Mode !!!", __LINE__);
							  }
						  }
					  }
					  break;
					  case SCARA_METHOD_TEST:
					  {
//						  test_value[0] = 0;
//						  test_value[1] = 0;
//						  test_value[2] = 0;
//						  test_value[3] = 0;
						  current_duty_state = SCARA_DUTY_STATE_FLOW;
						  switch (duty_cmd.test_key)
						  {
						  case SCARA_TEST_MOTOR1_POS:
							 test_value[duty_cmd.test_key/2] = test_value_array[0];
						  break;
						  case SCARA_TEST_MOTOR1_NEG:
							 test_value[duty_cmd.test_key/2] = -test_value_array[0];
						  break;
						  case SCARA_TEST_MOTOR2_POS:
							 test_value[duty_cmd.test_key/2] = test_value_array[1];
						  break;
						  case SCARA_TEST_MOTOR2_NEG:
							 test_value[duty_cmd.test_key/2] = -test_value_array[1];
						  break;
						  case SCARA_TEST_MOTOR3_POS:
							 test_value[duty_cmd.test_key/2] = test_value_array[2];
						  break;
						  case SCARA_TEST_MOTOR3_NEG:
							 test_value[duty_cmd.test_key/2] = -test_value_array[2];
						  break;
						  case SCARA_TEST_MOTOR4_POS:
							 test_value[duty_cmd.test_key/2] = test_value_array[3];
						  break;
						  case SCARA_TEST_MOTOR4_NEG:
							 test_value[duty_cmd.test_key/2] = -test_value_array[3];
						  break;						  
						  default:
							  current_duty_state = SCARA_DUTY_STATE_READY;
						  break;
						  }
					  }
					  break;
				  	  case SCARA_METHOD_GCODE:
				  	  {
				  		  if(current_duty_state == SCARA_DUTY_STATE_READY && duty_cmd.id_command == CMD_GCODE_RUN && Gcode_data_available && offset_data_available){
				  			  if(Gcode_Mode == GCODE_LINEAR){
				  				run_point = 1;
				  			  }else if(Gcode_Mode == GCODE_SMOOTH_LSPB){
				  				run_point = 0;
				  			  }
				  			detail_array[0] = GCODE_START;
							respond_lenght = commandRespond(RPD_START, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
							total_respond_length += respond_lenght;
				  			current_duty_state = SCARA_DUTY_STATE_OPERATION;
				  		  }else if(current_duty_state == SCARA_DUTY_STATE_INIT){
				  			detail_array[0] = GCODE_MODE_NOT_READY;
							respond_lenght = commandRespond(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
							total_respond_length += respond_lenght;
				  		  }else if(duty_cmd.id_command == CMD_GCODE_STOP){
				  			  current_duty_state = SCARA_DUTY_STATE_INIT;
				  		  }else if(Gcode_data_available == 0){
				  			detail_array[0] = GCODE_DATA_MISSING;
						    respond_lenght = commandRespond(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
						    total_respond_length += respond_lenght;
				  		  }else if(offset_data_available == 0){
				  			detail_array[0] = GCODE_OFFSET_MISSING;
							respond_lenght = commandRespond(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
							total_respond_length += respond_lenght;
				  		  }
				  	  }
				  	  break;
				  	  case SCARA_METHOD_PICK_AND_PLACE:{
//				  		  memcpy(&Object[object_head_pointer].object_position, &duty_cmd.target_point, sizeof(SCARA_PositionTypeDef));
//				  		  Object[object_head_pointer].timer_value = duty_cmd.target_point.packet_time_stamp;
//				  		  object_head_pointer = (object_head_pointer+1)%8;
//				  		  detail_array[0] = OBJECT_DETECTED;
//				  		  detail_array[1] = duty_cmd.target_point.object_type;
//						  respond_lenght = commandRespond1(RPD_OK, duty_cmd.id_command, detail_array, 2, &respond[total_respond_length]);
//						  total_respond_length += respond_lenght;
				  	  }
				  	  break;
				  	  default:
				  	  {
//				  		  LOG_REPORT("CMD Error Method !!!", __LINE__);
				  	  }
				  	  }
			  	  } else {
					detail_array[0] = INCORRECT_METHOD;
					respond_lenght = commandRespond(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
					total_respond_length += respond_lenght;
			  	  }

			  }else{
				  detail_array[0] = BUSY;
				  respond_lenght = commandRespond(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
				  total_respond_length += respond_lenght;
			  }
		  isNewDuty = FALSE;
	  }

	  /* 3--- Execute Phase ---*/
	  switch(current_method) {
	  case SCARA_METHOD_MANUAL:
	  {
		  switch( current_key_state) {
		  case SCARA_KEY_STATE_READY:
			  method_state = SCARA_METHOD_IDLE;
			  /* Wait for application keyboard , do nothing*/
		  break;
		  case SCARA_KEY_STATE_INIT:
		  {
			  method_state = SCARA_METHOD_ACTIVE;
			  if (scaraKeyInit(current_key, current_key_speed1) == SCARA_STATUS_OK) {
				  run_time = 0;
				  current_key_state = SCARA_KEY_STATE_FLOW;
					detail_array[0] = NONE;
					respond_lenght = commandRespond(RPD_START, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
					total_respond_length += respond_lenght;
			  } else {
				  current_key_state = SCARA_KEY_STATE_READY;
			  }
		  }
		  break;
		  case SCARA_KEY_STATE_FLOW:
		  {
			  method_state = SCARA_METHOD_ACTIVE;
			  SCARA_StatusTypeDef status;
			  // Increase run time
			  run_time += T_SAMPLING;
			  // Check Time Out
			  if (scaraIsFinish(run_time)) {
				  current_key_state = SCARA_KEY_STATE_FINISH;// Key Done
			  } else {
				  //status = scaraKeyFlow(run_time, &positionNext, positionCurrent);
				  status = scaraFlowDuty(run_time, &positionNext, positionCurrent);
				  if ( SCARA_STATUS_OK == status) {
					  lowlayer_computeAndWritePulse(positionCurrent, positionNext);
				  } else {
					  current_key_state = SCARA_KEY_STATE_FINISH;
					detail_array[0] = status;
					respond_lenght = commandRespond(RPD_STOP, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
					total_respond_length += respond_lenght;
					  LOG_REPORT("STOP KEY", __LINE__);
				  }
			  }
		  }
		  break;
		  case SCARA_KEY_STATE_FINISH:
		  {
			  lowlayer_readTruePosition(&positionNext);
			  kinematicForward(&positionNext);
			  current_key_state = SCARA_KEY_STATE_READY;
			  positionNext.t = 0;
			  positionNext.total_time = 0;
			  positionNext.q = 0;
		  }
		  break;
		  }
	  }
	  break;

	  case SCARA_METHOD_SEMI_AUTO:
	  {
		  switch( current_mode) {
		  case SCARA_MODE_STOP:
			  {
				  current_mode 	= SCARA_MODE_DUTY;
				  current_duty_state = SCARA_DUTY_STATE_READY;
			  }
		  break;

		  case SCARA_MODE_SCAN:
			  {
				  method_state = SCARA_METHOD_ACTIVE;
				  switch (current_scan_state) {
				  case SCARA_SCAN_STATE_INIT:
					  {
						  lowlayer_scanReset();
						  current_scan_state = SCARA_SCAN_STATE_HARD;
					  }
					  break;
				  case SCARA_SCAN_STATE_HARD:
					  {
						  if(lowlayer_scanFlow()) {
							  current_scan_state = SCARA_SCAN_STATE_SOFT;
						  }
					  }
					  break;
				  case SCARA_SCAN_STATE_SOFT:
					  {
						  if(lowlayer_goToSoftLimit(&positionNext)) {
							  current_scan_state = SCARA_SCAN_STATE_FINISH;
							}
					  }
					  break;
				  case SCARA_SCAN_STATE_FINISH:
					  {
						  //lowlayer_readSetPosition(&positionNext);
						  lowlayer_readTruePosition(&positionNext);
						  kinematicForward(&positionNext);
						  current_mode 	= SCARA_MODE_DUTY;
						  current_duty_state = SCARA_DUTY_STATE_READY;
						  scaraSetScanFlag();

						detail_array[0] = NONE;
						respond_lenght = commandRespond(RPD_DONE, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
						total_respond_length += respond_lenght;
					  }
					  break;
				  default:
					  {
						  LOG_REPORT("ERROR STATE !!!", __LINE__);
						  while(1);
					  }
				  }

			  }
		  break;

		  case SCARA_MODE_DUTY:
			  {
				  switch (current_duty_state) {
				  case SCARA_DUTY_STATE_READY:
					  {
						  // Do nothing;
						  method_state = SCARA_METHOD_IDLE;
					  }
				  break;

				  case SCARA_DUTY_STATE_INIT:
					  {
						  method_state = SCARA_METHOD_ACTIVE;
						  SCARA_StatusTypeDef status1, status2;
						  status1 = scaraInitDuty(duty_cmd);
						  if ( SCARA_STATUS_OK == status1) {
							  status2 = scaraTestDuty();
							  if (SCARA_STATUS_OK == status2) {
							  current_duty_state = SCARA_DUTY_STATE_FLOW;
							  //current_duty_state = SCARA_DUTY_STATE_FINISH;
							  run_time			= 0;
							  // Respond
//							  detail_array[0] = status1;
//							  respond_lenght = commandRespond1(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
//							  total_respond_length += respond_lenght;
							  detail_array[0] = NONE;
							  respond_lenght = commandRespond(RPD_START, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
							  total_respond_length += respond_lenght;

							  } else {
								  current_duty_state 	= SCARA_DUTY_STATE_READY;

								  //LOG_REPORT("TEST FAIL", __LINE__);
								detail_array[0] = status2;
								respond_lenght = commandRespond(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
								total_respond_length += respond_lenght;
							  }
						  } else {
							  current_duty_state 	= SCARA_DUTY_STATE_READY;

							detail_array[0] = status1;
							respond_lenght = commandRespond(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
							total_respond_length += respond_lenght;
							  LOG_REPORT("INIT FAIL", __LINE__);
						  }
					  }
				  break;

				  case SCARA_DUTY_STATE_FLOW:
					  {
						  method_state = SCARA_METHOD_ACTIVE;
						  SCARA_StatusTypeDef status;
						  // Increase run time
						  run_time += T_SAMPLING;
						  // Check Time Out
						  if (scaraIsFinish(run_time)) {
							  current_duty_state = SCARA_DUTY_STATE_FINISH;// Work Done
						  } else {
							  status = scaraFlowDuty(run_time , &positionNext, positionCurrent);
							  if ( SCARA_STATUS_OK == status) {
								  lowlayer_computeAndWritePulse(positionCurrent, positionNext);

							  } else {
								  current_duty_state = SCARA_DUTY_STATE_FINISH;

								detail_array[0] = status;
								respond_lenght = commandRespond(RPD_STOP, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
								total_respond_length += respond_lenght;
								  LOG_REPORT("STOP DUTY", __LINE__);
							  }
						  }
					  }
				  break;

				  case SCARA_DUTY_STATE_FINISH:
					  {
						  lowlayer_readTruePosition(&positionNext);
						  kinematicForward(&positionNext);
						  current_duty_state = SCARA_DUTY_STATE_READY;
//						  positionNext.t = 0;
//						  positionNext.total_time = 0;
//						  positionNext.q = 0;


						  detail_array[0] = NONE;
						  respond_lenght = commandRespond(RPD_DONE, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
						  total_respond_length += respond_lenght;
					  }
				  break;

				  default:
					  {
						  LOG_REPORT("ERROR STATE !!!", __LINE__);
						  while(1);
					  }
				  }

			  }
			  break;

		  default:
			  {
				  LOG_REPORT("ERROR MODE !!!", __LINE__);
				  while(1);
			  }
		  }
	  }
	  break;

	  case SCARA_METHOD_GCODE:
	  {
		  switch (current_duty_state)
		  {
		  case SCARA_DUTY_STATE_INIT:{
			  method_state = SCARA_METHOD_IDLE;
			  if(fabs(positionCurrent.x - offset_x) < 4 && fabs(positionCurrent.y - offset_y) < 4 && fabs(positionCurrent.z - offset_z) < 4){
				  current_duty_state = SCARA_DUTY_STATE_READY;
			  }
		  }
		  break;
		  case SCARA_DUTY_STATE_READY:{
			  // do nothing wait for hot key
			  method_state = SCARA_METHOD_IDLE;
		  }
		  break;
		  case SCARA_DUTY_STATE_OPERATION:{
			  method_state = SCARA_METHOD_ACTIVE;
			  update_gcode_point(&duty_cmd, run_point);
			  testing_value = 1;
			  SCARA_StatusTypeDef status;
			  status = scaraInitDuty(duty_cmd);
			  if(status == SCARA_STATUS_OK){
				  if(duty_cmd.trajec_type == DUTY_TRAJECTORY_LINEAR || duty_cmd.trajec_type == DUTY_TRAJECTORY_LSPB){
					  run_time = 0;
				  }else if(duty_cmd.trajec_type == DUTY_TRAJECTORY_GCODE_LSPB){
					  run_time = last_T;
				  }
				  current_duty_state = SCARA_DUTY_STATE_FLOW;
				  detail_array[0] = (uint8_t)((run_point+1) * 100.0f / total_num_of_point );
				  respond_lenght = commandRespond(RDP_GCODE_PROCESS, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
				  total_respond_length += respond_lenght;
			  }else{
				  current_duty_state = SCARA_DUTY_STATE_FINISH;
				  detail_array[0] = status;
				  respond_lenght = commandRespond(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
				  total_respond_length += respond_lenght;
				  break;
			  }
		  }
		  //break;

		  case SCARA_DUTY_STATE_FLOW:{
			  method_state = SCARA_METHOD_ACTIVE;
			  run_time += T_SAMPLING;
			  // Check Time Out
			  if (scaraIsFinish(run_time)) {
				if(run_point >= total_num_of_point-1){
					current_duty_state = SCARA_DUTY_STATE_INIT;
					lowlayer_readTruePosition(&positionNext);
				    kinematicForward(&positionNext);
				    Gcode_data_available = 0;
				    detail_array[0] = GCODE_FINISH;
					respond_lenght = commandRespond(RPD_DONE, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
					total_respond_length += respond_lenght;
				}else{
					if(Gcode_Mode == GCODE_SMOOTH_LSPB){
						accumulate_update(Gcode_Cor[run_point++]);
					//}else if(Gcode_Cor[run_point].configure.type_define[0] == BEZIER_TYPE){
						//run_point+=2;
					}else{
						run_point++;
					}
					current_duty_state = SCARA_DUTY_STATE_OPERATION;
					positionNext.x = duty_cmd.target_point.x;
					positionNext.y = duty_cmd.target_point.y;
					positionNext.z = duty_cmd.target_point.z;
					positionNext.roll = duty_cmd.target_point.roll;
					//memcpy(&positionNext, &duty_cmd.target_point, sizeof(SCARA_PositionTypeDef));
				}

//				lowlayer_readTruePosition(&positionNext);
//				kinematicForward(&positionNext);
			  } else{
				SCARA_StatusTypeDef status = scaraFlowDuty(run_time , &positionNext, positionCurrent);
				if ( SCARA_STATUS_OK == status) {
					lowlayer_computeAndWritePulse(positionCurrent, positionNext);
			    }else{
			  	    current_duty_state = SCARA_DUTY_STATE_FINISH;
			  	    detail_array[0] = status;
			  	    respond_lenght = commandRespond(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
			  	    total_respond_length += respond_lenght;
			    }
			  }
		  }
		  break;

		  case SCARA_DUTY_STATE_FINISH:{
			  method_state = SCARA_METHOD_IDLE;
		  }
		  break;
		  
		  default:
			  break;
		  }
	  }

	  break;
	  case SCARA_METHOD_TEST:
	  {
		  switch(current_duty_state){
		  case SCARA_DUTY_STATE_READY:
			  method_state = SCARA_METHOD_IDLE;
		  break;
		  case SCARA_DUTY_STATE_FLOW:
			  method_state = SCARA_METHOD_ACTIVE;
			  lowlayer_writePulse(test_value[0], test_value[1], test_value[2], test_value[3]);
		  break;
		  }

	  }
	  break;


	  case SCARA_METHOD_PICK_AND_PLACE:
	  {
		  switch(current_duty_state) {
		  case SCARA_DUTY_STATE_INIT:{
			  //HAL_TIM_Base_Start(&htim2);
			  object_tail_pointer = 0;
			  object_head_pointer = 0;
			  operation_state = SCARA_MOVE_TO_TARGET;
			  current_duty_state = SCARA_DUTY_STATE_OPERATION;
			  duty_cmd.robot_method = SCARA_METHOD_PICK_AND_PLACE;
			  duty_cmd.path_type = DUTY_PATH_LINE;
			  duty_cmd.space_type = DUTY_SPACE_TASK;

		  }
		  break;

		  case SCARA_DUTY_STATE_OPERATION:{
			  if(object_tail_pointer != object_head_pointer){
				  method_state = SCARA_METHOD_ACTIVE;
				  switch(operation_state){
					  case SCARA_MOVE_TO_TARGET:{
						  if(PNP_MOVE_TYPE == DUTY_MODE_INIT_QT){
							  double wait_time = 1e-6*((uint64_t)(GET_MICROS - Object[object_tail_pointer].timer_value)) + MOVE_FACTOR + PUT_DOWN_TIME_ON_OBJECT + ATTACH_TIME;
							  state_time = MOVE_FACTOR;
//							  Object[object_tail_pointer].object_position.y -= wait_time*CONVEYOR_SPEED;
//							  Object[object_tail_pointer].object_position.z = UP_HEIGHT;
							  duty_cmd.target_point.x = Object[object_tail_pointer].object_position.x;
							  duty_cmd.target_point.y = Object[object_tail_pointer].object_position.y - wait_time*CONVEYOR_SPEED;
							  duty_cmd.target_point.z = UP_HEIGHT;
							  duty_cmd.target_point.roll = Object[object_tail_pointer].object_position.roll;
						  }else if(PNP_MOVE_TYPE == DUTY_MODE_INIT_QV){
							  double has_moved_distance = CONVEYOR_SPEED*(1e-6*((uint64_t)(GET_MICROS - Object[object_tail_pointer].timer_value)) + PUT_DOWN_TIME_ON_OBJECT + ATTACH_TIME);
							  double implement_distance = PNPcalMovDistance(positionCurrent.x, positionCurrent.y, Object[object_tail_pointer].object_position.x, Object[object_tail_pointer].object_position.y-has_moved_distance, MOVE_FACTOR*V_MOVE_MAX/1.5, CONVEYOR_SPEED);
//							  Object[object_tail_pointer].object_position.y -= (has_moved_distance+implement_distance);
//							  Object[object_tail_pointer].object_position.z = UP_HEIGHT;
							  duty_cmd.target_point.x = Object[object_tail_pointer].object_position.x;
							  duty_cmd.target_point.y = Object[object_tail_pointer].object_position.y - has_moved_distance - implement_distance;
							  duty_cmd.target_point.z = UP_HEIGHT;
							  duty_cmd.target_point.roll = Object[object_tail_pointer].object_position.roll;
							  state_time = 0;
						  }
					  }
					  break;

					  case SCARA_MOVE_DOWN_ON_OBJECT:{
//						  Object[object_tail_pointer].object_position.z = DOWN_HEIGHT_ON_OBJECT;
						  duty_cmd.target_point.z = DOWN_HEIGHT_ON_OBJECT;
						  state_time = PUT_DOWN_TIME_ON_OBJECT;
					  }
					  break;
					  case SCARA_ATTACH:{
//						  state_time = ATTACH_TIME;
						  state_time = 0;
						  scaraSetOutput(1);
					  }
					  break;
					  case SCARA_MOVE_UP_ON_OBJECT:{
//						  Object[object_tail_pointer].object_position.z = UP_HEIGHT;
						  duty_cmd.target_point.z = UP_HEIGHT;
						  state_time = PICK_UP_TIME_ON_OBJECT;
					  }
					  break;
					  case SCARA_MOVE_TO_SLOT :{
						  ObjectType current_type = Object[object_tail_pointer].object_type;
//						  Object[object_tail_pointer].object_position.x = SLot_Cordinate[current_type].posx + Slot_Placement[current_type]*placement_spacing;
//						  Object[object_tail_pointer].object_position.y = SLot_Cordinate[current_type].posy;
//						  Object[object_tail_pointer].object_position.roll = SLot_Cordinate[current_type].roll;
//						  Object[object_tail_pointer].object_position.z = UP_HEIGHT;

						  duty_cmd.target_point.x = SLot_Cordinate[current_type].posx + Slot_Placement[current_type]*placement_spacing;
						  duty_cmd.target_point.y = SLot_Cordinate[current_type].posy;
						  duty_cmd.target_point.z = UP_HEIGHT;
						  duty_cmd.target_point.roll = SLot_Cordinate[current_type].roll;

						  if(PNP_MOVE_TYPE == DUTY_MODE_INIT_QT){
							  state_time = MOVE_FACTOR;
						  }else if(PNP_MOVE_TYPE == DUTY_MODE_INIT_QV){
							  state_time = 0;
						  }
						  Slot_Placement[current_type] = (Slot_Placement[current_type] + 1)%2;
					  }
					  break;
					  case SCARA_MOVE_DOWN_ON_SLOT:{
//						  Object[object_tail_pointer].object_position.z = DOWN_HEIGHT_ON_SLOT;
						  duty_cmd.target_point.z = DOWN_HEIGHT_ON_SLOT;
						  state_time = PUT_DOWN_TIME_ON_SLOT;
					  }
					  break;
					  case SCARA_RELEASE:{
						  state_time = DETACH_TIME;
						  scaraSetOutput(0);
					  }
					  break;
					  case SCARA_MOVE_UP_ON_SLOT:{
						  state_time = PICK_UP_TIME_ON_SLOT;
//						  Object[object_tail_pointer].object_position.z = UP_HEIGHT;
						  duty_cmd.target_point.z = UP_HEIGHT;
					  }
					  break;
				  }

//				  memcpy(&duty_cmd.target_point, &Object[object_tail_pointer].object_position, sizeof(SCARA_PositionTypeDef));
				  duty_cmd.time_total = state_time;
				  if(PNP_MOVE_TYPE == DUTY_MODE_INIT_QT){
					  duty_cmd.v_factor = 0;
				  }else if(PNP_MOVE_TYPE == DUTY_MODE_INIT_QV){
					  duty_cmd.v_factor = MOVE_FACTOR;
				  }
				  duty_cmd.coordinate_type = DUTY_COORDINATES_ABS;

				  SCARA_StatusTypeDef status1;
				  if(operation_state == SCARA_ATTACH || operation_state == SCARA_RELEASE){
					  status1 = SCARA_STATUS_OK;
				  }else{
					  if(operation_state == SCARA_MOVE_DOWN_ON_OBJECT || operation_state == SCARA_MOVE_DOWN_ON_SLOT || operation_state == SCARA_MOVE_UP_ON_OBJECT || operation_state == SCARA_MOVE_UP_ON_SLOT){
						  duty_cmd.path_type = DUTY_PATH_LINE;
						  duty_cmd.space_type = DUTY_SPACE_TASK;
						  duty_cmd.trajec_type = DUTY_TRAJECTORY_LINEAR;
						  duty_cmd.modeInit_type = DUTY_MODE_INIT_QT;
					  }else{
						  duty_cmd.modeInit_type = DUTY_MODE_INIT_QVT;
						  if(pnp_move_option == CMD_MOVE_LINE){
							  duty_cmd.path_type = DUTY_PATH_LINE;
							  duty_cmd.space_type = DUTY_SPACE_TASK;
							  duty_cmd.trajec_type = DUTY_TRAJECTORY_SCURVE;
						  }else if(pnp_move_option == CMD_MOVE_JOINT){
							  duty_cmd.joint_type = DUTY_JOINT_4DOF;
							  duty_cmd.space_type = DUTY_SPACE_JOINT;
							  duty_cmd.trajec_type = DUTY_TRAJECTORY_LSPB;
						  }

					  }
					  status1 = scaraInitDuty(duty_cmd);
//					  if(myDUTY.time_total < 0.2 && operation_state == SCARA_MOVE_TO_TARGET){
//						  object_tail_pointer = (object_tail_pointer+1)%8;
//						  operation_state = SCARA_MOVE_TO_TARGET;
//						  detail_array[0] = OBJECT_UNREACHABLE;
//						  respond_lenght = commandRespond1(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
//						  total_respond_length += respond_lenght;
//						  break;
//					  }
				  }
				  if ( SCARA_STATUS_OK == status1) {
				  	current_duty_state = SCARA_DUTY_STATE_FLOW;
				  	run_time = 0;
				  } else {
					object_tail_pointer = (object_tail_pointer+1)%8;
					operation_state = SCARA_MOVE_TO_TARGET;
					detail_array[0] = status1;
					respond_lenght = commandRespond(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
					total_respond_length += respond_lenght;
//					LOG_REPORT("INIT FAIL", __LINE__);
					break;
				  }
			  }else{
				  method_state = SCARA_METHOD_IDLE;
				  break;
			  }
		  }


		  case SCARA_DUTY_STATE_FLOW:{
			  method_state = SCARA_METHOD_ACTIVE;
			  SCARA_StatusTypeDef status;
			  // Increase run time
			  run_time += T_SAMPLING;
			  // Check Time Out
			  if (scaraIsFinish(run_time)) {
				current_duty_state = SCARA_DUTY_STATE_OPERATION;
				if(operation_state == SCARA_MOVE_UP_ON_SLOT){
					operation_state = SCARA_MOVE_TO_TARGET;
					object_tail_pointer = (object_tail_pointer+1)%8;
				}else{
					operation_state += 1; //next state
				}
				lowlayer_readTruePosition(&positionNext);
				kinematicForward(&positionNext);
			  } else if(operation_state != SCARA_ATTACH && operation_state != SCARA_RELEASE){
				status = scaraFlowDuty(run_time , &positionNext, positionCurrent);
			    if ( SCARA_STATUS_OK == status) {
			  	  lowlayer_computeAndWritePulse(positionCurrent, positionNext);
			    }else{
			  	  current_duty_state = SCARA_DUTY_STATE_FINISH;
			  	  detail_array[0] = status;
			  	  respond_lenght = commandRespond(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
			  	  total_respond_length += respond_lenght;
			    }
			    }
		  }
		  break;

		  case SCARA_DUTY_STATE_FINISH:
			  lowlayer_readTruePosition(&positionNext);
			  kinematicForward(&positionNext);
			  current_duty_state = SCARA_DUTY_STATE_INIT;
			  //error
			  //free(Object);
		  break;
		  }
	  }
	  break;

	  default:
	  {

	  }
	  }
	  if(continuous_update == 1 && update_pos_counter++ >= update_pos_cycle){
		  update_pos_counter = 0;
		  lowlayer_readTruePosition(&update_position);
//		  kinematicForward(&update_position);
		  if(testing_value == 1){
			  testing_value = 0;
			  update_position.D3 = -2000;
		  }
		  respond_lenght = scaraPosition_packaging(detail_array, update_position);
		  respond_lenght = commandRespond(RPD_POSITION, CMD_READ_POSITION, detail_array, respond_lenght, &respond[total_respond_length]);
		  total_respond_length += respond_lenght;
	  }
//	  if(testing_value == 1){
//		  testing_value = 0;
//		  LOG_REPORT1("info", value1, value2);
//	  }
	  /* 4--- Send to PC Phase ---*/
	if(total_respond_length > 0){
		CDC_Transmit_FS(respond, total_respond_length);
	}

	  /* 5--- Update ---*/
	  scaraSetMethod(current_method);
	  scaraSetMode(current_mode);
	  scaraSetDutyState(current_duty_state);

    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Start_USB_RX_Task */
/**
* @brief Function implementing the USB_RX_Check_Ta thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_USB_RX_Task */
void Start_USB_RX_Task(void const * argument)
{
  /* USER CODE BEGIN Start_USB_RX_Task */
	int32_t distance;
	int32_t id_command;
	DUTY_Command_TypeDef 	duty_cmd;
	Robot_CommandTypedef 	cmd_type;
	Robot_RespondTypedef 	rpd_type;
	uint8_t			 	detail[135];
	uint8_t				respond[145];
	uint8_t 			data_packet[256];
	//uint8_t				message[150];
	int32_t				respond_lenght;
	//int32_t				message_lenght;
	int32_t 			detail_length;

	// Default value
	duty_cmd.key_speed = 1;

  /* Infinite loop */
  for(;;)
  {
	  for(;;) {
		  distance = ringBuff_DistanceOf(&usb_rx_ringbuff, RECEIVE_END);
		  if (distance != -1) {
			  uint8_t temp[distance+1];
			  int32_t ret;
			  ringBuff_PopArray(&usb_rx_ringbuff, temp, distance + 1);
			  ret = unPackPayload(temp, distance + 1, data_packet);

			  if( -1 == ret) {
				  //LOG_REPORT("UNPACK FAIL", __LINE__);
			  } else {
				  //LOG_REPORT((char*) temp, __LINE__);
				  cmd_type = packetRead(data_packet, ret, &id_command, &duty_cmd);
				  memset(detail, 0, sizeof(detail));
				  detail_length = 0;
				  rpd_type = commandReply(cmd_type, duty_cmd, detail, &detail_length);
				  //LOG_REPORT("tail", usb_rx_ringbuff.tail);
				  if ( RPD_DUTY == rpd_type) {
					  DUTY_Command_TypeDef *dataMail;
					  dataMail = NULL;
					  // Wait allocate
					  while (dataMail == NULL) {
						  dataMail = osMailAlloc(commandMailHandle, osWaitForever);
					  }
					  memcpy( dataMail, &duty_cmd, sizeof(DUTY_Command_TypeDef));
					  osStatus result;
					  result = osMailPut(commandMailHandle, dataMail);
					  if (osOK == result) {
						  //LOG_REPORT("DUTY SEND", __LINE__);
					  }

				//   }else if( RPD_POSITION == rpd_type) { 
				// 	  CDC_Transmit_FS(detail, 84);
				  }else if(rpd_type == RPD_TRANSFER){

				  }else {
					  respond_lenght	= commandRespond(rpd_type, cmd_type,
							  	  	  	  detail, detail_length,
										  respond);
					  CDC_Transmit_FS(respond, respond_lenght);

				  }
			  }
		  }
	  }
    osDelay(1);// Never go to this place
  }
  /* USER CODE END Start_USB_RX_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
