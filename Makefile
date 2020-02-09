SRCDIR = src
INCDIR = inc dtl
BINDIR = bin
DEBUGDIR = $(BINDIR)/debug
RELEASEDIR = $(BINDIR)/release
OBJDIR = obj
EXEC = sgit
PACKAGE_NAME = sgit_1.0-1

BOOST_LIB_DIR = /usr/lib
BOOST_LIB = boost_filesystem boost_system boost_program_options
LIB = $(BOOST_LIB) dl z

SOURCES = blob commands diff helper object SimpleGit branch commit filesystem \
	index ref tag checkout config globals merge sha1 tree linux
OBJS = $(SOURCES:%=$(OBJDIR)/%.o)
DEPS = $(SOURCES:%=$(OBJDIR)/%.d)
CPPS = $(SOURCES:%=$(SRCDIR)/%.cpp)

CXX = g++
RM = rm -f

DISABLE_WARNINGS = -Wno-format-security -Wno-format-contains-nul

CXXFLAGS = $(INCDIR:%=-I%) $(DISABLE_WARNINGS)
CPPFLAGS = -L$(BOOST_LIB_DIR) $(LIB:%=-l%)

DEBUG ?= 1
ifeq ($(DEBUG),1)
	CXXFLAGS += -g -O0
	CPPFLAGS += -g -O0
	TARGET = $(DEBUGDIR)/$(EXEC)
else
	TARGET = $(RELEASEDIR)/$(EXEC)
	CXXFLAGS += -O3
	CPPFLAGS += -O3
endif

CXXFLAGS += -MMD 
test:
	echo $(CXXFLAGS)

.PHONY: build rebuild clean install create_directories

clean:
	find $(OBJDIR) $(DEBUGDIR) $(RELEASEDIR)  -type f -delete
	#@$(RM) $(OBJDIR)/* $(DEBUGDIR)/* $(RELEASEDIR)/*
dummy:
	@echo expected argument

build:		$(TARGET)
rebuild:	clean build

install: build
	@cp $(TARGET) $(BINDIR)/$(PACKAGE_NAME)/usr/local/bin
	@dpkg-deb --build $(BINDIR)/$(PACKAGE_NAME)
	@dpkg -i $(BINDIR)/$(PACKAGE_NAME).deb

reinstall: rebuild
	@cp $(TARGET) $(BINDIR)/$(PACKAGE_NAME)/usr/local/bin
	@dpkg-deb --build $(BINDIR)/$(PACKAGE_NAME)
	@dpkg -i $(BINDIR)/$(PACKAGE_NAME).deb

$(TARGET): $(OBJS)
	@echo linking...
	@$(CXX) -o $@ $^ $(CPPFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo compiling $<...
	@$(CXX) -c -o $@ $< $(CXXFLAGS)

linux.h:
windows.h:

-include $(DEPS)
