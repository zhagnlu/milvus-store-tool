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
#include <stdexcept>
#include <exception>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <limits>

/* Paste this on the file if you want to debug. */
namespace milvus
{
    enum ErrorCode : int
    {
        Success = 0,
        UnexpectedError = 1,
        ConnectFailed = 2,
        PermissionDenied = 3,
        CollectionNotExists = 4,
        IllegalArgument = 5,
        IllegalDimension = 7,
        IllegalIndexType = 8,
        IllegalCollectionName = 9,
        IllegalTOPK = 10,
        IllegalRowRecord = 11,
        IllegalVectorID = 12,
        IllegalSearchResult = 13,
        FileNotFound = 14,
        MetaFailed = 15,
        CacheFailed = 16,
        CannotCreateFolder = 17,
        CannotCreateFile = 18,
        CannotDeleteFolder = 19,
        CannotDeleteFile = 20,
        BuildIndexError = 21,
        IllegalNLIST = 22,
        IllegalMetricType = 23,
        OutOfMemory = 24,
        IndexNotExist = 25,
        EmptyCollection = 26,
        UpdateImportTaskFailure = 27,
        CollectionNameNotFound = 28,
        CreateCredentialFailure = 29,
        UpdateCredentialFailure = 30,
        DeleteCredentialFailure = 31,
        GetCredentialFailure = 32,
        ListCredUsersFailure = 33,
        GetUserFailure = 34,
        CreateRoleFailure = 35,
        DropRoleFailure = 36,
        OperateUserRoleFailure = 37,
        SelectRoleFailure = 38,
        SelectUserFailure = 39,
        SelectResourceFailure = 40,
        OperatePrivilegeFailure = 41,
        SelectGrantFailure = 42,
        RefreshPolicyInfoCacheFailure = 43,
        ListPolicyFailure = 44,
        NotShardLeader = 45,
        NoReplicaAvailable = 46,
        SegmentNotFound = 47,
        ForceDeny = 48,
        RateLimit = 49,
        NodeIDNotMatch = 50,
        UpsertAutoIDTrue = 51,
        InsufficientMemoryToLoad = 52,
        MemoryQuotaExhausted = 53,
        DiskQuotaExhausted = 54,
        TimeTickLongDelay = 55,
        NotReadyServe = 56,
        NotReadyCoordActivating = 57,
        DataCoordNA = 100,
        DDRequestRace = 1000,
        ErrorCode_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
        ErrorCode_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
    };

    using ErrorCodeEnum = ErrorCode;
    namespace impl
    {
        void
        EasyAssertInfo(bool value,
                       std::string_view expr_str,
                       std::string_view filename,
                       int lineno,
                       std::string_view extra_info,
                       ErrorCodeEnum error_code = ErrorCodeEnum::UnexpectedError);

    } // namespace impl

    class SegcoreError : public std::runtime_error
    {
    public:
        SegcoreError(ErrorCodeEnum error_code, const std::string &error_msg)
            : std::runtime_error(error_msg), error_code_(error_code)
        {
        }

        ErrorCodeEnum
        get_error_code()
        {
            return error_code_;
        }

    private:
        ErrorCodeEnum error_code_;
    };

} // namespace milvus

#define AssertInfo(expr, info)                                 \
    do                                                         \
    {                                                          \
        auto _expr_res = bool(expr);                           \
        /* call func only when needed */                       \
        if (!_expr_res)                                        \
        {                                                      \
            milvus::impl::EasyAssertInfo(                      \
                _expr_res, #expr, __FILE__, __LINE__, (info)); \
        }                                                      \
    } while (0)

#define Assert(expr) AssertInfo((expr), "")
#define PanicInfo(info)                                                      \
    do                                                                       \
    {                                                                        \
        milvus::impl::EasyAssertInfo(false, (info), __FILE__, __LINE__, ""); \
        __builtin_unreachable();                                             \
    } while (0)

#define PanicCodeInfo(errcode, info)                         \
    do                                                       \
    {                                                        \
        milvus::impl::EasyAssertInfo(                        \
            false, (info), __FILE__, __LINE__, "", errcode); \
        __builtin_unreachable();                             \
    } while (0)
