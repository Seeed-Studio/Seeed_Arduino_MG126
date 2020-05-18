#include <string.h>
#include "bsp.h"
#include "mg_api.h"

#include <Arduino.h>

ble_trans_t recv_struct;

/// Characteristic Properties Bit
#define ATT_CHAR_PROP_RD                            0x02
#define ATT_CHAR_PROP_W_NORSP                       0x04
#define ATT_CHAR_PROP_W                             0x08
#define ATT_CHAR_PROP_NTF                           0x10
#define ATT_CHAR_PROP_IND                           0x20 
#define GATT_PRIMARY_SERVICE_UUID                   0x2800

#define TYPE_CHAR      0x2803
#define TYPE_CFG       0x2902
#define TYPE_INFO      0x2901
#define TYPE_xRpRef    0x2907
#define TYPE_RpRef     0x2908
#define TYPE_INC       0x2802
#define UUID16_FORMAT  0xff


#define SOFTWARE_INFO "SV2.1.4"
#define MANU_INFO     "Seeeduino Bluetooth"
char DeviceInfo[15] =  "Wio_Lite_Serial";  /*max len is 24 bytes*/

uint16_t cur_notifyhandle = 0x12;  //Note: make sure each notify handle by invoking function: set_notifyhandle(hd);

uint8_t* getDeviceInfoData(uint8_t* len)
{
    *len = sizeof(DeviceInfo);
    return (uint8_t*)DeviceInfo;
}

void updateDeviceInfoData(uint8_t* name, uint8_t len)
{
    memcpy(DeviceInfo,name, len);
    ble_set_name(name,len);
}

/**********************************************************************************
                 *****DataBase****

01 - 06  GAP (Primary service) 0x1800
  03:04  name
07 - 0f  Device Info (Primary service) 0x180a
  0a:0b  firmware version
  0e:0f  software version
10 - 19  LED service (Primary service) 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
  11:12  6E400003-B5A3-F393-E0A9-E50E24DCCA9E(0x04)  RxNotify
  13     cfg
  14:15  6E400002-B5A3-F393-E0A9-E50E24DCCA9E(0x0C)  Tx
  16     cfg
  17:18  6E400004-B5A3-F393-E0A9-E50E24DCCA9E(0x0A)  BaudRate
  19     0x2901  info
************************************************************************************/

typedef struct ble_character16{
    uint16_t type16;          //type2
    uint16_t handle_rec;      //handle
    uint8_t  characterInfo[5];//property1 - handle2 - uuid2
    uint8_t  uuid128_idx;     //0xff means uuid16,other is idx of uuid128
}BLE_CHAR;

typedef struct ble_UUID128{    
    uint8_t  uuid128[16];//uuid128 string: little endian
}BLE_UUID128;

//
///STEP0:Character declare
//
const BLE_CHAR AttCharList[] = {
// ======  gatt =====  Do NOT Change!!
    {TYPE_CHAR,0x0003, {ATT_CHAR_PROP_RD, 0x04,0, 0x00,0x2a}, UUID16_FORMAT},//name
    //05-06 reserved
// ======  device info =====    Do NOT Change if using the default!!!  
    {TYPE_CHAR,0x0008, {ATT_CHAR_PROP_RD, 0x09,0, 0x29,0x2a}, UUID16_FORMAT},//manufacture
    {TYPE_CHAR,0x000a, {ATT_CHAR_PROP_RD, 0x0b,0, 0x26,0x2a}, UUID16_FORMAT},//firmware version
    {TYPE_CHAR,0x000e, {ATT_CHAR_PROP_RD, 0x0f,0, 0x28,0x2a}, UUID16_FORMAT},//sw version
    
// ======  User service or other services added here =====  User defined  
    {TYPE_CHAR,0x0011, {ATT_CHAR_PROP_NTF,                     0x12,0, 0,0}, 1/*uuid128-idx1*/ },//RxNotify
    {TYPE_CFG, 0x0013, {ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W}},//cfg    
    {TYPE_CHAR,0x0014, {ATT_CHAR_PROP_W|ATT_CHAR_PROP_W_NORSP, 0x15,0, 0,0}, 2/*uuid128-idx2*/ },//Tx    
	// {TYPE_CHAR,0x0017, {ATT_CHAR_PROP_W|ATT_CHAR_PROP_RD,      0x18,0, 0,0}, 3/*uuid128-idx3*/ },//BaudRate
    // {TYPE_INFO,0x0019, {ATT_CHAR_PROP_RD}}//description,"BaudRate"
};

const BLE_UUID128 AttUuid128List[] = {
    /*for supporting the android app [nRF UART V2.0], one SHOULD using the 0x9e,0xca,0xdc.... uuid128*/
    {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,1,0,0x40,0x6e}, //idx0,little endian, service uuid
    {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,3,0,0x40,0x6e}, //idx1,little endian, RxNotify
    {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,2,0,0x40,0x6e}, //idx2,little endian, Tx
    {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,4,0,0x40,0x6e}, //idx3,little endian, BaudRate
};

uint8_t GetCharListDim(void)
{
    return sizeof(AttCharList)/sizeof(AttCharList[0]);
}

//////////////////////////////////////////////////////////////////////////
///STEP1:Service declare
// read by type request handle, primary service declare implementation
void att_server_rdByGrType( uint8_t pdu_type, uint8_t attOpcode, uint16_t st_hd, uint16_t end_hd, uint16_t att_type )
{
 //!!!!!!!!  hard code for gap and gatt, make sure here is 100% matched with database:[AttCharList] !!!!!!!!!                 
    if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd == 1))//hard code for device info service
    {
        //att_server_rdByGrTypeRspDeviceInfo(pdu_type);//using the default device info service
        //GAP Device Name
        uint8_t t[] = {0x00,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x1,0x6,(uint8_t*)(t),2);
        return;
    }
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x07))//hard code for device info service
    {
        //apply user defined (device info)service example
        uint8_t t[] = {0xa,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x7,0xf,(uint8_t*)(t),2);
        return;
    }
    
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x10)) //usr's service
    {
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x10,0x19,(uint8_t*)(AttUuid128List[0].uuid128),16);
        return;
    }
    //other service added here if any
    //to do....

    ///error handle
    att_notFd( pdu_type, attOpcode, st_hd );
}


///STEP2:data coming
///write response, data coming....
void ser_write_rsp(uint8_t pdu_type/*reserved*/, uint8_t attOpcode/*reserved*/, 
                   uint16_t att_hd, uint8_t* attValue/*app data pointer*/, uint8_t valueLen_w/*app data size*/)
{
    arduino_c_print("data coming!");
    arduino_c_print_num(att_hd);
    switch(att_hd)
    {
        case 0x18://BaudRate
			ser_write_rsp_pkt(pdu_type);
			break;
        case 0x15://Tx
            // arduino_c_print(attValue);
            memcpy(recv_struct.buf,attValue,valueLen_w);
            recv_struct.recv_len = valueLen_w;
            break;
        case 0x12://cmd
        case 0x13://cfg  
            ser_write_rsp_pkt(pdu_type);  /*if the related character has the property of WRITE(with response) or TYPE_CFG, one MUST invoke this func*/      
            break;
        
        default:
            att_notFd( pdu_type, attOpcode, att_hd );	/*the default response, also for the purpose of error robust */
            break;
    }
 }

///STEP2.1:Queued Writes data if any
void ser_prepare_write(uint16_t handle, uint8_t* attValue, uint16_t attValueLen, uint16_t att_offset)//user's call back api 
{
    //queued data:offset + data(size)
    //when ser_execute_write() is invoked, means end of queue write.
    
    //to do    
}
 
void ser_execute_write(void)//user's call back api 
{
    //end of queued writes  
    
    //to do...    
}

///STEP3:Read data
//// read response
char not_cnt;
void server_rd_rsp(uint8_t attOpcode, uint16_t attHandle, uint8_t pdu_type)
{
    uint8_t tab[3];
    uint8_t  d_len;
    uint8_t buffer[20]= {0};
    uint8_t* ble_name = getDeviceInfoData(&d_len);
    arduino_c_print("Read something!");
    arduino_c_print("handle = ");
    arduino_c_print_num(attHandle);
    arduino_c_print_num(pdu_type);
    arduino_c_print_num(attOpcode);

    not_cnt++;
    // sprintf(buffer,"huangdao%d\r\n",not_cnt);
    // sconn_notifydata(buffer,strlen(buffer));

    switch(attHandle) //hard code
    {
        case 0x04: //GAP name
            att_server_rd( pdu_type, attOpcode, attHandle, ble_name, d_len);
            break;
                
        case 0x09: //MANU_INFO
            //att_server_rd( pdu_type, attOpcode, attHandle, (uint8_t*)(MANU_INFO), sizeof(MANU_INFO)-1);
            //get_ble_version->MG_BLE_LIB_PAIR_V3.0
            att_server_rd( pdu_type, attOpcode, attHandle, get_ble_version(), strlen((const char*)get_ble_version())); //ble lib build version
            break;
        
        case 0x0b: //FIRMWARE_INFO
        {            
            //do NOT modify this code!!!
            //GetFirmwareInfo -> FV1B.3.3.12
            att_server_rd( pdu_type, attOpcode, attHandle, GetFirmwareInfo(),strlen((const char*)GetFirmwareInfo()));
            break;
        }
        
        case 0x0f://SOFTWARE_INFO
            att_server_rd( pdu_type, attOpcode, attHandle, (uint8_t*)(SOFTWARE_INFO), sizeof(SOFTWARE_INFO)-1);
            break;
        
        case 0x13://cfg
            {
                uint8_t t[2] = {0,0};
                att_server_rd( pdu_type, attOpcode, attHandle, t, 2);
            }
            break;
        
        case 0x18://BaudRate
			att_server_rd( pdu_type, attOpcode, attHandle, &not_cnt, 1);
            break;
        
        case 0x19: //description
            #define MG_BaudRate "BaudRate"
            att_server_rd( pdu_type, attOpcode, attHandle, (uint8_t*)(MG_BaudRate), sizeof(MG_BaudRate)-1);
            break;
        
        default:
            //返回value为全0
            att_notFd( pdu_type, attOpcode, attHandle );/*the default response, also for the purpose of error robust */
            break;
    }               
}

void server_blob_rd_rsp(uint8_t attOpcode, uint16_t attHandle, uint8_t dataHdrP,uint16_t offset)
{
    arduino_c_print("bluetooth read something!");
}

//return 1 means found
int GetPrimaryServiceHandle(unsigned short hd_start, unsigned short hd_end,
                            unsigned short uuid16,   
                            unsigned short* hd_start_r,unsigned short* hd_end_r)
{
// example    
//    if((uuid16 == 0x1812) && (hd_start <= 0x19))// MUST keep match with the information save in function  att_server_rdByGrType(...)
//    {
//        *hd_start_r = 0x19;
//        *hd_end_r = 0x2a;
//        return 1;
//    }
    
    return 0;
}


//���ص���������������ģ���������������֮�ã�Э��ջ����ϵͳ�����ʱ���첽���ص���������������������
uint32_t count;
void gatt_user_send_notify_data_callback(void)
{
    // count++;
    // if( count>20 ){
    //     count = 0;
    //     sconn_notifydata("huangdao",8);
    // }
    // arduino_c_print("Connect OK!!!");
    //to do if any ...
    //add user sending data notify operation ....
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* getsoftwareversion(void)
{
    return (uint8_t*)SOFTWARE_INFO;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

static unsigned char gConnectedFlag=0;
char GetConnectedStatus(void)
{
    return gConnectedFlag;
}

//void LED_ONOFF(unsigned char onFlag);//module led indicator

void ConnectStausUpdate(unsigned char IsConnectedFlag) //porting api
{
    //[IsConnectedFlag] indicates the connection status
    
    LED_ONOFF(!IsConnectedFlag);
    arduino_c_print("connect status changed!");

    if (IsConnectedFlag != gConnectedFlag)
    {
        gConnectedFlag = IsConnectedFlag;

#ifdef USE_UART
#ifdef USE_AT_CMD
		if(gConnectedFlag)
		{
			
		}
		else
		{
		
		}
#else
//        if(gConnectedFlag){
//            SleepStop = 1;
//        }
//        else{
//            SleepStop = 2;
//        }
#endif
#endif
    }
}

void UsrProcCallback(void) //porting api
{
    // arduino_c_print("lululululu");
}
unsigned char aes_encrypt_HW(unsigned char *_data, unsigned char *_key)
{
    return 0; //not supported
}




