#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-PIC16F1936_xc8_1_38_no_BL.mk)" "nbproject/Makefile-local-PIC16F1936_xc8_1_38_no_BL.mk"
include nbproject/Makefile-local-PIC16F1936_xc8_1_38_no_BL.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=PIC16F1936_xc8_1_38_no_BL
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=${DISTDIR}/GPSDetector.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=${DISTDIR}/GPSDetector.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=--mafrlcsj
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=src/main.c src/Init_all.c src/UART.c src/Clock.c src/configuration_bits.c src/interrupt_isr_file.c src/ADC.c src/handlers.c src/ring_buffer.c src/rx_luz.c src/my_assert.c src/device_driver_config.c src/tilt_sensor.c src/timers.c src/gd_states.c src/gps.c src/extension_strings.c src/e_rtc.c src/eeprom.c src/DDS.c src/messages.c src/bit_banged_uart.c src/detector.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/src/main.p1 ${OBJECTDIR}/src/Init_all.p1 ${OBJECTDIR}/src/UART.p1 ${OBJECTDIR}/src/Clock.p1 ${OBJECTDIR}/src/configuration_bits.p1 ${OBJECTDIR}/src/interrupt_isr_file.p1 ${OBJECTDIR}/src/ADC.p1 ${OBJECTDIR}/src/handlers.p1 ${OBJECTDIR}/src/ring_buffer.p1 ${OBJECTDIR}/src/rx_luz.p1 ${OBJECTDIR}/src/my_assert.p1 ${OBJECTDIR}/src/device_driver_config.p1 ${OBJECTDIR}/src/tilt_sensor.p1 ${OBJECTDIR}/src/timers.p1 ${OBJECTDIR}/src/gd_states.p1 ${OBJECTDIR}/src/gps.p1 ${OBJECTDIR}/src/extension_strings.p1 ${OBJECTDIR}/src/e_rtc.p1 ${OBJECTDIR}/src/eeprom.p1 ${OBJECTDIR}/src/DDS.p1 ${OBJECTDIR}/src/messages.p1 ${OBJECTDIR}/src/bit_banged_uart.p1 ${OBJECTDIR}/src/detector.p1
POSSIBLE_DEPFILES=${OBJECTDIR}/src/main.p1.d ${OBJECTDIR}/src/Init_all.p1.d ${OBJECTDIR}/src/UART.p1.d ${OBJECTDIR}/src/Clock.p1.d ${OBJECTDIR}/src/configuration_bits.p1.d ${OBJECTDIR}/src/interrupt_isr_file.p1.d ${OBJECTDIR}/src/ADC.p1.d ${OBJECTDIR}/src/handlers.p1.d ${OBJECTDIR}/src/ring_buffer.p1.d ${OBJECTDIR}/src/rx_luz.p1.d ${OBJECTDIR}/src/my_assert.p1.d ${OBJECTDIR}/src/device_driver_config.p1.d ${OBJECTDIR}/src/tilt_sensor.p1.d ${OBJECTDIR}/src/timers.p1.d ${OBJECTDIR}/src/gd_states.p1.d ${OBJECTDIR}/src/gps.p1.d ${OBJECTDIR}/src/extension_strings.p1.d ${OBJECTDIR}/src/e_rtc.p1.d ${OBJECTDIR}/src/eeprom.p1.d ${OBJECTDIR}/src/DDS.p1.d ${OBJECTDIR}/src/messages.p1.d ${OBJECTDIR}/src/bit_banged_uart.p1.d ${OBJECTDIR}/src/detector.p1.d

# Object Files
OBJECTFILES=${OBJECTDIR}/src/main.p1 ${OBJECTDIR}/src/Init_all.p1 ${OBJECTDIR}/src/UART.p1 ${OBJECTDIR}/src/Clock.p1 ${OBJECTDIR}/src/configuration_bits.p1 ${OBJECTDIR}/src/interrupt_isr_file.p1 ${OBJECTDIR}/src/ADC.p1 ${OBJECTDIR}/src/handlers.p1 ${OBJECTDIR}/src/ring_buffer.p1 ${OBJECTDIR}/src/rx_luz.p1 ${OBJECTDIR}/src/my_assert.p1 ${OBJECTDIR}/src/device_driver_config.p1 ${OBJECTDIR}/src/tilt_sensor.p1 ${OBJECTDIR}/src/timers.p1 ${OBJECTDIR}/src/gd_states.p1 ${OBJECTDIR}/src/gps.p1 ${OBJECTDIR}/src/extension_strings.p1 ${OBJECTDIR}/src/e_rtc.p1 ${OBJECTDIR}/src/eeprom.p1 ${OBJECTDIR}/src/DDS.p1 ${OBJECTDIR}/src/messages.p1 ${OBJECTDIR}/src/bit_banged_uart.p1 ${OBJECTDIR}/src/detector.p1

# Source Files
SOURCEFILES=src/main.c src/Init_all.c src/UART.c src/Clock.c src/configuration_bits.c src/interrupt_isr_file.c src/ADC.c src/handlers.c src/ring_buffer.c src/rx_luz.c src/my_assert.c src/device_driver_config.c src/tilt_sensor.c src/timers.c src/gd_states.c src/gps.c src/extension_strings.c src/e_rtc.c src/eeprom.c src/DDS.c src/messages.c src/bit_banged_uart.c src/detector.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-PIC16F1936_xc8_1_38_no_BL.mk ${DISTDIR}/GPSDetector.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=16F1936
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/src/main.p1: src/main.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/main.p1.d 
	@${RM} ${OBJECTDIR}/src/main.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/main.p1 src/main.c 
	@-${MV} ${OBJECTDIR}/src/main.d ${OBJECTDIR}/src/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/Init_all.p1: src/Init_all.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Init_all.p1.d 
	@${RM} ${OBJECTDIR}/src/Init_all.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/Init_all.p1 src/Init_all.c 
	@-${MV} ${OBJECTDIR}/src/Init_all.d ${OBJECTDIR}/src/Init_all.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/Init_all.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/UART.p1: src/UART.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/UART.p1.d 
	@${RM} ${OBJECTDIR}/src/UART.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/UART.p1 src/UART.c 
	@-${MV} ${OBJECTDIR}/src/UART.d ${OBJECTDIR}/src/UART.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/UART.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/Clock.p1: src/Clock.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Clock.p1.d 
	@${RM} ${OBJECTDIR}/src/Clock.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/Clock.p1 src/Clock.c 
	@-${MV} ${OBJECTDIR}/src/Clock.d ${OBJECTDIR}/src/Clock.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/Clock.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/configuration_bits.p1: src/configuration_bits.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/configuration_bits.p1.d 
	@${RM} ${OBJECTDIR}/src/configuration_bits.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/configuration_bits.p1 src/configuration_bits.c 
	@-${MV} ${OBJECTDIR}/src/configuration_bits.d ${OBJECTDIR}/src/configuration_bits.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/configuration_bits.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/interrupt_isr_file.p1: src/interrupt_isr_file.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/interrupt_isr_file.p1.d 
	@${RM} ${OBJECTDIR}/src/interrupt_isr_file.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/interrupt_isr_file.p1 src/interrupt_isr_file.c 
	@-${MV} ${OBJECTDIR}/src/interrupt_isr_file.d ${OBJECTDIR}/src/interrupt_isr_file.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/interrupt_isr_file.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/ADC.p1: src/ADC.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/ADC.p1.d 
	@${RM} ${OBJECTDIR}/src/ADC.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/ADC.p1 src/ADC.c 
	@-${MV} ${OBJECTDIR}/src/ADC.d ${OBJECTDIR}/src/ADC.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/ADC.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/handlers.p1: src/handlers.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/handlers.p1.d 
	@${RM} ${OBJECTDIR}/src/handlers.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/handlers.p1 src/handlers.c 
	@-${MV} ${OBJECTDIR}/src/handlers.d ${OBJECTDIR}/src/handlers.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/handlers.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/ring_buffer.p1: src/ring_buffer.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/ring_buffer.p1.d 
	@${RM} ${OBJECTDIR}/src/ring_buffer.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/ring_buffer.p1 src/ring_buffer.c 
	@-${MV} ${OBJECTDIR}/src/ring_buffer.d ${OBJECTDIR}/src/ring_buffer.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/ring_buffer.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/rx_luz.p1: src/rx_luz.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/rx_luz.p1.d 
	@${RM} ${OBJECTDIR}/src/rx_luz.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/rx_luz.p1 src/rx_luz.c 
	@-${MV} ${OBJECTDIR}/src/rx_luz.d ${OBJECTDIR}/src/rx_luz.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/rx_luz.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/my_assert.p1: src/my_assert.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/my_assert.p1.d 
	@${RM} ${OBJECTDIR}/src/my_assert.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/my_assert.p1 src/my_assert.c 
	@-${MV} ${OBJECTDIR}/src/my_assert.d ${OBJECTDIR}/src/my_assert.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/my_assert.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/device_driver_config.p1: src/device_driver_config.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/device_driver_config.p1.d 
	@${RM} ${OBJECTDIR}/src/device_driver_config.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/device_driver_config.p1 src/device_driver_config.c 
	@-${MV} ${OBJECTDIR}/src/device_driver_config.d ${OBJECTDIR}/src/device_driver_config.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/device_driver_config.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/tilt_sensor.p1: src/tilt_sensor.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/tilt_sensor.p1.d 
	@${RM} ${OBJECTDIR}/src/tilt_sensor.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/tilt_sensor.p1 src/tilt_sensor.c 
	@-${MV} ${OBJECTDIR}/src/tilt_sensor.d ${OBJECTDIR}/src/tilt_sensor.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/tilt_sensor.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/timers.p1: src/timers.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/timers.p1.d 
	@${RM} ${OBJECTDIR}/src/timers.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/timers.p1 src/timers.c 
	@-${MV} ${OBJECTDIR}/src/timers.d ${OBJECTDIR}/src/timers.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/timers.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/gd_states.p1: src/gd_states.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/gd_states.p1.d 
	@${RM} ${OBJECTDIR}/src/gd_states.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/gd_states.p1 src/gd_states.c 
	@-${MV} ${OBJECTDIR}/src/gd_states.d ${OBJECTDIR}/src/gd_states.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/gd_states.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/gps.p1: src/gps.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/gps.p1.d 
	@${RM} ${OBJECTDIR}/src/gps.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/gps.p1 src/gps.c 
	@-${MV} ${OBJECTDIR}/src/gps.d ${OBJECTDIR}/src/gps.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/gps.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/extension_strings.p1: src/extension_strings.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/extension_strings.p1.d 
	@${RM} ${OBJECTDIR}/src/extension_strings.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/extension_strings.p1 src/extension_strings.c 
	@-${MV} ${OBJECTDIR}/src/extension_strings.d ${OBJECTDIR}/src/extension_strings.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/extension_strings.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/e_rtc.p1: src/e_rtc.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/e_rtc.p1.d 
	@${RM} ${OBJECTDIR}/src/e_rtc.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/e_rtc.p1 src/e_rtc.c 
	@-${MV} ${OBJECTDIR}/src/e_rtc.d ${OBJECTDIR}/src/e_rtc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/e_rtc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/eeprom.p1: src/eeprom.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/eeprom.p1.d 
	@${RM} ${OBJECTDIR}/src/eeprom.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/eeprom.p1 src/eeprom.c 
	@-${MV} ${OBJECTDIR}/src/eeprom.d ${OBJECTDIR}/src/eeprom.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/eeprom.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/DDS.p1: src/DDS.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/DDS.p1.d 
	@${RM} ${OBJECTDIR}/src/DDS.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/DDS.p1 src/DDS.c 
	@-${MV} ${OBJECTDIR}/src/DDS.d ${OBJECTDIR}/src/DDS.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/DDS.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/messages.p1: src/messages.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/messages.p1.d 
	@${RM} ${OBJECTDIR}/src/messages.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/messages.p1 src/messages.c 
	@-${MV} ${OBJECTDIR}/src/messages.d ${OBJECTDIR}/src/messages.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/messages.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/bit_banged_uart.p1: src/bit_banged_uart.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/bit_banged_uart.p1.d 
	@${RM} ${OBJECTDIR}/src/bit_banged_uart.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/bit_banged_uart.p1 src/bit_banged_uart.c 
	@-${MV} ${OBJECTDIR}/src/bit_banged_uart.d ${OBJECTDIR}/src/bit_banged_uart.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/bit_banged_uart.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/detector.p1: src/detector.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/detector.p1.d 
	@${RM} ${OBJECTDIR}/src/detector.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1  --debugger=none    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/detector.p1 src/detector.c 
	@-${MV} ${OBJECTDIR}/src/detector.d ${OBJECTDIR}/src/detector.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/detector.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/src/main.p1: src/main.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/main.p1.d 
	@${RM} ${OBJECTDIR}/src/main.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/main.p1 src/main.c 
	@-${MV} ${OBJECTDIR}/src/main.d ${OBJECTDIR}/src/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/Init_all.p1: src/Init_all.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Init_all.p1.d 
	@${RM} ${OBJECTDIR}/src/Init_all.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/Init_all.p1 src/Init_all.c 
	@-${MV} ${OBJECTDIR}/src/Init_all.d ${OBJECTDIR}/src/Init_all.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/Init_all.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/UART.p1: src/UART.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/UART.p1.d 
	@${RM} ${OBJECTDIR}/src/UART.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/UART.p1 src/UART.c 
	@-${MV} ${OBJECTDIR}/src/UART.d ${OBJECTDIR}/src/UART.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/UART.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/Clock.p1: src/Clock.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Clock.p1.d 
	@${RM} ${OBJECTDIR}/src/Clock.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/Clock.p1 src/Clock.c 
	@-${MV} ${OBJECTDIR}/src/Clock.d ${OBJECTDIR}/src/Clock.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/Clock.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/configuration_bits.p1: src/configuration_bits.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/configuration_bits.p1.d 
	@${RM} ${OBJECTDIR}/src/configuration_bits.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/configuration_bits.p1 src/configuration_bits.c 
	@-${MV} ${OBJECTDIR}/src/configuration_bits.d ${OBJECTDIR}/src/configuration_bits.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/configuration_bits.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/interrupt_isr_file.p1: src/interrupt_isr_file.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/interrupt_isr_file.p1.d 
	@${RM} ${OBJECTDIR}/src/interrupt_isr_file.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/interrupt_isr_file.p1 src/interrupt_isr_file.c 
	@-${MV} ${OBJECTDIR}/src/interrupt_isr_file.d ${OBJECTDIR}/src/interrupt_isr_file.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/interrupt_isr_file.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/ADC.p1: src/ADC.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/ADC.p1.d 
	@${RM} ${OBJECTDIR}/src/ADC.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/ADC.p1 src/ADC.c 
	@-${MV} ${OBJECTDIR}/src/ADC.d ${OBJECTDIR}/src/ADC.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/ADC.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/handlers.p1: src/handlers.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/handlers.p1.d 
	@${RM} ${OBJECTDIR}/src/handlers.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/handlers.p1 src/handlers.c 
	@-${MV} ${OBJECTDIR}/src/handlers.d ${OBJECTDIR}/src/handlers.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/handlers.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/ring_buffer.p1: src/ring_buffer.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/ring_buffer.p1.d 
	@${RM} ${OBJECTDIR}/src/ring_buffer.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/ring_buffer.p1 src/ring_buffer.c 
	@-${MV} ${OBJECTDIR}/src/ring_buffer.d ${OBJECTDIR}/src/ring_buffer.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/ring_buffer.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/rx_luz.p1: src/rx_luz.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/rx_luz.p1.d 
	@${RM} ${OBJECTDIR}/src/rx_luz.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/rx_luz.p1 src/rx_luz.c 
	@-${MV} ${OBJECTDIR}/src/rx_luz.d ${OBJECTDIR}/src/rx_luz.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/rx_luz.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/my_assert.p1: src/my_assert.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/my_assert.p1.d 
	@${RM} ${OBJECTDIR}/src/my_assert.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/my_assert.p1 src/my_assert.c 
	@-${MV} ${OBJECTDIR}/src/my_assert.d ${OBJECTDIR}/src/my_assert.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/my_assert.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/device_driver_config.p1: src/device_driver_config.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/device_driver_config.p1.d 
	@${RM} ${OBJECTDIR}/src/device_driver_config.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/device_driver_config.p1 src/device_driver_config.c 
	@-${MV} ${OBJECTDIR}/src/device_driver_config.d ${OBJECTDIR}/src/device_driver_config.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/device_driver_config.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/tilt_sensor.p1: src/tilt_sensor.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/tilt_sensor.p1.d 
	@${RM} ${OBJECTDIR}/src/tilt_sensor.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/tilt_sensor.p1 src/tilt_sensor.c 
	@-${MV} ${OBJECTDIR}/src/tilt_sensor.d ${OBJECTDIR}/src/tilt_sensor.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/tilt_sensor.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/timers.p1: src/timers.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/timers.p1.d 
	@${RM} ${OBJECTDIR}/src/timers.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/timers.p1 src/timers.c 
	@-${MV} ${OBJECTDIR}/src/timers.d ${OBJECTDIR}/src/timers.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/timers.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/gd_states.p1: src/gd_states.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/gd_states.p1.d 
	@${RM} ${OBJECTDIR}/src/gd_states.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/gd_states.p1 src/gd_states.c 
	@-${MV} ${OBJECTDIR}/src/gd_states.d ${OBJECTDIR}/src/gd_states.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/gd_states.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/gps.p1: src/gps.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/gps.p1.d 
	@${RM} ${OBJECTDIR}/src/gps.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/gps.p1 src/gps.c 
	@-${MV} ${OBJECTDIR}/src/gps.d ${OBJECTDIR}/src/gps.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/gps.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/extension_strings.p1: src/extension_strings.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/extension_strings.p1.d 
	@${RM} ${OBJECTDIR}/src/extension_strings.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/extension_strings.p1 src/extension_strings.c 
	@-${MV} ${OBJECTDIR}/src/extension_strings.d ${OBJECTDIR}/src/extension_strings.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/extension_strings.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/e_rtc.p1: src/e_rtc.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/e_rtc.p1.d 
	@${RM} ${OBJECTDIR}/src/e_rtc.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/e_rtc.p1 src/e_rtc.c 
	@-${MV} ${OBJECTDIR}/src/e_rtc.d ${OBJECTDIR}/src/e_rtc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/e_rtc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/eeprom.p1: src/eeprom.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/eeprom.p1.d 
	@${RM} ${OBJECTDIR}/src/eeprom.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/eeprom.p1 src/eeprom.c 
	@-${MV} ${OBJECTDIR}/src/eeprom.d ${OBJECTDIR}/src/eeprom.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/eeprom.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/DDS.p1: src/DDS.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/DDS.p1.d 
	@${RM} ${OBJECTDIR}/src/DDS.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/DDS.p1 src/DDS.c 
	@-${MV} ${OBJECTDIR}/src/DDS.d ${OBJECTDIR}/src/DDS.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/DDS.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/messages.p1: src/messages.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/messages.p1.d 
	@${RM} ${OBJECTDIR}/src/messages.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/messages.p1 src/messages.c 
	@-${MV} ${OBJECTDIR}/src/messages.d ${OBJECTDIR}/src/messages.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/messages.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/bit_banged_uart.p1: src/bit_banged_uart.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/bit_banged_uart.p1.d 
	@${RM} ${OBJECTDIR}/src/bit_banged_uart.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/bit_banged_uart.p1 src/bit_banged_uart.c 
	@-${MV} ${OBJECTDIR}/src/bit_banged_uart.d ${OBJECTDIR}/src/bit_banged_uart.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/bit_banged_uart.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/detector.p1: src/detector.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/detector.p1.d 
	@${RM} ${OBJECTDIR}/src/detector.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib $(COMPARISON_BUILD)  --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -o${OBJECTDIR}/src/detector.p1 src/detector.c 
	@-${MV} ${OBJECTDIR}/src/detector.d ${OBJECTDIR}/src/detector.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/detector.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/GPSDetector.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) --chip=$(MP_PROCESSOR_OPTION) -G -m${DISTDIR}/GPSDetector.${IMAGE_TYPE}.map  -D__DEBUG=1  --debugger=none  -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"        $(COMPARISON_BUILD) --memorysummary ${DISTDIR}/memoryfile.xml -o${DISTDIR}/GPSDetector.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	@${RM} ${DISTDIR}/GPSDetector.${IMAGE_TYPE}.hex 
	
	
else
${DISTDIR}/GPSDetector.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) --chip=$(MP_PROCESSOR_OPTION) -G -m${DISTDIR}/GPSDetector.${IMAGE_TYPE}.map  -DXPRJ_PIC16F1936_xc8_1_38_no_BL=$(CND_CONF)    --double=32 --float=24 --opt=+asm,-asmfile,-speed,+space,+debug --addrqual=require --mode=pro -DPIC_16F1936 -DXC8_V_138 -N63 -I"./include" -I"./src" -I"./" --warn=9 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,+resetbits,-download,+stackcall,-config,+clib --output=+mcof,-elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     $(COMPARISON_BUILD) --memorysummary ${DISTDIR}/memoryfile.xml -o${DISTDIR}/GPSDetector.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(wildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
