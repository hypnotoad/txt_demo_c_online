///////////////////////////////////////////////////////////////////////////////
//
// File:    ftProInterface2013SoecketCom.h
//
// Project: ftPro - fischertechnik Control Graphical Programming System
//
// Module:  TCP/IP communication protocol with interface 2013
//
// Author:  Michael S�gtrop
//
///////////////////////////////////////////////////////////////////////////////
//
// Urheberrecht im Sinne des Urheberrechtsgesetzes bei
//
// Michael S�gtrop
// Germany
//
// Copyright (C) 2014
//
///////////////////////////////////////////////////////////////////////////////
//
// TO DO:
//
///////////////////////////////////////////////////////////////////////////////
//
// Usage details for module ftProInterface2013SoecketCom
//
// see also:
//
///////////////////////////////////////////////////////////////////////////////

// Double inclusion protection 
#if(!defined(ftProInterface2013SocketCom_H))
#define ftProInterface2013SocketCom_H

#ifdef WIN32
#pragma warning (push)
#pragma warning (disable:4200)
#endif

#include "common.h"

//******************************************************************************
//*
//* Definition of constants
//*
//******************************************************************************

enum
{
    ftIF2013_nPwmOutputs = 8,
    ftIF2013_nMotorOutputs = 4,
    ftIF2013_nCounters = 4,
    ftIF2013_nUniversalInputs = 8,
    ftIF2013_nIRChannels = 4
};

//******************************************************************************
//*
//* Definitions
//*
//******************************************************************************

// The commands are 4 byte random numbers and serve as command and magic
enum ftIF2013CommandId
{
    ftIF2013CommandId_QueryStatus = 0xDC21219A,

    ftIF2013CommandId_StartOnline = 0x163FF61D,
    ftIF2013CommandId_UpdateConfig = 0x060EF27E,
    ftIF2013CommandId_ExchangeData = 0xCC3597BA,
    ftIF2013CommandId_ExchangeDataCmpr = 0xFBC56F98,
    ftIF2013CommandId_StopOnline = 0x9BE5082C,

    ftIF2013CommandId_StartCameraOnline = 0x882A40A6,
    ftIF2013CommandId_StopCameraOnline = 0x17C31F2F,

    // Used in camera channel
    ftIF2013AcknowledgeId_CameraOnlineFrame = 0xADA09FBA
};
// The response ids are 4 byte random numbers and serve as packet id and magic
enum ftIF2013ResponseId
{
    ftIF2013ResponseId_QueryStatus = 0xBAC9723E,

    ftIF2013ResponseId_StartOnline = 0xCA689F75,
    ftIF2013ResponseId_UpdateConfig = 0x9689A68C,
    ftIF2013ResponseId_ExchangeData = 0x4EEFAC41,
    ftIF2013ResponseId_ExchangeDataCmpr = 0x6F3B54E6,
    ftIF2013ResponseId_StopOnline = 0xFBF600D2,

    ftIF2013ResponseId_StartCameraOnline = 0xCF41B24E,
    ftIF2013ResponseId_StopCameraOnline = 0x4B3C1EB6,

    // Used in camera channel
    ftIF2013DataId_CameraOnlineFrame = 0xBDC2D7A1,
};

// Base class for command packets

struct ftIF2013Command_Base
{
    ftIF2013CommandId m_id;
};

// Base class for response packets

struct ftIF2013Response_Base
{
    ftIF2013ResponseId m_id;
};

// Base class for command packets with extra data

struct ftIF2013Command_BaseEx : ftIF2013Command_Base
{
    UINT32 m_extrasize;
};

// Base class for response packets with extra data

struct ftIF2013Response_BaseEx : ftIF2013Response_Base
{
    UINT32 m_extrasize;
};

// Command packet for QueryStatus Command

struct ftIF2013Command_QueryStatus : public ftIF2013Command_Base
{
};

// Response packet for QueryStatus Command

struct ftIF2013Response_QueryStatus : public ftIF2013Response_Base
{
    char m_devicename[16];
    unsigned int m_version;
};

// Command packet for StartOnline Command

struct ftIF2013Command_StartOnline : public ftIF2013Command_Base
{
    CCHAR8 m_name[64];
};

// Response packet for StartOnline Command

struct ftIF2013Response_StartOnline : public ftIF2013Response_Base
{
};

// Command packet for UpdateConfig Command

struct ftIF2013Command_UpdateConfig : public ftIF2013Command_Base
{
    // config id from FTX1_CONFIG
    INT16 m_config_id;
    INT16 m_extension_id;
    FTX1_CONFIG m_config;
};

// Response packet for UpdateConfig Command

struct ftIF2013Response_UpdateConfig : public ftIF2013Response_Base
{
};

// Command packet for ExchangeData Command

struct ftIF2013Command_ExchangeData : public ftIF2013Command_Base
{
    // Motor / Output PWM data
    // Also for bipolar motor outputs both values are used
    INT16 m_pwmOutputValues[ftIF2013_nPwmOutputs];
    INT16 m_motor_master[ftIF2013_nMotorOutputs];
    INT16 m_motor_distance[ftIF2013_nMotorOutputs];
    INT16 m_motor_command_id[ftIF2013_nMotorOutputs];
    INT16 m_counter_reset_command_id[ftIF2013_nCounters];
    UINT16  m_sound_command_id;
    UINT16  m_sound_index;
    UINT16  m_sound_repeat;
};

// Response packet for ExchangeData Command

struct ftIF2013Response_ExchangeData : public ftIF2013Response_Base
{
    // Universal input values
    INT16 m_universalInputs[ftIF2013_nUniversalInputs];
    INT16 m_counter_input[ftIF2013_nCounters];
    INT16 m_counter_value[ftIF2013_nCounters];
    INT16 m_counter_command_id[ftIF2013_nCounters];
    INT16 m_motor_command_id[ftIF2013_nMotorOutputs];
    UINT16  m_sound_command_id;
    struct IR
    {
        INT8  m_ir_leftX;  // left  handle, horizontal, -15..15
        INT8  m_ir_leftY;  // left  handle, vertical,   -15..15
        INT8  m_ir_rightX; // right handle, horizontal, -15..15
        INT8  m_ir_rightY; // right handle, vertical,   -15..15
        UINT8 m_ir_bits;   // 2^0=on, 2^1=off, 2^2=switch1, 2^3=switch2
    } m_ir[5];
};

// Command packet for ExchangeDataCmpr Command

struct ftIF2013Command_ExchangeDataCmpr : public ftIF2013Command_BaseEx
{
    // CRC16 of decompressed data
    UINT32 m_crc;
    // One bit for each connected extension
    // Data is transmitted only for the master and for the bits set here
    UINT16 m_active_extensions;
    UINT16 m_dmy_align;
    // Compressed data
    // Data is compressed as 16 bit words source to a 2 bit code, MSB first
    // 00 NoChange 1x16 bit
    // 01 00 NoChange 2x16 bit
    // 01 01 NoChange 3x16 bit
    // 01 10 NoChange 4x16 bit
    // 01 11 xxxx NoChange 5..19x16 bit
    // 01 11 1111 xxxxxxxx NoChange 20..274 x16 bit
    // 01 11 1111 11111111 xxxxxxxx-xxxxxxxx NoChange 275... x16 bit
    // 10 Toggle (0 to 1, everything else to 0)
    // 11 16 bit follow immediately
    // For each active extension, the following data is encoded in the order given below:
    // INT16 m_pwmOutputValues[ftIF2013_nPwmOutputs];
    // INT16 m_motor_master[ftIF2013_nMotorOutputs];
    // INT16 m_motor_distance[ftIF2013_nMotorOutputs];
    // INT16 m_motor_command_id[ftIF2013_nMotorOutputs];
    // INT16 m_counter_reset_command_id[ftIF2013_nCounters];
    // UINT16  m_sound_command_id;
    // UINT16  m_sound_index;
    // UINT16  m_sound_repeat;
    UINT8 m_data[0];
};

// Response packet for ExchangeDataCmpr Command

struct ftIF2013Response_ExchangeDataCmpr : public ftIF2013Response_BaseEx
{
    // CRC16 of decompressed data
    UINT32 m_crc;
    // One bit for each connected extension
    // Data is transmitted only for the master and for the bits set here
    UINT16 m_active_extensions;
    UINT16 m_dmy_align;
    // Compressed data
    // See ftIF2013Command_ExchangeData for compression scheme
    // INT16 m_universalInputs[ftIF2013_nUniversalInputs];
    // INT16 m_counter_input[ftIF2013_nCounters];
    // INT16 m_counter_value[ftIF2013_nCounters];
    // INT16 m_counter_command_id[ftIF2013_nCounters];
    // INT16 m_motor_command_id[ftIF2013_nMotorOutputs];
    // UINT16  m_sound_command_id;
    // struct IR
    // {
    //     INT16  m_ir_leftX;  // left  handle, horizontal, -15..15
    //     INT16  m_ir_leftY;  // left  handle, vertical,   -15..15
    //     INT16  m_ir_rightX; // right handle, horizontal, -15..15
    //     INT16  m_ir_rightY; // right handle, vertical,   -15..15
    //     UINT16 m_ir_bits;   // 2^0=on, 2^1=off, 2^2=switch1, 2^3=switch2
    // } m_ir[5];
    UINT8 m_data[0];
};

// Command packet for StopOnline Command

struct ftIF2013Command_StopOnline : public ftIF2013Command_Base
{
};

// Response packet for StopOnline Command

struct ftIF2013Response_StopOnline : public ftIF2013Response_Base
{
};



// Command packet for StartCameraOnline Command

struct ftIF2013Command_StartCameraOnline : public ftIF2013Command_Base
{
    INT32 m_width;
    INT32 m_height;
    INT32 m_framerate;
    INT32 m_powerlinefreq; // 0=auto, 1=50Hz, 2=60Hz
};

// Response packet for StartCameraOnline Command

struct ftIF2013Response_StartCameraOnline : public ftIF2013Response_Base
{
};

// Command packet for CameraOnlineFrame Command

struct ftIF2013Acknowledge_CameraOnlineFrame : public ftIF2013Command_Base
{
};

// Response packet for CameraOnlineFrame Command

struct ftIF2013Response_CameraOnlineFrame : public ftIF2013Response_Base
{
    INT32 m_numframeready;
    INT16 m_framewidth;
    INT16 m_frameheight;
    INT32 m_framesizeraw;
    INT32 m_framesizecompressed;
    UINT8 m_framedata[0];
};


// Command packet for StopCameraOnline Command

struct ftIF2013Command_StopCameraOnline : public ftIF2013Command_Base
{
};

// Response packet for StopCameraOnline Command

struct ftIF2013Response_StopCameraOnline : public ftIF2013Response_Base
{
};

// Computation of CRC32 for a stream of 16 bit numbers
struct CRC32
{
    CRC32();
    void Reset();
    void Add16bit(UINT16 val);
    UINT32 m_crc;
    UINT32 m_table[256];
};

// Expansion of IO data
class ExpansionBuffer
{
public:
    ExpansionBuffer( UINT8 *buffer, int bufsize );
    void Reset();
    void Rewind();
    UINT32 GetBits( INT32 count );
    UINT16 GetUINT16();
    INT16 GetINT16() { return (INT16) GetUINT16(); }
    UINT32 GetCrc() { return m_crc.m_crc; }
    bool GetError() { return m_error; }
    UINT8 *GetBuffer() { return m_compressed; }
    UINT32 GetMaxBufferSize() { return max_compressed_size; }
    void SetBufferSize(UINT32 size) { m_compressed_size_limit = size; }

protected:
    enum {max_word_count=4096};
    UINT16 m_previous_words[max_word_count];
    UINT32 max_compressed_size;
    UINT8 *m_compressed;
    INT32 m_word_count;
    UINT32 m_compressed_size;
    UINT32 m_compressed_size_limit;
    INT32 m_nochange_count;
    UINT32 m_bitbuffer;
    INT32 m_bitcount;
    CRC32 m_crc;
    bool m_error;
};

// Compression of IO data
class CompressionBuffer
{
public:
    CompressionBuffer(UINT8 *buffer, int bufsize);
    ~CompressionBuffer();
    void Reset();
    void Rewind();
    void PushBits( INT32 count, UINT32 bits );
    void EncodeNoChangeCount();
    void AddWord(UINT16 word);
    void Finish();
    UINT32 GetCrc() { return m_crc.m_crc; }
    bool GetError() { return m_error; }
    UINT32 GetCompressedSize() { return m_compressed_size; }
    const UINT8 *GetBuffer() { return m_compressed; }
    INT32 GetWordCount() { return m_word_count; }
    UINT16 GetPrevWord(INT32 i) { return m_previous_words[i]; }

protected:
    enum {max_word_count=4096};
    UINT16 m_previous_words[max_word_count];
    UINT32 max_compressed_size;
    UINT8 *m_compressed;
    INT32 m_word_count;
    UINT32 m_compressed_size;
    INT32 m_nochange_count;
    UINT32 m_bitbuffer;
    INT32 m_bitcount;
    CRC32 m_crc;
    bool m_error;
#ifdef _DEBUG
    ExpansionBuffer m_check_expand;
#endif
};

#ifdef WIN32
#pragma warning (pop)
#endif

#endif //ftProInterface2013SocketCom_H
