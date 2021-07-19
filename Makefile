SRCDIR = src
INCDIR = inc dtl
BINDIR = bin
DEBUGDIR = $(BINDIR)/debug
RELEASEDIR = $(BINDIR)/release
OBJDIR = obj
EXEC = sgit
PACKAGE_NAME = sgit_1.0-1
CPP_VERSION = c++17

BOOST_LIB_DIR = /usr/lib
BOOST_LIB = boost_filesystem boost_system boost_program_options
LIB = $(BOOST_LIB) dl z pthread 

SOURCES = blob commands diff helper object SimpleGit branch commit filesystem \
	  index ref tag checkout config globals merge sha1 tree linux repository
OBJS = $(SOURCES:%=$(OBJDIR)/%.o)
DEPS = $(SOURCES:%=$(OBJDIR)/%.d)
CPPS = $(SOURCES:%=$(SRCDIR)/%.cpp)

CXX = g++
RM = rm -rf

DISABLE_WARNINGS = -Wno-format-security -Wno-format-contains-nul

CXXFLAGS = $(INCDIR:%=-I%) $(DISABLE_WARNINGS) -std=$(CPP_VERSION)
CPPFLAGS = -L$(BOOST_LIB_DIR) $(LIB:%=-l%) -std=$(CPP_VERSION)

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

.PHONY: build rebuild clean install create_directories

clean:
	@$(RM) $(OBJDIR) $(DEBUGDIR) $(RELEASEDIR)
dummy:
	@echo expected argument

build:		$(TARGET)
rebuild:	clean build

install: build
	@rm -rf install/$(PACKAGE_NAME)
	@cp -r install/template/ install/$(PACKAGE_NAME)/
	@mkdir -p install/$(PACKAGE_NAME)/usr/local/bin/
	@cp $(TARGET) install/$(PACKAGE_NAME)/usr/local/bin/
	@dpkg-deb --build install/$(PACKAGE_NAME)
	@dpkg -i install/$(PACKAGE_NAME).deb

reinstall: rebuild install

$(TARGET): $(OBJS)
	@mkdir -p `dirname $@`
	@echo linking ...
	@$(CXX) -o $@ $^ $(CPPFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p obj
	@echo compiling $< ...
	@$(CXX) -c -o $@ $< $(CXXFLAGS)

linux.h:
windows.h:

-include $(DEPS)
