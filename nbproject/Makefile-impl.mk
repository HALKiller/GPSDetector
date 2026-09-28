#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a pre- and a post- target defined where you can add customization code.
#
# This makefile implements macros and targets common to all configurations.
#
# NOCDDL


# Building and Cleaning subprojects are done by default, but can be controlled with the SUB
# macro. If SUB=no, subprojects will not be built or cleaned. The following macro
# statements set BUILD_SUB-CONF and CLEAN_SUB-CONF to .build-reqprojects-conf
# and .clean-reqprojects-conf unless SUB has the value 'no'
SUB_no=NO
SUBPROJECTS=${SUB_${SUB}}
BUILD_SUBPROJECTS_=.build-subprojects
BUILD_SUBPROJECTS_NO=
BUILD_SUBPROJECTS=${BUILD_SUBPROJECTS_${SUBPROJECTS}}
CLEAN_SUBPROJECTS_=.clean-subprojects
CLEAN_SUBPROJECTS_NO=
CLEAN_SUBPROJECTS=${CLEAN_SUBPROJECTS_${SUBPROJECTS}}


# Project Name
PROJECTNAME=GPSDetector

# Active Configuration
DEFAULTCONF=PIC16F1936_xc8_1_38_no_BL
CONF=${DEFAULTCONF}

# All Configurations
ALLCONFS=PIC16F1936_xc8_1_38 PIC16F1936_xc8_2_46 PIC16F1936_xc8_prepro xc8_2_46_optim PIC16F1936_xc8_1_38_no_BL PIC16F1936_xc8_1_38_BL 


# build
.build-impl: .build-pre
	${MAKE} -f nbproject/Makefile-${CONF}.mk SUBPROJECTS=${SUBPROJECTS} .build-conf


# clean
.clean-impl: .clean-pre
	${MAKE} -f nbproject/Makefile-${CONF}.mk SUBPROJECTS=${SUBPROJECTS} .clean-conf

# clobber
.clobber-impl: .clobber-pre .depcheck-impl
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=PIC16F1936_xc8_1_38 clean
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=PIC16F1936_xc8_2_46 clean
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=PIC16F1936_xc8_prepro clean
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=xc8_2_46_optim clean
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=PIC16F1936_xc8_1_38_no_BL clean
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=PIC16F1936_xc8_1_38_BL clean



# all
.all-impl: .all-pre .depcheck-impl
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=PIC16F1936_xc8_1_38 build
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=PIC16F1936_xc8_2_46 build
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=PIC16F1936_xc8_prepro build
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=xc8_2_46_optim build
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=PIC16F1936_xc8_1_38_no_BL build
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=PIC16F1936_xc8_1_38_BL build



# dependency checking support
.depcheck-impl:
#	@echo "# This code depends on make tool being used" >.dep.inc
#	@if [ -n "${MAKE_VERSION}" ]; then \
#	    echo "DEPFILES=\$$(wildcard \$$(addsuffix .d, \$${OBJECTFILES}))" >>.dep.inc; \
#	    echo "ifneq (\$${DEPFILES},)" >>.dep.inc; \
#	    echo "include \$${DEPFILES}" >>.dep.inc; \
#	    echo "endif" >>.dep.inc; \
#	else \
#	    echo ".KEEP_STATE:" >>.dep.inc; \
#	    echo ".KEEP_STATE_FILE:.make.state.\$${CONF}" >>.dep.inc; \
#	fi
