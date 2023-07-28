// Copyright (C) 2019-2020 Zilliz. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied. See the License for the specific language governing permissions and limitations under the License

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <glog/logging.h>

#include "storage/MinioChunkManager.h"

using namespace std;
using namespace milvus;
using namespace milvus::storage;
using namespace boost::filesystem;

class MinioChunkManagerTest : public testing::Test
{
public:
    MinioChunkManagerTest()
    {
        InitLogSystem();
    }
    ~MinioChunkManagerTest()
    {
        CloseLogSystem();
    }

    virtual void
    SetUp()
    {
        config_ = get_storage_config();
        chunk_manager_ =
            std::make_unique<MinioChunkManager>(config_);
    }

protected:
    StorageConfig config_;
    MinioChunkManagerPtr chunk_manager_;
};

TEST_F(MinioChunkManagerTest, WritePositive)
{
    string testBucketName = config_.bucket_name;
    chunk_manager_->SetBucketName(testBucketName);
    EXPECT_EQ(chunk_manager_->GetBucketName(), testBucketName);

    if (!chunk_manager_->BucketExists(testBucketName))
    {
        throw std::runtime_error("bucket not exists");
    }

    uint8_t data[5] = {0x17, 0x32, 0x45, 0x34, 0x23};
    string path = "1/3/5";
    chunk_manager_->Write(path, data, sizeof(data));

    bool exist = chunk_manager_->Exist(path);
    EXPECT_EQ(exist, true);

    auto size = chunk_manager_->Size(path);
    EXPECT_EQ(size, 5);

    int datasize = 10000;
    uint8_t *bigdata = new uint8_t[datasize];
    srand((unsigned)time(NULL));
    for (int i = 0; i < datasize; ++i)
    {
        bigdata[i] = rand() % 256;
    }
    chunk_manager_->Write(path, bigdata, datasize);
    size = chunk_manager_->Size(path);
    EXPECT_EQ(size, datasize);
    delete[] bigdata;

    chunk_manager_->Remove(path);
}

TEST_F(MinioChunkManagerTest, ReadPositive)
{
    string testBucketName = config_.bucket_name;
    chunk_manager_->SetBucketName(testBucketName);
    EXPECT_EQ(chunk_manager_->GetBucketName(), testBucketName);

    if (!chunk_manager_->BucketExists(testBucketName))
    {
        throw std::runtime_error("bucket not exists");
    }
    uint8_t data[5] = {0x17, 0x32, 0x45, 0x34, 0x23};
    string path = "1/4/6";
    chunk_manager_->Write(path, data, sizeof(data));
    bool exist = chunk_manager_->Exist(path);
    EXPECT_EQ(exist, true);
    auto size = chunk_manager_->Size(path);
    EXPECT_EQ(size, sizeof(data));

    uint8_t readdata[20] = {0};
    size = chunk_manager_->Read(path, readdata, sizeof(data));
    EXPECT_EQ(size, sizeof(data));
    EXPECT_EQ(readdata[0], 0x17);
    EXPECT_EQ(readdata[1], 0x32);
    EXPECT_EQ(readdata[2], 0x45);
    EXPECT_EQ(readdata[3], 0x34);
    EXPECT_EQ(readdata[4], 0x23);

    size = chunk_manager_->Read(path, readdata, 3);
    EXPECT_EQ(size, 3);
    EXPECT_EQ(readdata[0], 0x17);
    EXPECT_EQ(readdata[1], 0x32);
    EXPECT_EQ(readdata[2], 0x45);

    uint8_t dataWithNULL[] = {0x17, 0x32, 0x00, 0x34, 0x23};
    chunk_manager_->Write(path, dataWithNULL, sizeof(dataWithNULL));
    exist = chunk_manager_->Exist(path);
    EXPECT_EQ(exist, true);
    size = chunk_manager_->Size(path);
    EXPECT_EQ(size, sizeof(dataWithNULL));
    size = chunk_manager_->Read(path, readdata, sizeof(dataWithNULL));
    EXPECT_EQ(size, sizeof(dataWithNULL));
    EXPECT_EQ(readdata[0], 0x17);
    EXPECT_EQ(readdata[1], 0x32);
    EXPECT_EQ(readdata[2], 0x00);
    EXPECT_EQ(readdata[3], 0x34);
    EXPECT_EQ(readdata[4], 0x23);

    chunk_manager_->Remove(path);
}

TEST_F(MinioChunkManagerTest, RemovePositive)
{
    string testBucketName = config_.bucket_name;
    chunk_manager_->SetBucketName(testBucketName);
    EXPECT_EQ(chunk_manager_->GetBucketName(), testBucketName);

    if (!chunk_manager_->BucketExists(testBucketName))
    {
        throw std::runtime_error("bucket not exists");
    }
    uint8_t data[5] = {0x17, 0x32, 0x45, 0x34, 0x23};
    string path = "1/7/8";
    chunk_manager_->Write(path, data, sizeof(data));

    bool exist = chunk_manager_->Exist(path);
    EXPECT_EQ(exist, true);

    chunk_manager_->Remove(path);

    exist = chunk_manager_->Exist(path);
    EXPECT_EQ(exist, false);
}

TEST_F(MinioChunkManagerTest, ListWithPrefixPositive)
{
    string testBucketName = config_.bucket_name;
    chunk_manager_->SetBucketName(testBucketName);
    EXPECT_EQ(chunk_manager_->GetBucketName(), testBucketName);

    if (!chunk_manager_->BucketExists(testBucketName))
    {
        throw std::runtime_error("bucket not exists");
    }

    string path1 = "1/7/8";
    string path2 = "1/7/4";
    string path3 = "1/4/8";
    uint8_t data[5] = {0x17, 0x32, 0x45, 0x34, 0x23};
    chunk_manager_->Write(path1, data, sizeof(data));
    chunk_manager_->Write(path2, data, sizeof(data));
    chunk_manager_->Write(path3, data, sizeof(data));

    vector<string> objs = chunk_manager_->ListWithPrefix("1/7");
    EXPECT_EQ(objs.size(), 2);
    std::sort(objs.begin(), objs.end());
    EXPECT_EQ(objs[0], "1/7/4");
    EXPECT_EQ(objs[1], "1/7/8");

    objs = chunk_manager_->ListWithPrefix("//1/7");
    EXPECT_EQ(objs.size(), 2);

    objs = chunk_manager_->ListWithPrefix("1");
    EXPECT_EQ(objs.size(), 3);
    std::sort(objs.begin(), objs.end());
    EXPECT_EQ(objs[0], "1/4/8");
    EXPECT_EQ(objs[1], "1/7/4");

    chunk_manager_->Remove(path1);
    chunk_manager_->Remove(path2);
    chunk_manager_->Remove(path3);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();

    return result;
}