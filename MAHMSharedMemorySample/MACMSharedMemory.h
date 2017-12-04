/////////////////////////////////////////////////////////////////////////////
//
// This header file defines MSI Afterburner Control shared memory format
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _MACM_SHARED_MEMORY_INCLUDED_
#define _MACM_SHARED_MEMORY_INCLUDED_
/////////////////////////////////////////////////////////////////////////////
// v2.0 header
/////////////////////////////////////////////////////////////////////////////
#define	MACM_SHARED_MEMORY_FLAG_LINK								0x00000001
#define	MACM_SHARED_MEMORY_FLAG_SYNC								0x00000002
#define	MACM_SHARED_MEMORY_FLAG_LINK_THERMAL						0x00000004
/////////////////////////////////////////////////////////////////////////////
#define MACM_SHARED_MEMORY_COMMAND_INIT								0x00AB0000
#define MACM_SHARED_MEMORY_COMMAND_FLUSH							0x00AB0001
/////////////////////////////////////////////////////////////////////////////
typedef struct MACM_SHARED_MEMORY_HEADER
{
	DWORD	dwSignature;
		//signature allows applications to verify status of shared memory

		//The signature can be set to:
		//'MACM'	- control memory is initialized and contains 
		//			valid data 
		//0xDEAD	- control memory is marked for deallocation and
		//			no longer contain valid data
		//otherwise the memory is not initialized
	DWORD	dwVersion;
		//header version ((major<<16) + minor)
		//must be set to 0x00020000 for v2.0
	DWORD	dwHeaderSize;
		//size of header
	DWORD	dwNumGpuEntries;
		//number of subsequent MACM_SHARED_MEMORY_GPU_ENTRY entries
	DWORD	dwGpuEntrySize;
		//size of entries in subsequent MACM_SHARED_MEMORY_GPU_ENTRY entries array

	DWORD	dwMasterGpu;
		//master GPU index
	DWORD	dwFlags;	
		//bitmask containing combination of MACM_SHARED_MEMORY_FLAG_...  flags

	time_t	time;
		//control memory timestamp 

		//The timestamp is refreshed each time new hardware settings are
		//being applied, so you can latch the timestamp and periodically
		//check it to monitor settings changes from Afterburner side
		
		//WARNING! Force 32-bit time_t usage with #define _USE_32BIT_TIME_T 
		//to provide compatibility with VC8.0 and newer compiler versions

	DWORD	dwCommand;
		//write MACM_SHARED_MEMORY_COMMAND_... to this field to force 
		//MSI Afterburner to perform the following actions on the next 
		//hardware polling iteration:
		//
		//MACM_SHARED_MEMORY_COMMAND_INIT - force MSI Afterburner to
		//reinitialize control memory with current hardware settings
		//
		//MACM_SHARED_MEMORY_COMMAND_FLUSH - force MSI Afterburner to
		//flush settings from control memory and apply it to hardware
		//then read back applied settings from hardware
		//
		//NOTE: MSI Afterburner reset this field to 0 after executing
		//the command


} MACM_SHARED_MEMORY_HEADER, *LPMACM_SHARED_MEMORY_HEADER;
/////////////////////////////////////////////////////////////////////////////
#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_CORE_CLOCK				0x00000001
#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_SHADER_CLOCK				0x00000002
#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_MEMORY_CLOCK				0x00000004
#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_FAN_SPEED					0x00000008
#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_CORE_VOLTAGE				0x00000010
#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_MEMORY_VOLTAGE			0x00000020
#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_AUX_VOLTAGE				0x00000040
#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_CORE_VOLTAGE_BOOST		0x00000080
#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_MEMORY_VOLTAGE_BOOST		0x00000100
#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_AUX_VOLTAGE_BOOST			0x00000200
#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_POWER_LIMIT				0x00000400
#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_CORE_CLOCK_BOOST			0x00000800
#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_MEMORY_CLOCK_BOOST		0x00001000
#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_THERMAL_LIMIT				0x00002000
#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_THERMAL_PRIORITIZE		0x00004000

#define	MACM_SHARED_MEMORY_GPU_ENTRY_FLAG_SYNCHRONIZED_WITH_MASTER	0x80000000
/////////////////////////////////////////////////////////////////////////////
#define MACM_SHARED_MEMORY_GPU_ENTRY_FAN_FLAG_AUTO					0x00000001
/////////////////////////////////////////////////////////////////////////////
// v2.0 entry (array of entries follows immediately by the header)
typedef struct MACM_SHARED_MEMORY_GPU_ENTRY
{
	DWORD	dwFlags;
		//bitmask containing combination of MACM_SHARED_GPU_MEMORY_ENTRY_FLAG_...

	//clock control

	DWORD dwCoreClockCur;
		//current core clock in KHz
	DWORD dwCoreClockMin;
		//minimum core clock in KHz
	DWORD dwCoreClockMax;
		//maximum core clock in KHz
	DWORD dwCoreClockDef;
		//default core clock in KHz

	DWORD dwShaderClockCur;
		//current shader clock in KHz
	DWORD dwShaderClockMin;
		//minimum shader clock in KHz
	DWORD dwShaderClockMax;
		//maximum shader clock in KHz
	DWORD dwShaderClockDef;
		//default shader clock in KHz

	DWORD dwMemoryClockCur;
		//current memory clock in KHz
	DWORD dwMemoryClockMin;
		//minimum memory clock in KHz
	DWORD dwMemoryClockMax;
		//maximum memory clock in KHz
	DWORD dwMemoryClockDef;
		//default memory clock in KHz

	//fan speed control

	DWORD dwFanSpeedCur;
		//current fan speed in %
	DWORD dwFanFlagsCur;
		//bitmask containing combination of MACM_SHARED_MEMORY_GPU_ENTRY_FAN_FLAG_...  flags
	DWORD dwFanSpeedMin;
		//minimum fan speed in %
	DWORD dwFanSpeedMax;
		//maximum fan speed in %
	DWORD dwFanSpeedDef;
		//default fan speed in %
	DWORD dwFanFlagsDef;
		//default bitmask containing combination of MACM_SHARED_MEMORY_GPU_ENTRY_FAN_FLAG_...  flags

	//voltage control

	DWORD dwCoreVoltageCur;
		//current core voltage in mV
	DWORD dwCoreVoltageMin;
		//minimum core voltage in mV
	DWORD dwCoreVoltageMax;
		//maximum core voltage in mV
	DWORD dwCoreVoltageDef;
		//default core voltage in mV

	DWORD dwMemoryVoltageCur;
		//current memory voltage in mV
	DWORD dwMemoryVoltageMin;
		//minimum memory voltage in mV
	DWORD dwMemoryVoltageMax;
		//maximum memory voltage in mV
	DWORD dwMemoryVoltageDef;
		//default memory voltage in mV

	DWORD dwAuxVoltageCur;
		//current aux voltage in mV
	DWORD dwAuxVoltageMin;
		//minimum aux voltage in mV
	DWORD dwAuxVoltageMax;
		//maximum aux voltage in mV
	DWORD dwAuxVoltageDef;
		//default aux voltage in mV

	//voltage boost control

	LONG dwCoreVoltageBoostCur;
		//current core voltage boost in mV
	LONG dwCoreVoltageBoostMin;
		//minimum core voltage boost in mV
	LONG dwCoreVoltageBoostMax;
		//maximum core voltage boost in mV
	LONG dwCoreVoltageBoostDef;
		//default core voltage boost in mV

	LONG dwMemoryVoltageBoostCur;
		//current memory voltage boost in mV
	LONG dwMemoryVoltageBoostMin;
		//minimum memory voltage boost in mV
	LONG dwMemoryVoltageBoostMax;
		//maximum memory voltage boost in mV
	LONG dwMemoryVoltageBoostDef;
		//default memory voltage boost in mV

	LONG dwAuxVoltageBoostCur;
		//current aux voltage boost in mV
	LONG dwAuxVoltageBoostMin;
		//minimum aux voltage boost in mV
	LONG dwAuxVoltageBoostMax;
		//maximum aux voltage boost in mV
	LONG dwAuxVoltageBoostDef;
		//default aux voltage boost in mV

	//the following fields are only valid for v2.0 and newer version

	//power limit control

	LONG dwPowerLimitCur;
		//current power limit in %
	LONG dwPowerLimitMin;
		//minimum power limit in %
	LONG dwPowerLimitMax;
		//maximum power limit in %
	LONG dwPowerLimitDef;
		//default power limit in %

	//clock boost control

	LONG dwCoreClockBoostCur;
		//current core clock boost in KHz
	LONG dwCoreClockBoostMin;
		//minimum core clock boost in KHz
	LONG dwCoreClockBoostMax;
		//maximum core clock boost in KHz
	LONG dwCoreClockBoostDef;
		//default core clock boost in KHz

	LONG dwMemoryClockBoostCur;
		//current memory clock boost in KHz
	LONG dwMemoryClockBoostMin;
		//minimum memory clock boost in KHz
	LONG dwMemoryClockBoostMax;
		//maximum memory clock boost in KHz
	LONG dwMemoryClockBoostDef;
		//default memory clock boost in KHz

	//the following fields are only valid for v2.1 and newer version

	//thermal limit control

	LONG dwThermalLimitCur;
		//current thermal limit in C
	LONG dwThermalLimitMin;
		//minimum thermal limit in C
	LONG dwThermalLimitMax;
		//maximum thermal limit in C
	LONG dwThermalLimitDef;
		//default thermal limit in C

	//thermal prioritize control 

	DWORD dwThermalPrioritizeCur;
		//current thermal prioritize state
	DWORD dwThermalPrioritizeDef;
		//default thermal prioritize state

} MACM_SHARED_MEMORY_GPU_ENTRY, *LPMACM_SHARED_MEMORY_GPU_ENTRY;
/////////////////////////////////////////////////////////////////////////////
#endif //_MACM_SHARED_MEMORY_INCLUDED_