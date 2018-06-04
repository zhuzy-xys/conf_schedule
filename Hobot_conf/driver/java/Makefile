#hconf java lib

#PARAM
CC      = g++
JNI_DIR = /usr/java/jdk1.7.0_55/include/
JNI_MD_DIR = /usr/java/jdk1.7.0_55/include/linux/
HCONF_HEAD_DIR = /usr/local/include/hconf
LIBOBJS = /usr/local/hconf/lib/libhconf.a

CFLAGS  = -g -c -Wall -D__STDC_FORMAT_MACROS  -I$(JNI_DIR) -I$(JNI_MD_DIR) -I$(HCONF_HEAD_DIR) -Iinclude -pthread
FLAGS   = -g -Wall -D__STDC_FORMAT_MACROS -I$(JNI_DIR) -I$(JNI_MD_DIR) -I $(HCONF_HEAD_DIR) -Iinclude -pthread
COBJS 	= lib/java_hconf.o
SO_OBJS = libhconf_java.so
SOFLAGS = -shared -fPIC 
C_SRC   = lib/java_hconf.cc

#.PHONY
.PHONY : $(AR_OBJS) $(SO_OBJS) clean

all : $(AR_OBJS) $(SO_OBJS) clean

$(SO_OBJS) : $(C_SRC)
	$(CC) $(SOFLAGS) $(FLAGS) -o $@  $^ $(LIBOBJS)  -lpthread 

clean : 
	rm -fr $(COBJS)
	mv $(SO_OBJS) $(AR_OBJS) hconf_jar
	ant -buildfile hconf_jar/build.xml

$(COBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
