OBJS = \
	ProcesseurUnique.o \
	Labo2c.o

SHOWFLAGS = -c -Wall -MMD -Wpedantic

CXXFLAG = -std=c++14 $(SHOWFLAGS) $(USRINCFLAGS)
INCDIR = 
LDFLAGS = $(USRLDFLAGS)
SRCDIR = ./


USRINCFLAGS = $(INCDIR)
USRLDFLAGS  =
LIBS = 

EXECUTABLE = tabou

# Release build settings
CXX = g++
RELDIR = dist/release
RELEXE = $(RELDIR)/$(EXECUTABLE)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELFLAGS = -O3
RELLIBS = 
RELDEP = $(RELOBJS:%.o=%.d)

# Release build settings
CXX = g++
DBGDIR = dist/debug
DBGEXE = $(DBGDIR)/$(EXECUTABLE)
DBGOBJS = $(addprefix $(RELDIR)/, $(OBJS))
DBGFLAGS = -O1 -g3
DBGLIBS = 
DBGDEP = $(DBGOBJS:%.o=%.d)

# Default build
all: release

#
# Release rules
#
release: $(RELEXE)

$(RELEXE): $(RELOBJS)
	@echo $(CXX) $(RELFLAGS) $(LDFLAGS) $(LIBS) $^
	@$(CXX) $(RELFLAGS) $(LDFLAGS) -o $(RELEXE) $^ $(LIBS) $(RELLIBS)

$(RELDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	@echo $(CXX) $(RELFLAGS) -o $@ $<
	@$(CXX) $(CXXFLAG) $(RELFLAGS) -o $@ $<

#
# Debug rules
#
debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	@echo $(CXX) $(DBGFLAGS) $(LDFLAGS) $(LIBS) $^
	@$(CXX) $(DBGFLAGS) $(LDFLAGS) -o $(DBGEXE) $^ $(LIBS) $(DBGLIBS)

$(DBGDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	@echo $(CXX) $(DBGFLAGS) -o $@ $<
	@$(CXX) $(CXXFLAG) $(DBGFLAGS) -o $@ $<


#
# Clean release build, debug build, or both
#
clean:
	rm -f $(RELEXE) $(RELOBJS) $(RELDEP) $(DBGEXE) $(DBGOBJS) $(DBGDEP) $(NVEXE)
	@echo Clean done

relclean:
	rm -f $(RELEXE) $(RELOBJS) $(RELDEP)
	@echo Clean done
	
dbgclean:
	rm -f $(DBGEXE) $(DBGOBJS) $(DBGDEP)
	@echo Clean done

	

.PHONY: all release debug clean relclean dbgclean
