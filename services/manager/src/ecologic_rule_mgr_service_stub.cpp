/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <thread>
#include <chrono>
#include "ecological_rule_mgr_service_stub.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "ecological_rule_mgr_service_logger.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
#define TAG "ERMS_STUB"

static inline const std::u16string ERMS_INTERFACE_TOKEN =
    u"ohos.cloud.ecologicalrulemgrservice.IEcologicalRuleMgrService";

std::mutex EcologicalRuleMgrServiceStub::bmInstanceLock_;
EcologicalRuleMgrServiceStub::EcologicalRuleMgrServiceStub()
{
    memberFuncMap_[QUERY_FREE_INSTALL_EXPERIENCE_CMD] =
        &EcologicalRuleMgrServiceStub::OnQueryFreeInstallExperienceResult;
    memberFuncMap_[QUERY_START_EXPERIENCE_CMD] = &EcologicalRuleMgrServiceStub::OnQueryStartExperienceResult;
    memberFuncMap_[EVALUATE_RESOLVE_INFO_CMD] = &EcologicalRuleMgrServiceStub::OnEvaluateResolveInfosResult;
    memberFuncMap_[IS_SUPPORT_PUBLISH_FORM_CMD] = &EcologicalRuleMgrServiceStub::OnIsSupportPublishFormResult;
}

EcologicalRuleMgrServiceStub::~EcologicalRuleMgrServiceStub()
{
    memberFuncMap_.clear();
}

int32_t EcologicalRuleMgrServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    LOG_INFO("OnRemoteRequest called, code=%{public}d", code);
    if (!EnforceInterceToken(data)) {
        LOG_ERROR("check interface token failed");
        return ERR_PERMISSION_DENIED;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    auto defaultRet = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    return defaultRet;
}

int32_t EcologicalRuleMgrServiceStub::OnQueryFreeInstallExperienceResult(MessageParcel &data, MessageParcel &reply)
{
    if (!VerifySystemApp()) {
        LOG_ERROR("verify system app failed");
        return ERR_FAILED;
    }
    sptr<Want> want = data.ReadParcelable<Want>();
    if (want == nullptr) {
        LOG_ERROR("read want failed");
        return ERR_FAILED;
    }
    sptr<CallerInfo> caller = data.ReadParcelable<CallerInfo>();
    if (caller == nullptr) {
        LOG_ERROR("read caller failed");
        return ERR_FAILED;
    }
    sptr<ExperienceRule> rule = new ExperienceRule();
    QueryFreeInstallExperience(*want, *caller, *rule);
    if (!reply.WriteParcelable(rule)) {
        LOG_ERROR("write rule failed");
        return ERR_FAILED;
    }
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceStub::OnEvaluateResolveInfosResult(MessageParcel &data, MessageParcel &reply)
{
    if (!VerifySystemApp()) {
        LOG_ERROR("verify system app failed");
        return ERR_FAILED;
    }
    sptr<Want> want = data.ReadParcelable<Want>();
    if (want == nullptr) {
        LOG_ERROR("read want failed");
        return ERR_FAILED;
    }

    int32_t type = 0;
    if (!data.ReadInt32(type)) {
        LOG_ERROR("read type failed");
        return ERR_FAILED;
    }
    LOG_DEBUG("read type = %{public}d", type);
    std::vector<AbilityInfo> abilityInfos = {};
    int32_t abilityInfoSize = 0;
    if (!data.ReadInt32(abilityInfoSize)) {
        LOG_ERROR("read abilityInfoSize failed");
        return ERR_FAILED;
    }
    if (abilityInfoSize > MAX_ABILITY_INFO_SIZE) {
        LOG_ERROR("abilityInfoSize exceed the maximum limit, abilityInfoSize = %{public}d, limit = %{public}d",
            abilityInfoSize, MAX_ABILITY_INFO_SIZE);
        return ERR_FAILED;
    }
    for (int32_t i = 0; i < abilityInfoSize; i++) {
        sptr<AbilityInfo> abilityInfo = data.ReadParcelable<AbilityInfo>();
        if (abilityInfo == nullptr) {
            LOG_ERROR("read abilityInfo failed");
            return ERR_FAILED;
        }
        abilityInfos.emplace_back(*abilityInfo);
    }
    int32_t infoSize = static_cast<int32_t>(abilityInfos.size());
    LOG_DEBUG("before process abilityInfos size= %{public}d", infoSize);

    sptr<CallerInfo> caller = data.ReadParcelable<CallerInfo>();
    if (caller == nullptr) {
        LOG_ERROR("read caller failed");
        return ERR_FAILED;
    }
    EvaluateResolveInfos(*want, *caller, type, abilityInfos);
    if (!WriteParcelableVector(abilityInfos, reply)) {
        LOG_ERROR("WriteParcelableVector failed");
        return ERR_FAILED;
    }
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceStub::OnQueryStartExperienceResult(MessageParcel &data, MessageParcel &reply)
{
    if (!VerifySystemApp()) {
        LOG_ERROR("verify system app failed");
        return ERR_FAILED;
    }
    sptr<Want> want = data.ReadParcelable<Want>();
    if (want == nullptr) {
        LOG_ERROR("read want failed");
        return ERR_FAILED;
    }
    sptr<CallerInfo> caller = data.ReadParcelable<CallerInfo>();
    if (caller == nullptr) {
        LOG_ERROR("read caller failed");
        return ERR_FAILED;
    }
    sptr<ExperienceRule> rule = new ExperienceRule();
    QueryStartExperience(*want, *caller, *rule);
    if (!reply.WriteParcelable(rule)) {
        LOG_ERROR("write rule failed");
        return ERR_FAILED;
    }
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceStub::OnIsSupportPublishFormResult(MessageParcel &data, MessageParcel &reply)
{
    if (!VerifySystemApp()) {
        LOG_ERROR("verify system app failed");
        return ERR_FAILED;
    }
    std::vector<Want> wants = {};
    int32_t wantSize = 0;
    if (!data.ReadInt32(wantSize)) {
        LOG_ERROR("read wantSize failed");
        return ERR_FAILED;
    }
    if (wantSize > MAX_WANT_SIZE) {
        LOG_ERROR("wantSize exceed the maximum limit, wantSize = %{public}d, limit = %{public}d", wantSize,
            MAX_WANT_SIZE);
        return ERR_FAILED;
    }
    for (int32_t i = 0; i < wantSize; i++) {
        sptr<Want> want = data.ReadParcelable<Want>();
        if (want == nullptr) {
            LOG_ERROR("read wants failed");
            return ERR_FAILED;
        }
        wants.emplace_back(*want);
    }

    sptr<CallerInfo> caller = data.ReadParcelable<CallerInfo>();
    if (caller == nullptr) {
        LOG_ERROR("read caller failed");
        return ERR_FAILED;
    }
    bool bSupport = true;
    IsSupportPublishForm(wants, *caller, bSupport);

    if (!reply.WriteBool(bSupport)) {
        LOG_ERROR("write bSupport failed");
        return ERR_FAILED;
    }
    return ERR_OK;
}

bool EcologicalRuleMgrServiceStub::EnforceInterceToken(MessageParcel &data)
{
    std::u16string interfaceToken = data.ReadInterfaceToken();
    return interfaceToken == ERMS_INTERFACE_TOKEN;
}

bool EcologicalRuleMgrServiceStub::VerifySystemApp()
{
    LOG_INFO("verifying systemApp");
    Security::AccessToken::AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    Security::AccessToken::ATokenTypeEnum tokenType =
        Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken);
    LOG_INFO("token type is %{public}d", static_cast<int32_t>(tokenType));
    int32_t callingUid = IPCSkeleton::GetCallingUid();
    if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE
        || tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL
        || callingUid == Constants::ROOT_UID) {
        LOG_INFO("caller tokenType is native, verify success");
        return true;
    }
    uint64_t accessTokenIdEx = IPCSkeleton::GetCallingFullTokenID();
    if (!Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(accessTokenIdEx)) {
        LOG_INFO("non-system app calling system api");
        return false;
    }
    return true;
}

template <typename T>
bool EcologicalRuleMgrServiceStub::WriteParcelableVector(const std::vector<T> &parcelableVector, MessageParcel &reply)
{
    if (!reply.WriteInt32(parcelableVector.size())) {
        LOG_ERROR("write ParcelableVector size failed");
        return false;
    }

    for (auto &parcelable : parcelableVector) {
        if (!reply.WriteParcelable(&parcelable)) {
            LOG_ERROR("write ParcelableVector failed");
            return false;
        }
    }
    return true;
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS