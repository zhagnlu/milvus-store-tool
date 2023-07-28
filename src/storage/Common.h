// Licensed to the LF AI & Data foundation under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <boost/filesystem.hpp>
#include <glog/logging.h>

namespace milvus::storage
{
    struct StorageConfig
    {
        std::string address = "localhost:9000";
        std::string bucket_name = "a-bucket";
        std::string access_key_id = "minioadmin";
        std::string access_key_value = "minioadmin";
        std::string remote_root_path = "files";
        std::string storage_type = "minio";
        std::string iam_endpoint = "";
        std::string log_level = "error";
        std::string region = "";
        bool useSSL = false;
        bool useIAM = false;
        bool useVirtualHost = false;

        bool
        operator==(const StorageConfig &other) const
        {
            return address.compare(other.address) == 0 && bucket_name.compare(other.bucket_name) == 0 &&
                   access_key_id.compare(other.access_key_id) == 0 &&
                   access_key_value.compare(other.access_key_value) == 0 &&
                   remote_root_path.compare(other.remote_root_path) == 0 && storage_type.compare(other.storage_type) == 0 &&
                   iam_endpoint.compare(other.iam_endpoint) == 0 && useSSL == other.useSSL && useIAM == other.useIAM &&
                   log_level.compare(other.log_level) && region.compare(other.region) == 0 &&
                   useVirtualHost == other.useVirtualHost;
        }
    };

    using namespace boost::filesystem;
    inline bool
    find_file(const path &dir, const std::string &file_name, path &path_found)
    {
        const recursive_directory_iterator end;
        boost::system::error_code err;
        auto iter = recursive_directory_iterator(dir, err);
        while (iter != end)
        {
            try
            {
                if ((*iter).path().filename() == file_name)
                {
                    path_found = (*iter).path();
                    return true;
                }
                iter++;
            }
            catch (filesystem_error &e)
            {
            }
            catch (std::exception &e)
            {
                // ignore error
            }
        }
        return false;
    }

    inline void
    InitLogSystem()
    {
        google::InitGoogleLogging("milvus");
        char testPath[100];
        auto pwd = std::string(getcwd(testPath, sizeof(testPath)));
        boost::filesystem::path filepath;
        auto currentPath = path(pwd);
        while (!find_file(currentPath, "glog.conf", filepath))
        {
            currentPath = currentPath.append("../");
        }
        auto configPath = filepath.string();
        gflags::SetCommandLineOption("flagfile", configPath.c_str());
    }

    inline void
    CloseLogSystem()
    {
        if (google::IsGoogleLoggingInitialized())
        {
            google::ShutdownGoogleLogging();
        }
    }

} // namespace milvus::storage
