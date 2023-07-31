# Makefile for C++ project using Conan and CMake

# Conan and CMake directories
CXXFLAGS = -std=c++17 -Wall
CMAKE_BUILD_DIR = $(CURDIR)/build

CONAN_REVISIONS_ENABLED := 1
DEFAULT_CONAN_REMOTE := default-conan-local
CONAN_REMOTE_URL := https://milvus01.jfrog.io/artifactory/api/conan/$(DEFAULT_CONAN_REMOTE)

CONAN_REMOTES := $(shell conan remote list)
ifeq (,$(findstring $(DEFAULT_CONAN_REMOTE),$(CONAN_REMOTES)))
    conan_remote_add_cmd := conan remote add $(DEFAULT_CONAN_REMOTE) $(CONAN_REMOTE_URL)
else
    # If default-conan-local remote is already added, do nothing
    conan_remote_add_cmd :=
endif

# Conan install command
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CONAN_INSTALL_CMD := cd $(CMAKE_BUILD_DIR) && conan install ..  --build=missing -s compiler=clang -s compiler.libcxx=libc++ -s compiler.cppstd=17 || { echo 'conan install failed'; exit 1; }
else ifeq ($(UNAME_S),Linux)
    $(info Running on $(OS_NAME))
    GCC_VERSION := $(shell gcc -dumpversion)
    ifeq ($(shell gcc -v 2>&1 | sed -n 's/.*\(--with-default-libstdcxx-abi\)=\(\w*\).*/\2/p'),gcc4)
        CONAN_INSTALL_CMD := cd $(CMAKE_BUILD_DIR) && conan install .. --build=missing -s compiler.version=$(GCC_VERSION) || { echo 'conan install failed'; exit 1; }
    else
        CONAN_INSTALL_CMD := cd $(CMAKE_BUILD_DIR) && conan install ..  --build=missing -s compiler.version=$(GCC_VERSION) -s compiler.libcxx=libstdc++11 || { echo 'conan install failed'; exit 1; }
    endif
else
    $(error Cannot build on Windows)
endif

#CONAN_INSTALL_CMD =  cd $(CMAKE_BUILD_DIR) && conan install .. --build=missing -s compiler.libcxx=libstdc++11 

# CMake commands
CMAKE_CONFIGURE_CMD =  cd $(CMAKE_BUILD_DIR) && cmake ..
CMAKE_BUILD_CMD = cd $(CMAKE_BUILD_DIR) && cmake --build .

all: conan_install cmake_build

conan_remote_add:
    $(conan_remote_add_cmd)

conan_install: conan_remote_add
	@echo "Installing dependencies with Conan..."
	@mkdir -p $(CMAKE_BUILD_DIR)
	$(CONAN_INSTALL_CMD)

cmake_build: conan_install
	@echo "Generating Makefile with CMake..."
	$(CMAKE_CONFIGURE_CMD)
	@echo "Building project with Make..."
	$(CMAKE_BUILD_CMD)

install: cmake_build
	@echo "Installing binary"
	@mkdir -p ${CURDIR}/bin
	@mkdir -p ${CURDIR}/lib
	-cp -r -P ${CMAKE_BUILD_DIR}/bin/* ${CURDIR}/bin/ 2>/dev/null
	-cp -r -P ${CMAKE_BUILD_DIR}/lib/* ${CURDIR}/lib/ 2>/dev/null

clean:
	@echo "Cleaning build files..."
	rm -rf $(CMAKE_BUILD_DIR) $(TARGET)
	rm -rf ${CURDIR}/bin
	rm -rf ${CURDIR}/lib
