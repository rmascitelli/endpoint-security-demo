GCC = gcc
GPP = g++
EPS_EXE = eps-example-test
PY_BUILD_DIR = dist
OBJECT_DIR = objects
LIBS_DIR = libs

LD_SHARED_LIBS = -lEndpointSecurity -lbsm
CHECK_EPS_ENTITLE = codesign --entitlements - -d ${PY_BUILD_DIR}/${EPS_EXE} | grep com.apple.developer.endpoint-security.client

all: clean build_dirs create_eps_so create_py_exe

run: clean build_dirs create_eps_so create_py_exe run

build_dirs:
	mkdir ${LIBS_DIR} ${OBJECT_DIR} ${PY_BUILD_DIR}

create_eps_so:
	${GPP} -fPIC -shared ${LD_SHARED_LIBS} eps_lib.cpp -o ${OBJECT_DIR}/eps_lib.so

create_py_exe:
	pyinstaller --onefile ${EPS_EXE}.py
	codesign -f --entitlement sentries.plist -s - ${PY_BUILD_DIR}/${EPS_EXE}
	${CHECK_EPS_ENTITLE}
	rm ${EPS_EXE}.spec

cpp_scratch:
	${GPP} -fPIC -shared ${LIBS_DIR}/custom_struct.c -o ${OBJECT_DIR}/custom_struct.so
	${GPP} -fPIC -shared -I${LIBS_DIR} ${OBJECT_DIR}/custom_struct.so deprecated/cpp_scratch_pad.cpp -o ${OBJECT_DIR}/cpp_scratch.so

clean:
	rm -rf ${LIBS_DIR} ${OBJECT_DIR} ${PY_BUILD_DIR}
	#rm -f ${OBJECT_DIR}/*.*o

run:
	${PY_BUILD_DIR}/${EPS_EXE}
	
