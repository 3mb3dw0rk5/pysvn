#
#	pysvn_common.mak
#
#	include this mak file after defining the variables it needs
#

CXX_OBJECTS=cxxsupport.o cxx_extensions.o cxxextensions.o IndirectPythonInterface.o
PYSVN_OBJECTS=pysvn.o pysvn_callbacks.o pysvn_client.o pysvn_entry.o pysvn_enum_string.o \
	pysvn_revision.o pysvn_status.o pysvn_docs.o pysvn_path.o

all: pysvn/_pysvn.so

pysvn/_pysvn.so: $(PYSVN_OBJECTS) $(CXX_OBJECTS)
	$(LDSHARED) -o $@ $(PYSVN_OBJECTS) $(CXX_OBJECTS) $(LDLIBS)

pysvn.o: pysvn.cpp
	$(CCC) $(CCCFLAGS) -o $@ $<

pysvn_callbacks.o: pysvn_callbacks.cpp
	$(CCC) $(CCCFLAGS) -o $@ $<

pysvn_client.o: pysvn_client.cpp
	$(CCC) $(CCCFLAGS) -o $@ $<

pysvn_entry.o: pysvn_entry.cpp
	$(CCC) $(CCCFLAGS) -o $@ $<

pysvn_enum_string.o: pysvn_enum_string.cpp
	$(CCC) $(CCCFLAGS) -o $@ $<

pysvn_docs.o: pysvn_docs.cpp
	$(CCC) $(CCCFLAGS) -o $@ $<

pysvn_path.o: pysvn_path.cpp
	$(CCC) $(CCCFLAGS) -o $@ $<

pysvn_revision.o: pysvn_revision.cpp
	$(CCC) $(CCCFLAGS) -o $@ $<

pysvn_status.o: pysvn_status.cpp
	$(CCC) $(CCCFLAGS) -o $@ $<

cxxsupport.o: $(PYCXX)/Src/cxxsupport.cxx
	$(CCC) $(CCCFLAGS) -o $@ $<

cxx_extensions.o: $(PYCXX)/Src/cxx_extensions.cxx
	$(CCC) $(CCCFLAGS) -o $@ $<

cxxextensions.o: $(PYCXX)/Src/cxxextensions.c
	$(CC) -c $(CCCFLAGS) -o $@ $<

IndirectPythonInterface.o: $(PYCXX)/Src/IndirectPythonInterface.cxx
	$(CCC) $(CCCFLAGS) -o $@ $< 

clean:
	rm -f *.o
	rm -f pysvn/_pysvn.so

test: pysvn/_pysvn.so
	LD_LIBRARY_PATH=$(SVNCPP_LIB) $(PYTHON) -c "import pysvn;print pysvn.Client()"
