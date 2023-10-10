from conans import ConanFile, CMake

class StoreTool(ConanFile):
    name = "StoreTool"  # Name of your project
    version = "1.0.0"   # Version of your project
    author = "zhanglu" # Your name or organization
    url = "" # URL to your project's repository
    description = "A third tool that connect and check milvus storage functions"

    # Specify the package settings and options
    settings = "os", "compiler", "build_type", "arch"
    default_options = {
        "arrow:parquet": True,
        "arrow:compute": True,
        "arrow:with_zstd": True,
        "arrow:shared": False,
        "arrow:with_thrift": True,
        "arrow:with_jemalloc": True,
        "aws-sdk-cpp:text-to-speech": False,
        "aws-sdk-cpp:transfer": False,
        "gtest:build_gmock": False,
        "glog:with_gflags": True,
        "glog:shared": True,
     }

    # Define the package requirements (dependencies)
    requires = ( "boost/1.79.0",
                 "zlib/1.2.13",
                 "gflags/2.2.2",
                 "glog/0.6.0",
                 "gtest/1.8.1",
                 "yaml-cpp/0.7.0",
                 "openssl/1.1.1q",
                 "libcurl/7.86.0",
		 "arrow/11.0.0",
                 "protobuf/3.21.4",
                 "re2/20230301",
                 "aws-c-common/0.8.2@milvus/dev",
                 "aws-c-compression/0.2.15@milvus/dev",
                 "aws-c-sdkutils/0.1.3@milvus/dev",
                 "aws-checksums/0.1.13@milvus/dev",
                 "aws-c-cal/0.5.20@milvus/dev",
                 "aws-c-io/0.10.20@milvus/dev",
                 "aws-c-http/0.6.13@milvus/dev",
                 "aws-c-auth/0.6.11@milvus/dev",
                 "aws-c-event-stream/0.2.7@milvus/dev",
                 "aws-c-s3/0.1.37@milvus/dev",
                 "aws-crt-cpp/0.17.23@milvus/dev",
                 "aws-sdk-cpp/1.9.234",
                 "googleapis/cci.20221108",
                 "google-cloud-cpp/2.5.0@milvus/dev",
         )

    # Set up the build system (CMake in this example)
    generators = ("cmake", "cmake_find_package")
    exports_sources = "src/*", "CMakeLists.txt"

    def configure(self):
        if self.settings.os == "Macos":
            # Macos M1 cannot use jemalloc
            if self.settings.arch not in ("x86_64", "x86"):
                del self.options["folly"].use_sse4_2

            self.options["arrow"].with_jemalloc = False
            self.options["boost"].without_fiber = True
            self.options["boost"].without_json = True
            self.options["boost"].without_wave = True
            self.options["boost"].without_math = True
            self.options["boost"].without_graph = True
            self.options["boost"].without_graph_parallel = True
            self.options["boost"].without_nowide = True

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        # Copy the built binaries and other necessary files to the package
        self.copy("*", dst="bin", src="bin")
        self.copy("*", dst="lib", src="lib")
        self.copy("*", dst="include", src="src/include")

