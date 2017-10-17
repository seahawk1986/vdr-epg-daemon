BASELIBS = -lrt -lz -larchive -ldl -lcrypto -luuid
BASELIBS += $(shell mysql_config --libs_r) $(shell python-config --libs) $(shell pkg-config --cflags --libs jansson)

HLIB     = -L../lib -lhorchi
TOOLSLIB = -L ../tools
DLIBS    = $(HLIB) $(BASELIBS) -lcurl $(shell xml2-config --libs) $(shell xslt-config --libs) -lexslt
HTTPLIBS = $(HLIB) -lmicrohttpd $(BASELIBS) -lcurl $(shell xml2-config --libs) $(shell xslt-config --libs) -lexslt -ljpeg $(shell imlib2-config --libs)
CFLAGS   += $(shell mysql_config --include)

all:
	$(MAKE) -C ../ 
	g++ -g tvdbapi_test.cpp ../levenshtein.o  -o test $(HTTPLIBS) $(TOOLSLIB) $(DLIBS) $(mysql_config --libs_r) -lcrypto -larchive -lxslt -luuid
