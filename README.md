# Milvus-store-tool
Debug tool for Milvus project. This tool connects to object storage of milvus and check whether read and write it as expected.

# Get Startted

**Prerequisite**

```
gcc >= 7.5
cmake >= 3.12
conan == 1.58.0
```

**Install**

Clone repo and build

```
git clone https://github.com/zhagnlu/milvus-store-tool.git

make install
```

# How to use

1. rewrite src/configs/milvus.yaml according to storage configure.
2. test

   minio:

   ```
   ./bin/MinioTool
   ```

   AliyunOss:
    ```
   ./bin/AliOssTool
   ```

   GoogleCloud:

   ```
   ./bin/GoogleCloudTool
   ```

3. if test failed, improve log level to get more details. update src/configs/milvus.yaml Minio.logLevel to debug or trace.
