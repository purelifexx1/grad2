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
extern SCARA_PositionTypeDef		positionTrue;
extern SCARA_Gcode_Cor_TypeDef		Gcode_Cor[1000];
extern int32_t						current_key_speed1 = 1;
extern double						conveyor_speed = 0;
extern double						up_z_height;
extern double 						down_z_height;
extern int32_t						total_num_of_point;
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
  osThreadDef(defaultTask, StartDefaultTask, osPriorityHigh, 0, 2048);
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
  int8_t 				test_value[4];
  uint8_t				respond[40];
  int32_t				respond_lenght;
  //uint8_t				position[135];
  //uint8_t				infor[145];
  int32_t				infor_lenght;
  //uint8_t				task_usb[150];
  int32_t				task_usb_lenght;
  int32_t 				total_respond_length;
  int32_t 				detail_ptr;
  uint8_t				detail_array[10];

  //uint8_t				respond_packed[50];
  int32_t				respond_packed_lenght;
  //uint8_t				infor_packed[150];
  int32_t				infor_packed_lenght;
  //uint8_t				usb_buff[350];
  int32_t				usb_lenght;
  // Debug variables
  //uint32_t total_pulse = 0;

  // Robot variables
  SCARA_MethodTypeDef		current_method;
  SCARA_ModeTypeDef			current_mode;
  SCARA_DutyStateTypeDef 	current_duty_state;
  SCARA_ScanStateTypeDef	current_scan_state;
  SCARA_KeyStateTypeDef		current_key_state;
  SCARA_KeyTypeDef			current_key;
  int32_t					current_key_speed;
  double						run_time;

  SCARA_Pick_And_Place_Package Object[8];
  SCARA_Pick_And_Place_State  operation_state;
  uint8_t object_tail_pointer = 0;
  uint8_t object_head_pointer = 0;
  double state_time = 0;
  int run_point = 0;
  const SCARA_Slot_TypeDef SLot_Cordinate[NUM_OF_OBJECT] = {
  		{203.859f, -101.616f, 0.0f} ,
  		{203.859f, -101.616f, 0.0f} ,
  		{203.859f, -101.616f, 0.0f} ,
  		{203.859f, -101.616f, 0.0f} ,
		{203.859f, -101.616f, 0.0f} ,
		{203.859f, -101.616f, 0.0f}
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

  positionNext.Theta1 = -1.48267f;
  positionNext.Theta2 = 2.35575f;
  positionNext.D3 = 0.034999;
  positionNext.Theta4 = 2.96586;
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
	  respond_packed_lenght = 0;
	  infor_lenght			= 0;
	  infor_packed_lenght	= 0;
	  task_usb_lenght		= 0;
	  usb_lenght			= 0;
	  total_respond_length  = 0;
	  detail_ptr 			= 0;
	  // Update new position
	  memcpy(&positionPrevios, &positionCurrent, sizeof(SCARA_PositionTypeDef));
	  memcpy(&positionCurrent, &positionNext, sizeof(SCARA_PositionTypeDef));
//#ifndef SIMULATION
//	  if(scaraIsScanLimit()) {
//		  lowlayer_readTruePosition(&positionTrue);
//		  kinematicForward(&positionTrue);
//		  positionTrue.t = positionCurrent.t;
//		  positionTrue.total_time = positionCurrent.total_time;
//		  positionTrue.q = positionCurrent.q;
//	  }
//#endif
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
		  //memset(respond, 0, 40);
		  // Check change method
		  if (duty_cmd.change_method == TRUE) {
			  //free(Object);
			  if (SCARA_METHOD_MANUAL == duty_cmd.robot_method) {
				  // Need add check condition idle in each method
				  current_method = SCARA_METHOD_MANUAL;
				  current_duty_state = SCARA_DUTY_STATE_READY;
				//   respond_lenght = commandRespond(RPD_OK,
				// 								duty_cmd.id_command,
				// 								"Changed MANUAL Method",
				// 								(char *)respond);
				detail_array[0] = MANUAL_METHOD;
				respond_lenght = commandRespond1(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
				total_respond_length += respond_lenght;
			  } else if (SCARA_METHOD_SEMI_AUTO == duty_cmd.robot_method) {
				  current_method = SCARA_METHOD_SEMI_AUTO;
				  current_duty_state = SCARA_DUTY_STATE_READY;
				//   respond_lenght = commandRespond(RPD_OK,
				// 								duty_cmd.id_command,
				// 								"Changed SEMI AUTO Method",
				// 								(char *)respond);
				detail_array[0] = SEMI_AUTO_METHOD;
				respond_lenght = commandRespond1(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
				total_respond_length += respond_lenght;
			  } else if (SCARA_METHOD_GCODE == duty_cmd.robot_method) {
//				  Gcode_Cor = (SCARA_Gcode_Cor_TypeDef*)malloc(1*sizeof(SCARA_Gcode_Cor_TypeDef));
//				  if(Gcode_Cor == NULL){
//					  int t = 2;
//				  }
//				  testing_array = (double*)malloc(2*sizeof(double));
//				  if(testing_array == NULL){
//					  int t = 2;
//				  }
				  current_duty_state = SCARA_DUTY_STATE_READY;
				  current_method = SCARA_METHOD_GCODE;
				//   respond_lenght = commandRespond(RPD_OK,
				// 								duty_cmd.id_command,
				// 								"Changed AUTO Method",
				// 								(char *)respond);
				detail_array[0] = AUTO_METHOD;
				respond_lenght = commandRespond1(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
				total_respond_length += respond_lenght;
			  }else if(SCARA_METHOD_TEST == duty_cmd.robot_method){
				  test_value[0] = 0;
				  test_value[1] = 0;
				  test_value[2] = 0;
				  test_value[3] = 0;
				  current_method = SCARA_METHOD_TEST;
				  detail_array[0] = TEST_METHOD;
				  respond_lenght = commandRespond1(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
				  total_respond_length += respond_lenght;
			  }else if(SCARA_METHOD_PICK_AND_PLACE == duty_cmd.robot_method){
				  current_method = SCARA_METHOD_PICK_AND_PLACE;
				  current_duty_state = SCARA_DUTY_STATE_INIT;
				  detail_array[0] = PICK_AND_PLACE_METHOD;
				  respond_lenght = commandRespond1(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
				  total_respond_length += respond_lenght;
			  }
		  } else {
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
								//   respond_lenght = commandRespond(RPD_OK,
								// 								  duty_cmd.id_command,
								// 								  "Stop Now",
								// 								  (char *)respond);
								detail_array[0] = STOP_NOW;
								respond_lenght = commandRespond1(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
								total_respond_length += respond_lenght;
								  LOG_REPORT("ROBOT STOP !!!", __LINE__);
							  }
							  break;

						  case SCARA_MODE_SCAN:
							  {
								  if (SCARA_MODE_DUTY == current_mode && SCARA_DUTY_STATE_READY == current_duty_state) {
									  current_mode = SCARA_MODE_SCAN;
									  current_scan_state = SCARA_SCAN_STATE_INIT;
									//   respond_lenght = commandRespond(RPD_OK,
									// 								  duty_cmd.id_command,
									// 								  "Start Scan",
									// 								  (char *)respond);
									detail_array[0] = START_SCAN;
									respond_lenght = commandRespond1(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
									total_respond_length += respond_lenght;
								  } else {
									//   respond_lenght = commandRespond(RPD_ERROR,
									// 								  duty_cmd.id_command,
									// 								  "Busy",
									// 								  (char *)respond);
									detail_array[0] = BUSY;
									respond_lenght = commandRespond1(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
									total_respond_length += respond_lenght;
									  LOG_REPORT("SCAN FAIL: BUSY", __LINE__);
								  }
							  }
							  break;

						  case SCARA_MODE_DUTY:
							  {
								  if (SCARA_MODE_DUTY == current_mode && SCARA_DUTY_STATE_READY == current_duty_state) {
									  //scaraSetScanFlag();
//									  if(duty_cmd.modeInit_type == DUTY_MODE_INIT_LINEAR){
//										  current_method = SCARA_METHOD_AUTO;
//									  }
									  if (scaraIsScanLimit()) {
										  current_mode	 = SCARA_MODE_DUTY;
										  current_duty_state	 = SCARA_DUTY_STATE_INIT;
									  } else {
										//   respond_lenght = commandRespond(RPD_ERROR,
										// 								  duty_cmd.id_command,
										// 								  "Has Not Scan Yet.",
										// 								  (char *)respond);
										detail_array[0] = NOT_SCAN;
										respond_lenght = commandRespond1(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
										total_respond_length += respond_lenght;
										  LOG_REPORT("MOVE FAIL:NOT SCAN", __LINE__);
									  }
								  } else {
									//   respond_lenght	= commandRespond(RPD_ERROR,
									// 									  duty_cmd.id_command,
									// 									  "Busy.",
									// 									  (char *)respond);
									detail_array[0] = BUSY;
									respond_lenght = commandRespond1(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
									total_respond_length += respond_lenght;
									  LOG_REPORT("MOVE FAIL:BUSY", __LINE__);
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
						  test_value[0] = 0;
						  test_value[1] = 0;
						  test_value[2] = 0;
						  test_value[3] = 0;
						  switch (duty_cmd.test_key)
						  {
						  case SCARA_TEST_MOTOR1_POS:
							 test_value[duty_cmd.test_key/2] = 5;
						  break;
						  case SCARA_TEST_MOTOR1_NEG:
							 test_value[duty_cmd.test_key/2] = -5;
						  break;
						  case SCARA_TEST_MOTOR2_POS:
							 test_value[duty_cmd.test_key/2] = 5;
						  break;
						  case SCARA_TEST_MOTOR2_NEG:
							 test_value[duty_cmd.test_key/2] = -5;
						  break;
						  case SCARA_TEST_MOTOR3_POS:
							 test_value[duty_cmd.test_key/2] = 5;
						  break;
						  case SCARA_TEST_MOTOR3_NEG:
							 test_value[duty_cmd.test_key/2] = -5;
						  break;
						  case SCARA_TEST_MOTOR4_POS:
							 test_value[duty_cmd.test_key/2] = 10;
						  break;
						  case SCARA_TEST_MOTOR4_NEG:
							 test_value[duty_cmd.test_key/2] = -10;
						  break;						  
						  default:
							  break;
						  }
					  }
					  break;
				  	  case SCARA_METHOD_GCODE:
				  	  {
				  		  if(current_duty_state == SCARA_DUTY_STATE_READY && duty_cmd.id_command == CMD_GCODE_RUN){
				  			  run_point = 1;
				  			  current_duty_state = SCARA_DUTY_STATE_OPERATION;
				  		  }else if(current_duty_state == SCARA_DUTY_STATE_INIT && duty_cmd.id_command == CMD_GCODE_RESUME){
				  			  lowlayer_readTruePosition(&positionCurrent);
				  			  kinematicForward(&positionCurrent);
				  			  current_duty_state = SCARA_DUTY_STATE_INIT;
				  		  }else if(duty_cmd.id_command == CMD_GCODE_STOP){
				  			  current_duty_state = SCARA_DUTY_STATE_READY;
				  		  }else if(duty_cmd.id_command == CMD_GCODE_PAUSE){
				  			  current_duty_state = SCARA_DUTY_STATE_INIT;
				  		  }
				  	  }
				  	  break;
				  	  case SCARA_METHOD_PICK_AND_PLACE:{
				  		  memcpy(&Object[object_head_pointer].object_position, &duty_cmd.target_point, sizeof(SCARA_PositionTypeDef));
				  		  Object[object_head_pointer].timer_value = (uint16_t)duty_cmd.target_point.t;
				  		  object_head_pointer = (object_head_pointer+1)%8;
				  		  detail_array[0] = OBJECT_DETECTED;
						  respond_lenght = commandRespond1(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
						  total_respond_length += respond_lenght;
				  	  }
				  	  break;
				  	  default:
				  	  {
				  		  LOG_REPORT("CMD Error Method !!!", __LINE__);
				  	  }
				  	  }
			  	  } else {
					//   respond_lenght = commandRespond(RPD_ERROR,
					// 								duty_cmd.id_command,
					// 								"METHOD isn't correct",
					// 								(char *)respond);
					detail_array[0] = INCORRECT_METHOD;
					respond_lenght = commandRespond1(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
					total_respond_length += respond_lenght;
			  	  }

			  }
		  isNewDuty = FALSE;
	  }

	  /* 3--- Execute Phase ---*/
	  switch(current_method) {
	  case SCARA_METHOD_MANUAL:
	  {
		  switch( current_key_state) {
		  case SCARA_KEY_STATE_READY:
			  /* Wait for application keyboard , do nothing*/
		  break;
		  case SCARA_KEY_STATE_INIT:
		  {
			  if (scaraKeyInit1(current_key, current_key_speed1) == SCARA_STATUS_OK) {
				  run_time = 0;
				  current_key_state = SCARA_KEY_STATE_FLOW;
// #ifdef SIMULATION
// 				  scaraPosition2String((char *)position, positionCurrent);
// #else
// 				  scaraPosition2String((char *)position, positionTrue);
// #endif
// 				  infor_lenght 		= commandRespond(RPD_START,
// 													  0,
// 													  (char *)position,
// 													  (char *)infor);
					detail_array[0] = NONE;
					respond_lenght = commandRespond1(RPD_START, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
					total_respond_length += respond_lenght;
			  } else {
				  current_key_state = SCARA_KEY_STATE_READY;
			  }
		  }
		  break;
		  case SCARA_KEY_STATE_FLOW:
		  {
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
					  // Running Inform
// #ifdef SIMULATION
// 					  scaraPosition2String((char *)position, positionCurrent);
// #else
// 					  scaraPosition2String((char *)position, positionTrue);
// #endif
// 					  infor_lenght = commandRespond(RPD_RUNNING,
// 													0,
// 													(char *)position,
// 													(char *)infor);

					// detail_array[0] = NONE;
					// respond_lenght = commandRespond1(RPD_RUNNING, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
					// total_respond_length += respond_lenght;
				  } else {
					  current_key_state = SCARA_KEY_STATE_FINISH;
					  // Critical
					  // If a error appear while Flowing, This is very important
					//   infor_lenght = commandRespond(RPD_STOP,
					// 								0,
					// 								(char *)DETAIL_STATUS[status],
					// 								(char *)infor);
					detail_array[0] = status;
					respond_lenght = commandRespond1(RPD_STOP, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
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
						  //Done Inform
						//   scaraPosition2String((char *)position, positionNext);
						//   infor_lenght 		= commandRespond(RPD_DONE,
						// 									 0,
						// 									(char *)position,
						// 									(char *)infor);
						detail_array[0] = NONE;
						respond_lenght = commandRespond1(RPD_DONE, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
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
						  // Do nothing();

					  }
				  break;

				  case SCARA_DUTY_STATE_INIT:
					  {

						  SCARA_StatusTypeDef status1, status2;
						  status1 = scaraInitDuty(duty_cmd);
						  if ( SCARA_STATUS_OK == status1) {
							  status2 = scaraTestDuty();
							  if (SCARA_STATUS_OK == status2) {
							  current_duty_state = SCARA_DUTY_STATE_FLOW;
							  //current_duty_state = SCARA_DUTY_STATE_FINISH;
							  run_time			= 0;
							  // Respond
							  detail_array[0] = status1;
							  respond_lenght = commandRespond1(RPD_OK, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
							  total_respond_length += respond_lenght;
							  detail_array[0] = NONE;
							  respond_lenght = commandRespond1(RPD_START, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
							  total_respond_length += respond_lenght;

							  } else {
								  current_duty_state 	= SCARA_DUTY_STATE_READY;
								//   respond_lenght	= commandRespond(RPD_ERROR,
								// 									  duty_cmd.id_command,
								// 									  (char *)DETAIL_STATUS[status2],
								// 									  (char *)respond);
								  LOG_REPORT("TEST FAIL", __LINE__);
								detail_array[0] = status2;
								respond_lenght = commandRespond1(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
								total_respond_length += respond_lenght;
							  }
						  } else {
							  current_duty_state 	= SCARA_DUTY_STATE_READY;

							detail_array[0] = status1;
							respond_lenght = commandRespond1(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
							total_respond_length += respond_lenght;
							  LOG_REPORT("INIT FAIL", __LINE__);
						  }
					  }
				  break;

				  case SCARA_DUTY_STATE_FLOW:
					  {
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
								respond_lenght = commandRespond1(RPD_STOP, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
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
						  positionNext.t = 0;
						  positionNext.total_time = 0;
						  positionNext.q = 0;
						  // Done Inform

					detail_array[0] = NONE;
					respond_lenght = commandRespond1(RPD_DONE, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
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
		  case SCARA_DUTY_STATE_READY:{
			  //run_point = 1;
		  }
		  break;
		  case SCARA_DUTY_STATE_INIT:{

		  }
		  break;
		  case SCARA_DUTY_STATE_OPERATION:{
			  update_gcode_point(&duty_cmd, Gcode_Cor[run_point]);
			  SCARA_StatusTypeDef status;
			  status = scaraInitDuty(duty_cmd);
			  if(status == SCARA_STATUS_OK){
				  run_time = 0;
				  current_duty_state = SCARA_DUTY_STATE_FLOW;
				  detail_array[0] = (uint8_t)(run_point * 100.0f / total_num_of_point );
				  respond_lenght = commandRespond1(RDP_GCODE_PROCESS, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
				  total_respond_length += respond_lenght;
			  }else{
				  current_duty_state = SCARA_DUTY_STATE_FINISH;
				  detail_array[0] = status;
				  respond_lenght = commandRespond1(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
				  total_respond_length += respond_lenght;
				  break;
			  }
		  }
		  //break;

		  case SCARA_DUTY_STATE_FLOW:{
			  run_time += T_SAMPLING;
			  // Check Time Out
			  if (scaraIsFinish(run_time)) {
				if(run_point >= total_num_of_point){
					current_duty_state = SCARA_DUTY_STATE_READY;
					lowlayer_readTruePosition(&positionNext);
				    kinematicForward(&positionNext);
				}else{
					current_duty_state = SCARA_DUTY_STATE_OPERATION;
					run_point++;
					memcpy(&positionNext, &duty_cmd.target_point, sizeof(SCARA_PositionTypeDef));
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
					  respond_lenght = commandRespond1(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
					  total_respond_length += respond_lenght;
				  }
			  }
		  }
		  break;

		  case SCARA_DUTY_STATE_FINISH:{

		  }
		  break;
		  
		  default:
			  break;
		  }
	  }

	  break;
	  case SCARA_METHOD_TEST:
	  {
		  lowlayer_writePulse(test_value[0], test_value[1], test_value[2], test_value[3]);
	  }
	  break;


	  case SCARA_METHOD_PICK_AND_PLACE:
	  {
		  switch(current_duty_state) {
		  case SCARA_DUTY_STATE_INIT:{
			  //Object = calloc(8, sizeof(SCARA_Pick_And_Place_Package));
			  HAL_TIM_Base_Start(&htim2);
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
				  switch(operation_state){
					  case SCARA_MOVE_TO_TARGET:{
						  double wait_time = TIMER_SCALE*((uint16_t)(TIM2->CNT - Object[object_tail_pointer].timer_value)) + MOVE_TIME + PUT_DOWN_TIME_ON_OBJECT;
						  Object[object_tail_pointer].object_position.y -= wait_time*conveyor_speed;
						  Object[object_tail_pointer].object_position.z = UP_HEIGHT;
						  state_time = MOVE_TIME;
					  }
					  break;

					  case SCARA_MOVE_DOWN_ON_OBJECT:{
						  Object[object_tail_pointer].object_position.z = DOWN_HEIGHT_ON_OBJECT;
						  state_time = PUT_DOWN_TIME_ON_OBJECT;
					  }
					  break;
					  case SCARA_ATTACH:{
						  state_time = ATTACH_TIME;
						  scaraSetOutput(1);
					  }
					  break;
					  case SCARA_MOVE_UP_ON_OBJECT:{
						  Object[object_tail_pointer].object_position.z = UP_HEIGHT;
						  state_time = PICK_UP_TIME_ON_OBJECT;
					  }
					  break;
					  case SCARA_MOVE_TO_SLOT :{

						  Object[object_tail_pointer].object_position.x = SLot_Cordinate[Object[object_tail_pointer].object_position.object_type].posx;
						  Object[object_tail_pointer].object_position.y = SLot_Cordinate[Object[object_tail_pointer].object_position.object_type].posy;
						  Object[object_tail_pointer].object_position.roll = SLot_Cordinate[Object[object_tail_pointer].object_position.object_type].roll;
						  Object[object_tail_pointer].object_position.z = UP_HEIGHT;
						  state_time = MOVE_TIME;
					  }
					  break;
					  case SCARA_MOVE_DOWN_ON_SLOT:{
						  Object[object_tail_pointer].object_position.z = DOWN_HEIGHT_ON_SLOT;
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
						  Object[object_tail_pointer].object_position.z = UP_HEIGHT;
					  }
					  break;
				  }
				  memcpy(&duty_cmd.target_point, &Object[object_tail_pointer].object_position, sizeof(SCARA_PositionTypeDef));
				  duty_cmd.time_total = state_time;
				  SCARA_StatusTypeDef status1;
				  duty_cmd.v_factor = 0;
				  if(operation_state == SCARA_ATTACH || operation_state == SCARA_RELEASE){
					  status1 = SCARA_STATUS_OK;
				  }else{
					  if(operation_state == SCARA_MOVE_DOWN_ON_OBJECT || operation_state == SCARA_MOVE_DOWN_ON_SLOT || operation_state == SCARA_MOVE_UP_ON_OBJECT || operation_state == SCARA_MOVE_UP_ON_SLOT){
						  duty_cmd.trajec_type = DUTY_TRAJECTORY_LINEAR;
						  duty_cmd.modeInit_type = DUTY_MODE_INIT_QT;
					  }else{
						  duty_cmd.trajec_type = DUTY_TRAJECTORY_LSPB;
						  duty_cmd.modeInit_type = DUTY_MODE_INIT_QVT;
					  }
					  status1 = scaraInitDuty(duty_cmd);
				  }
				  if ( SCARA_STATUS_OK == status1) {
				  	current_duty_state = SCARA_DUTY_STATE_FLOW;
				  	run_time = 0;
				  } else {
					object_tail_pointer = (object_tail_pointer+1)%8;
					operation_state = SCARA_MOVE_TO_TARGET;
					detail_array[0] = status1;
					respond_lenght = commandRespond1(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
					total_respond_length += respond_lenght;
					LOG_REPORT("INIT FAIL", __LINE__);
					break;
				  }
			  }else{
				  break;
			  }
		  }


		  case SCARA_DUTY_STATE_FLOW:{
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
					  respond_lenght = commandRespond1(RPD_ERROR, duty_cmd.id_command, detail_array, 1, &respond[total_respond_length]);
					  total_respond_length += respond_lenght;
				  }
			  }
		  }
		  break;

		  case SCARA_DUTY_STATE_FINISH:

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

	  /* 4--- Send to PC Phase ---*/
	  // Check buffer from USB task
	//   osMutexWait(usbTxMutexHandle, osWaitForever);
	//   task_usb_lenght = ringBuff_PopArray(&cmd_tx_ringbuff, task_usb, RINGBUFFER_SIZE);
	//   osMutexRelease(usbTxMutexHandle);
	//   // Intergrate to 1 buffer
	//   if (respond_lenght > 0) {
	// 	  respond_packed_lenght = packPayload(respond, respond_packed, respond_lenght);
	// 	  memcpy(usb_buff, respond_packed, respond_packed_lenght);
	//   }
	//   if (task_usb_lenght > 0) {
	// 	  memcpy(usb_buff + respond_packed_lenght, task_usb, task_usb_lenght);
	//   }
	//   if (infor_lenght > 0) {
	// 	  infor_packed_lenght 	= packPayload(infor, infor_packed, infor_lenght);
	// 	  memcpy(usb_buff + respond_packed_lenght + task_usb_lenght, infor_packed, infor_packed_lenght);
	//   }
	//   usb_lenght = respond_packed_lenght + task_usb_lenght + infor_packed_lenght;
	//   // Send through USB
	//   if (usb_lenght > 0) {
	// 	  CDC_Transmit_FS(usb_buff, (uint16_t)usb_lenght);
	//   }

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
					//   memset(respond, 0, sizeof(respond));
					//   memset(message, 0, sizeof(message));
					  respond_lenght	= commandRespond1(rpd_type, cmd_type,
							  	  	  	  (char *)detail, detail_length,
										  (char *)respond);
					  //message_lenght	= packPayload(respond, message, respond_lenght);
					  CDC_Transmit_FS(respond, respond_lenght);
					  // Mutex
//					   osMutexWait(usbTxMutexHandle, osWaitForever);
					//   ringBuff_PushArray(&cmd_tx_ringbuff, message, message_lenght);
					//   osMutexRelease(usbTxMutexHandle);
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
