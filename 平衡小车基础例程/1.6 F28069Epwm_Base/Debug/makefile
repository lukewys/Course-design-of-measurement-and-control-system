################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/ti/ccsv6/tools/compiler/c2000_15.12.3.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main.obj" \
"./source/F2806x_Adc.obj" \
"./source/F2806x_CodeStartBranch.obj" \
"./source/F2806x_Comp.obj" \
"./source/F2806x_CpuTimers.obj" \
"./source/F2806x_DBGIER.obj" \
"./source/F2806x_DefaultIsr.obj" \
"./source/F2806x_DisInt.obj" \
"./source/F2806x_Dma.obj" \
"./source/F2806x_ECan.obj" \
"./source/F2806x_ECap.obj" \
"./source/F2806x_EPwm.obj" \
"./source/F2806x_EQep.obj" \
"./source/F2806x_GlobalVariableDefs.obj" \
"./source/F2806x_Gpio.obj" \
"./source/F2806x_HRCap.obj" \
"./source/F2806x_I2C.obj" \
"./source/F2806x_Mcbsp.obj" \
"./source/F2806x_OscComp.obj" \
"./source/F2806x_PieCtrl.obj" \
"./source/F2806x_PieVect.obj" \
"./source/F2806x_Sci.obj" \
"./source/F2806x_Spi.obj" \
"./source/F2806x_SysCtrl.obj" \
"./source/F2806x_TempSensorConv.obj" \
"./source/F2806x_usDelay.obj" \
"../28069_RAM_lnk.cmd" \
"../F2806x_Headers_nonBIOS.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include source/subdir_vars.mk
-include subdir_rules.mk
-include source/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
1.6\ F28069Epwm_Base.out \

EXE_OUTPUTS__QUOTED += \
"1.6 F28069Epwm_Base.out" \

BIN_OUTPUTS += \
1.6\ F28069Epwm_Base.hex \

BIN_OUTPUTS__QUOTED += \
"1.6 F28069Epwm_Base.hex" \


# All Target
all: 1.6\ F28069Epwm_Base.out

# Tool invocations
1.6\ F28069Epwm_Base.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C2000 Linker'
	"D:/ti/ccsv6/tools/compiler/c2000_15.12.3.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --vcu_support=vcu0 --float_support=fpu32 -g --display_error_number --diag_warning=225 --diag_wrap=off -z -m"1.6 F28069Epwm_Base.map" --stack_size=0x300 --warn_sections -i"D:/ti/ccsv6/tools/compiler/c2000_15.12.3.LTS/lib" -i"D:/ti/ccsv6/tools/compiler/c2000_15.12.3.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="1.6 F28069Epwm_Base_linkInfo.xml" --rom_model -o "1.6 F28069Epwm_Base.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

1.6\ F28069Epwm_Base.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: C2000 Hex Utility'
	"D:/ti/ccsv6/tools/compiler/c2000_15.12.3.LTS/bin/hex2000"  -o "1.6 F28069Epwm_Base.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "main.d" "source\F2806x_Adc.d" "source\F2806x_Comp.d" "source\F2806x_CpuTimers.d" "source\F2806x_DefaultIsr.d" "source\F2806x_Dma.d" "source\F2806x_ECan.d" "source\F2806x_ECap.d" "source\F2806x_EPwm.d" "source\F2806x_EQep.d" "source\F2806x_GlobalVariableDefs.d" "source\F2806x_Gpio.d" "source\F2806x_HRCap.d" "source\F2806x_I2C.d" "source\F2806x_Mcbsp.d" "source\F2806x_OscComp.d" "source\F2806x_PieCtrl.d" "source\F2806x_PieVect.d" "source\F2806x_Sci.d" "source\F2806x_Spi.d" "source\F2806x_SysCtrl.d" "source\F2806x_TempSensorConv.d" 
	-$(RM) "main.obj" "source\F2806x_Adc.obj" "source\F2806x_CodeStartBranch.obj" "source\F2806x_Comp.obj" "source\F2806x_CpuTimers.obj" "source\F2806x_DBGIER.obj" "source\F2806x_DefaultIsr.obj" "source\F2806x_DisInt.obj" "source\F2806x_Dma.obj" "source\F2806x_ECan.obj" "source\F2806x_ECap.obj" "source\F2806x_EPwm.obj" "source\F2806x_EQep.obj" "source\F2806x_GlobalVariableDefs.obj" "source\F2806x_Gpio.obj" "source\F2806x_HRCap.obj" "source\F2806x_I2C.obj" "source\F2806x_Mcbsp.obj" "source\F2806x_OscComp.obj" "source\F2806x_PieCtrl.obj" "source\F2806x_PieVect.obj" "source\F2806x_Sci.obj" "source\F2806x_Spi.obj" "source\F2806x_SysCtrl.obj" "source\F2806x_TempSensorConv.obj" "source\F2806x_usDelay.obj" 
	-$(RM) "source\F2806x_CodeStartBranch.d" "source\F2806x_DBGIER.d" "source\F2806x_DisInt.d" "source\F2806x_usDelay.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

