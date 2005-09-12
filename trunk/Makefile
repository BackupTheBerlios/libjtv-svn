PROJECT_TEST=test-libjtv
PROJECT_LIB=libjtv.a

all: $(PROJECT_TEST) $(PROJECT_LIB)

#test-libjtv: libjtv.a test-libjtv.o

%.o: %.c
	g++ -g -c -o $@ $< -I . -DOS_LINUX

%.o: %.cpp
	g++ -g -c -o $@ $< -I . -DOS_LINUX

$(PROJECT_LIB): archive.o strnew.o libjtv.o csstrvec.o csvector.o cbase.o
	ar rsf $@ $^

$(PROJECT_TEST): $(PROJECT_TEST).o $(PROJECT_LIB)
	g++ -g -o $@ $< $(PROJECT_LIB) -lstdc++ -lz

clean:
	rm -f *.o *.a $(PROJECT_TEST)
