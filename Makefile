# Makefile for C++ project using Conan and CMake

# Conan and CMake directories
CXXFLAGS = -std=c++17 -Wall
CMAKE_BUILD_DIR = $(CURDIR)/build

# Conan install command
CONAN_INSTALL_CMD =  cd $(CMAKE_BUILD_DIR) && conan install .. --build=missing -s compiler.libcxx=libstdc++11 

# CMake commands
CMAKE_CONFIGURE_CMD =  cd $(CMAKE_BUILD_DIR) && cmake ..
CMAKE_BUILD_CMD = cd $(CMAKE_BUILD_DIR) && cmake --build .

all: conan_install cmake_build

conan_install:
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
