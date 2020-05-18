

#include <string.h>
#include "bsp.h"
#include "mg_api.h"

#include <Arduino.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

extern T_H_val_t  T_H_data;

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


#define SOFTWARE_INFO "V1.0.0"
#define MANU_INFO     "Seeeduino"
char DeviceInfo[11] =  "Wio_BLE_T&H";  /*max len is 24 bytes*/

u16 cur_notifyhandle = 0x12;  //Note: make sure each notify handle by invoking function: set_notifyhandle(hd);

u8* getDeviceInfoData(u8* len)
{    
    *len = sizeof(DeviceInfo);
    return (u8*)DeviceInfo;
}

void updateDeviceInfoData(u8* name, u8 len)
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
************************************************************************************/

typedef struct ble_character16{
    u16 type16;          //type2
    u16 handle_rec;      //handle
    u8  characterInfo[5];//property1 - handle2 - uuid2
    u8  uuid128_idx;     //0xff means uuid16,other is idx of uuid128
}BLE_CHAR;

typedef struct ble_UUID128{    
    u8  uuid128[16];//uuid128 string: little endian
}BLE_UUID128;

//
///STEP0:Character declare
//
const BLE_CHAR AttCharList[] = {
// ======  gatt =====  Do NOT Change!!
    {TYPE_CHAR,0x03,ATT_CHAR_PROP_RD, 0x04,0,0x00,0x2a,UUID16_FORMAT},//name
    //05-06 reserved
// ======  device info =====    Do NOT Change if using the default!!!  
    {TYPE_CHAR,0x08,ATT_CHAR_PROP_RD, 0x09,0,0x29,0x2a,UUID16_FORMAT},//manufacture
    {TYPE_CHAR,0x0a,ATT_CHAR_PROP_RD, 0x0b,0,0x26,0x2a,UUID16_FORMAT},//firmware version
    {TYPE_CHAR,0x0e,ATT_CHAR_PROP_RD, 0x0f,0,0x28,0x2a,UUID16_FORMAT},//sw version
    
    
    {TYPE_CHAR,0x11,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_NTF, 0x12,0, 0x1E,0x2A, UUID16_FORMAT},//Temperature
    // {TYPE_CFG, 0x13,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W},//cfg

    {TYPE_CHAR,0x14,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_NTF, 0x15,0, 0x6F,0x2A, UUID16_FORMAT},  //Humidity
    // {TYPE_CFG, 0x16,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W},//cfg

};

const BLE_UUID128 AttUuid128List[] = {
0
};

const BLE_UUID128 AttUuid16List[][2] = {
{0x1a,0x18},
{0x0f,0x18},
};

u8 GetCharListDim(void)
{
    return sizeof(AttCharList)/sizeof(AttCharList[0]);
}

//////////////////////////////////////////////////////////////////////////
///STEP1:Service declare
// read by type request handle, primary service declare implementation
void att_server_rdByGrType( u8 pdu_type, u8 attOpcode, u16 st_hd, u16 end_hd, u16 att_type )
{
 //!!!!!!!!  hard code for gap and gatt, make sure here is 100% matched with database:[AttCharList] !!!!!!!!!
                     
    if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd == 1))//hard code for device info service
    {
        u8 t[] = {0x00,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x1,0x6,(u8*)(t),2);
        return;
    }
    
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x07)) //usr's service
    {
        u8 t[] = {0x0a,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x7,0x0f,(u8*)(t),2);
        return;
    }
    
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x10)) //usr's service
    {
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x10,0x16,(u8*)(AttUuid16List[0]),2);
        return;
    }
    ///error handle
    att_notFd( pdu_type, attOpcode, st_hd );
}
///STEP2:data coming
///write response, data coming....
void ser_write_rsp(u8 pdu_type/*reserved*/, u8 attOpcode/*reserved*/, 
                   u16 att_hd, u8* attValue/*app data pointer*/, u8 valueLen_w/*app data size*/)
{
    arduino_c_print("write rsp,handle = ");
    arduino_c_print(att_hd);
    switch(att_hd)
    {
        case 0x13://cfg 
        case 0x19://cfg 
            ser_write_rsp_pkt(pdu_type);  /*if the related character has the property of WRITE(with response) or TYPE_CFG, one MUST invoke this func*/      
            break;
        
        default:
            att_notFd( pdu_type, attOpcode, att_hd );  /*the default response, also for the purpose of error robust */
            break;
    }
 }

///STEP2.1:Queued Writes data if any
void ser_prepare_write(u16 handle, u8* attValue, u16 attValueLen, u16 att_offset)//user's call back api 
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
unsigned char SimBatt=100;

///STEP3:Read data
//// read response
void server_rd_rsp(u8 attOpcode, u16 attHandle, u8 pdu_type)
{
    u8 Type[1] = {0x03};//finger
    u8  d_len;
    u8* ble_name = getDeviceInfoData(&d_len);
    
    arduino_c_print("Read something!");
    arduino_c_print("handle = ");
    arduino_c_print_num(attHandle);
    arduino_c_print_num(pdu_type);
    arduino_c_print_num(attOpcode);

    switch(attHandle) //hard code
    {
        case 0x04: //GAP name
            att_server_rd( pdu_type, attOpcode, attHandle, ble_name, d_len);
            break;
        
        case 0x09: //MANU_INFO
            //att_server_rd( pdu_type, attOpcode, attHandle, (u8*)(MANU_INFO), sizeof(MANU_INFO)-1);
            att_server_rd( pdu_type, attOpcode, attHandle, get_ble_version(), strlen((const char*)get_ble_version())); //ble lib build version
            break;
        
        case 0x0b: //FIRMWARE_INFO
            //do NOT modify this code!!!
            att_server_rd( pdu_type, attOpcode, attHandle, GetFirmwareInfo(),strlen((const char*)GetFirmwareInfo()));
            break;
        
        case 0x0f://SOFTWARE_INFO
            att_server_rd( pdu_type, attOpcode, attHandle, (u8*)(SOFTWARE_INFO), sizeof(SOFTWARE_INFO)-1);
            break;

        case 0x12:
            att_server_rd( pdu_type, attOpcode, attHandle, T_H_data.temp_str, strlen(T_H_data.temp_str));
            break;
        case 0x15:
            att_server_rd( pdu_type, attOpcode, attHandle, T_H_data.hum_str, strlen(T_H_data.hum_str));
            break;
        
        case 0x18:
            att_server_rd( pdu_type, attOpcode, attHandle, &SimBatt, 1);
            break;
        
        default:
            att_notFd( pdu_type, attOpcode, attHandle );/*the default response, also for the purpose of error robust */
            break;
    }
}

void server_blob_rd_rsp(u8 attOpcode, u16 attHandle, u8 dataHdrP,u16 offset)
{
}

//return 1 means found
int GetPrimaryServiceHandle(unsigned short hd_start, unsigned short hd_end,
                            unsigned short uuid16,
                            unsigned short* hd_start_r,unsigned short* hd_end_r)
{
    
    return 0;
}

static unsigned char HRMData[3]={
    0x00,0x00,0x01
};




uint8_t cnt;
void gatt_user_send_notify_data_callback(void)
{
    
    // static u8 Cont=0;
//     u16 Val=0;
    
//     Cont++;
//     if (Cont >= 20)
//     {
//         Cont = 0;
        
//         Val = Get_Adc_Average(ADC_Channel_3,5); //0~4095
//         Val = Val>>3; // 0~511 for HRM data
//         cur_notifyhandle = 0x12;
//         if (Val<0x100)
//         {
//             HRMData[0] = 0; //1Byte
//             HRMData[1] = Val;
//             sconn_notifydata(HRMData,2);
//         }
//         else
//         {
//             HRMData[0] = 1; //2Byte
//             HRMData[1] = Val;
//             HRMData[2] = Val>>8;
//             sconn_notifydata(HRMData,3);
//         }
//     }
//     else if (10 == Cont)
//     {
//         Val = Get_Adc_Average(ADC_Channel_3,5); //0~4095
//         Val = Val>>3;
//         SimBatt = (Val*100)>>9; //0~100
//         cur_notifyhandle = 0x18;
//         sconn_notifydata(&SimBatt,1);
// #ifdef USE_I2C
//         I2C_Send(&SimBatt, 1);
// #endif
//     }
}

static unsigned char gConnectedFlag=0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
u8* getsoftwareversion(void)
{
    return (u8*)SOFTWARE_INFO;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

char GetConnectedStatus(void)
{
    return gConnectedFlag;
}

//void LED_ONOFF(unsigned char onFlag);//module led indicator

void ConnectStausUpdate(unsigned char IsConnectedFlag) //porting api
{
    //[IsConnectedFlag] indicates the connection status
    
    LED_ONOFF(!IsConnectedFlag);

    if (IsConnectedFlag != gConnectedFlag)
    {
        gConnectedFlag = IsConnectedFlag;
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


