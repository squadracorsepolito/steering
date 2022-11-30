// SDRAM

#ifndef MT48LC4M32B2_H
#define MT48LC4M32B2_H

#include "mt48lc4m32b2_conf.h"

typedef enum
{
    MT48LC4M32B2_ERROR = -1,
    MT48LC4M32B2_OK = 0
} MT48LC4M32B2_StatusTypeDef;

typedef struct
{
    uint32_t TargetBank;     /* Target Bank */
    uint32_t RefreshMode;    /* Refresh Mode */
    uint32_t RefreshRate;    /* Refresh Rate */
    uint32_t BurstLength;    /* Burst Length */
    uint32_t BurstType;      /* Burst Type */
    uint32_t CASLatency;     /* CAS Latency */
    uint32_t OperationMode;  /* Operation Mode */
    uint32_t WriteBurstMode; /* Write Burst Mode */
} MT48LC4M32B2_ContextTypeDef;

/* Register Mode */
#define MT48LC4M32B2_BURST_LENGTH_1 0x00000000U
#define MT48LC4M32B2_BURST_LENGTH_2 0x00000001U
#define MT48LC4M32B2_BURST_LENGTH_4 0x00000002U
#define MT48LC4M32B2_BURST_LENGTH_8 0x00000004U
#define MT48LC4M32B2_BURST_TYPE_SEQUENTIAL 0x00000000U
#define MT48LC4M32B2_BURST_TYPE_INTERLEAVED 0x00000008U
#define MT48LC4M32B2_CAS_LATENCY_2 0x00000020U
#define MT48LC4M32B2_CAS_LATENCY_3 0x00000030U
#define MT48LC4M32B2_OPERATING_MODE_STANDARD 0x00000000U
#define MT48LC4M32B2_WRITEBURST_MODE_PROGRAMMED 0x00000000U
#define MT48LC4M32B2_WRITEBURST_MODE_SINGLE 0x00000200U

/* Command Mode */
#define MT48LC4M32B2_NORMAL_MODE_CMD 0x00000000U
#define MT48LC4M32B2_CLK_ENABLE_CMD 0x00000001U
#define MT48LC4M32B2_PALL_CMD 0x00000002U
#define MT48LC4M32B2_AUTOREFRESH_MODE_CMD 0x00000003U
#define MT48LC4M32B2_LOAD_MODE_CMD 0x00000004U
#define MT48LC4M32B2_SELFREFRESH_MODE_CMD 0x00000005U
#define MT48LC4M32B2_POWERDOWN_MODE_CMD 0x00000006U

MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_Init(SDRAM_HandleTypeDef *Ctx, MT48LC4M32B2_ContextTypeDef *pRegMode);
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_ClockEnable(SDRAM_HandleTypeDef *Ctx, uint32_t Interface);
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_Precharge(SDRAM_HandleTypeDef *Ctx, uint32_t Interface);
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_ModeRegConfig(SDRAM_HandleTypeDef *Ctx, MT48LC4M32B2_ContextTypeDef *pRegMode);
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_TimingConfig(SDRAM_HandleTypeDef *Ctx, FMC_SDRAM_TimingTypeDef *pTiming);
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_RefreshMode(SDRAM_HandleTypeDef *Ctx, uint32_t Interface, uint32_t RefreshMode);
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_RefreshRate(SDRAM_HandleTypeDef *Ctx, uint32_t RefreshCount);
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_EnterPowerMode(SDRAM_HandleTypeDef *Ctx, uint32_t Interface);
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_ExitPowerMode(SDRAM_HandleTypeDef *Ctx, uint32_t Interface);
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_Sendcmd(SDRAM_HandleTypeDef *Ctx, FMC_SDRAM_CommandTypeDef *SdramCmd);

#endif /* MT48LC4M32B2_H */