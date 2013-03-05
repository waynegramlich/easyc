# Copyright (c) 2004-2010 by Wayne C. Gramlich.
# All rights reserved.

#ROOT := ..
ROOT := .

include $(ROOT)/Makefile_common.mk
include $(ROOT)/Makefile_private.mk

OGLS_DIRECTORY := ../ogls
OGLS_BUILD := $(OGLS_DIRECTORY)/OGLS_Build
OGLS_BUILD_GENERATE := $(OGLS_DIRECTORY)/OGLS_Build_Generate
EZCC_STABLE := releases/latest/ezcc

EZCC := ezccx
EZCC_BASES :=			\
	EZCC			\
	Compiler		\
	Declaration		\
	Easy_C			\
	Expression		\
	Parse			\
	Statement		\
	Token			\
	Unix
EZCC_EZCS := ${EZCC_BASES:%=%.ezc}
EZCC_EZHS := ${EZCC_BASES:%=%.ezh}
EZCC_EZGS := ${EZCC_BASES:%=%.ezg}
EZCC_OS := ${EZCC_BASES:%=%.o}
EZCC_CS := ${EZCC_BASES:%=%.c}
EZCC_HS := ${EZCC_BASES:%=%.h}

EZCC_EZ := ezcc.ez
EZCC_EZ_EZCS := ${EZCC_BASES:%=%.ez.ezc}
EZCC_EZ_EZHS := ${EZCC_BASES:%=%.ez.ezh}
EZCC_EZ_EZGS := ${EZCC_BASES:%=%.ez.ezg}
EZCC_EZ_OS := ${EZCC_BASES:%=%.ez.o}
EZCC_EZ_CS := ${EZCC_BASES:%=%.ez.c}
EZCC_EZ_HS := ${EZCC_BASES:%=%.ez.h}

EZCIDE := EZCIDE

LIB_BASES :=		\
    Easy_C		\
    Unix		\
    Unix_Termios
LIB_CS := ${LIB_BASES:%=%.c}
LIB_EZCS := ${LIB_BASES:%=%.ezc}
LIB_EZHS := ${LIB_BASES:%=%.ezh}
LIB_EZGS := ${LIB_BASES:%=%.ezg}
LIB_HS := ${LIB_BASES:%=%.h}
LIB_OS := ${LIB_BASES:%=%.o}
LIB_CLEANS :=		\
    ${LIB_CS}		\
    ${LIB_EZHS}		\
    ${LIB_EZGS}		\
    ${LIB_HS}		\
    ${LIB_OS}

LIB_C_BASES :=		\
    Easy_C		\
    Unix
LIB_C_CS := ${LIB_C_BASES:%=%_C.c}
LIB_C_HS := ${LIB_C_BASES:%=%_C.h}
LIB_C_OS := ${LIB_C_BASES:%=%_C.o}
LIB_C_CLEANS := ${LIB_C_OS}

#AVR_BASES :=		\
#    AVR			\
#    ATmega168		\
#    ATmega324p		\
#    Blink168		\
#    PWM168
#AVR_EZCS := ${AVR_BASES:%=%.ezc}
#AVR_EZHS := ${AVR_BASES:%=%.ezh}
#AVR_EZGS := ${AVR_BASES:%=%.ezg}
#AVR_HS := ${AVR_BASES:%=%.h}
#AVR_OS := ${AVR_BASES:%=%.o}

UNIX_TEST := Unix_Test
UNIX_TEST_BASES :=	\
    Unix_Test		\
    Unix		\
    Easy_C
UNIX_TEST_CS := ${UNIX_TEST_BASES:%=%.c}
UNIX_TEST_EZCS := ${UNIX_TEST_BASES:%=%.ezc}
UNIX_TEST_EZGS := ${UNIX_TEST_BASES:%=%.ezg}
UNIX_TEST_EZHS := ${UNIX_TEST_BASES:%=%.ezh}
UNIX_TEST_HS := ${UNIX_TEST_BASES:%=%.h}
UNIX_TEST_OS := ${UNIX_TEST_BASES:%=%.o}
UNIX_TEST_CLEANS :=		\
    ${UNIX_TEST_CS}		\
    ${UNIX_TEST_HS}		\
    ${UNIX_TEST_EZHS}		\
    ${UNIX_TEST_EZGS}		\
    ${UNIX_TEST_OS}		\
    $(UNIX_TEST)

# Everything needed for download:
ALL_EZCS :=			\
    ${EZCC_EZCS}
#    ${AVR_EZCS}
ALL_CS :=			\
    bootstrap.c			\
    ${EZCC_CS}			\
    Easy_C_C.c			\
    Unix_C.c
#    AVR_C.c
ALL_HS :=			\
    ${EZCC_HS}			\
    Easy_C_C.h
#    AVR_C.h
ALL_SOURCES :=			\
    ${ALL_EZCS}			\
    ${ALL_CS}			\
    ${ALL_HS}
ALL_EXECUTABLES :=		\
    EZCIDE			\
    ezcc
#    ezcc.exe
ALL_BOOT_STRAPS :=		\
    bootstrap.sh		\
    bootstrap.bat
ALL_HTMLS :=			\
    index.html			\
    downloads.html		\
    tutorial.html		\
    reference.html
ALL_FILES :=			\
    README.txt			\
    ${ALL_SOURCES}		\
    ${ALL_HTMLS}		\
    ${ALL_BOOT_STRAPS}		\
    ${ALL_EXECUTABLES}


EZCC_CS := ${EZCC_BASES:%=%.c}
EZCC_HS := ${EZCC_BASES:%=%.h}


# Release candidate stuff:
RCD := releases/candidate
RCM := $(RCD)/Makefile

PROGRAMS :=	\
    $(EZCC)	\
    $(EZCC_EZ)	\
    $(EZCIDE)

TAR_FILES :=	\
    ezcc.ez	\
    EZCIDE	\
    Easy_C.ezc	\
    Easy_C_C.ezc \
    Easy_C_C.c	\
    Easy_C_C.h	\
    update.sh

BOOTSTRAP_FILES := \
    Compiler.ez.c \
    Compiler.ez.h \
    Declaration.ez.c \
    Declaration.ez.h \
    Easy_C.ez.c \
    Easy_C.ez.h \
    Expression.ez.c \
    Expression.ez.h \
    EZCC.ez.c \
    EZCC.ez.h \
    Parse.ez.c \
    Parse.ez.h \
    Statement.ez.c \
    Statement.ez.h \
    Token.ez.c \
    Token.ez.h \
    Unix.ez.c \
    Unix.ez.h \
    ezcc.ez.c \
    Easy_C.h \
    Unix.h

.PHONY: all clean

all:  ${PROGRAMS}

%.ezc: %.x.h
	$(EZCC_EZ) -X ${<:%.x.h=%}

$(EZCC): ${EZCC_EZCS} ${LIB_C_HS} ${LIB_C_CS}
	$(EZCC_STABLE) -o $(EZCC) EZCC

$(EZCC_EZ): ${EZCC}
	./$(EZCC) -S .ez -o $(EZCC_EZ) EZCC

EZC_IDE_GUI.ezc: EZC_IDE_GUI.xml
	$(OGLS_BUILD_GENERATE) EZC_IDE_GUI

EZCIDE: EZCIDE.ezc EZC_IDE_GUI.ezc ../ogls/OGLS.ezc
	$(EZCC_EZ) -I $(OGLS_DIRECTORY) EZCIDE

$(UNIX_TEST): ${UNIX_TEST_EZCS} ${EXTRACTS_EZCS}
	$(EZCC_EZ) Unix_Test

bootstrap.tar: ${BOOTSTRAP_FILES}
	rm -f $@
	tar cvf $@ ${BOOTSTRAP_FILES}

easyc.tar: ${TAR_FILES}
	rm -f $@
	tar cvf - ${TAR_FILES} | gzip -9 > easy_c.tar

clean:
	rm -f easyc.tar.gz
	rm -f ${EZCC_EZGS} ${EZCC_EZHS} ${EZCC_CS} ${EZCC_HS} ${EZCC_OS}
	rm -f ${EZCC_EZ_EZGS} ${EZCC_EZ_EZHS} ${EZCC_EZ_CS} ${EZCC_EZ_HS}
	rm -f ${EZCC_EZ_OS}
	rm -f $(EZCC) $(EZCC_EZ) EZCC
	rm -f ${UNIX_TEST_CLEANS}
	rm -f ${LIB_CLEANS}
	rm -f ${LIB_C_CLEANS}
	rm -f Easy_C_C.o
	rm -f Test.ezg Test.ezh Test.h Test.o Test
	$(CLEAN)

candidate:
	rm -rf $(RCD)
	mkdir $(RCD)
	cp ${EZCC_EZCS} $(RCD)
	#cp ${AVR_EZCS} $(RCD)
	cp Math.ezc ${RCD}
	cp Unix_Termios.ezc $(RCD)
	cp Unix_Termios_C.h $(RCD)
	cp Unix_Termios_C.c $(RCD)
	cp Easy_C_C.c $(RCD)
	cp Easy_C_C.h $(RCD)
	cp Unix_C.c $(RCD)
	cp Unix_C.h $(RCD)
	#cp AVR_C.c $(RCD)
	#cp AVR_C.h $(RCD)
	cp README.txt $(RCD)
	cp ${ALL_HTMLS} $(RCD)
	echo "all:" >> $(RCM)
	echo "	tar cf /tmp/candidate.tar ."  >> $(RCM)
	echo "	rm -rf boot1 boot2 boot3" >> $(RCM)
	echo "	mkdir boot1 boot2 boot3" >> $(RCM)
	echo "	cd boot1 ; tar xf /tmp/candidate.tar" >> $(RCM)
	echo "	cd boot2 ; tar xf /tmp/candidate.tar" >> $(RCM)
	echo "	cd boot3 ; tar xf /tmp/candidate.tar" >> $(RCM)
	echo "	cd boot1 ; ../../../ezccx  -o ezcc EZCC" >> $(RCM)
	echo "	cd boot2 ; ../boot1/ezcc -o ezcc EZCC" >> $(RCM)
	echo "	cd boot3 ; ../boot2/ezcc -o ezcc EZCC" >> $(RCM)
	for i in ${EZCC_BASES} ; do \
	  echo "	cmp boot2/$$i.c boot3/$$i.c" >> $(RCM) ; \
	  echo "	cmp boot2/$$i.h boot3/$$i.h" >> $(RCM) ; \
	  done
	echo "	boot3/ezcc -o ezcc EZCC" >> $(RCM)
	#echo "	ezcc -o ezcc.exe -C win-gcc EZCC" >> $(RCM)
	echo "	cp /tmp/EZCC.c bootstrap.c" >> $(RCM)
	echo "	echo gcc -o ezcc -g ${ALL_CS} > bootstrap.sh" >> $(RCM)
	echo "	echo gcc -o ezcc -g ${ALL_CS} > bootstrap.bat" >> $(RCM)
	echo "	zip easy_c.zip ${ALL_FILES}" >> $(RCM)
	echo "	rm -rf boot1 boot2 boot3" >> $(RCM)
	echo "	echo Bootstrap Succeeded" >> $(RCM)


