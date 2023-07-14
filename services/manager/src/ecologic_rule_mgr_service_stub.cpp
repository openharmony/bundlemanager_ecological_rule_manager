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

    memberFuncMap_[GET_ENGINE_VERSION_CMD] = &EcologicalRuleMgrServiceStub::OnGetVersionResult;
    memberFuncMap_[SET_EXEMPT_EXPERIENCE_CMD] = &EcologicalRuleMgrServiceStub::OnExemptExperienceResult;
    memberFuncMap_[SET_RULE_INFO_CMD] = &EcologicalRuleMgrServiceStub::OnSetRuleInfoResult;
    memberFuncMap_[SET_APP_ENHANCED_DATA_CMD] = &EcologicalRuleMgrServiceStub::OnSetAppEnhancedDataResult;
    memberFuncMap_[SET_SCENE_EXPERIENCE_CMD] = &EcologicalRuleMgrServiceStub::OnSetSceneExperienceResult;

    memberFuncMap_[GET_SCENE_CODE_CMD] = &EcologicalRuleMgrServiceStub::OnGetSceneCodeResult;
    memberFuncMap_[GET_ORIGINAL_SCENE_CODE_CMD] = &EcologicalRuleMgrServiceStub::OnGetInitialSceneCodeResult;
    memberFuncMap_[GET_SELF_SCENE_CODE_CMD] = &EcologicalRuleMgrServiceStub::OnGetSelfSceneCodeResult;
    memberFuncMap_[GET_ADS_VERIFICATION_VERSION_CMD] = &EcologicalRuleMgrServiceStub::OnGetAdsVerificationVersionResult;
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

    sptr<CallerInfo> caller = data.ReadParcelable<CallerInfo>();
    if (caller == nullptr) {
        LOG_ERROR("read caller failed");
        return ERR_FAILED;
    }

    int32_t type = 0;
    if (!data.ReadInt32(type)) {
        LOG_ERROR("read type failed");
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

int32_t EcologicalRuleMgrServiceStub::OnSetAppEnhancedDataResult(MessageParcel &data, MessageParcel &reply)
{
    if (!VerifyCallingPermission(PERMISSION_MANAGE_ECOLOGICAL_RULE)) {
        LOG_ERROR("verify permission failed");
        return ERR_FAILED;
    }
    int32_t type;
    if (!data.ReadInt32(type)) {
        LOG_ERROR("read type failed");
        return ERR_FAILED;
    }
    std::string appData;
    if (!data.ReadString(appData)) {
        LOG_ERROR("read appData failed");
        return ERR_FAILED;
    }
    SetAppEnhancedData(type, appData);
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceStub::OnSetRuleInfoResult(MessageParcel &data, MessageParcel &reply)
{
    if (!VerifyCallingPermission(PERMISSION_MANAGE_ECOLOGICAL_RULE)) {
        LOG_ERROR("verify permission failed");
        return ERR_FAILED;
    }
    std::string ruleInfo;
    if (!data.ReadString(ruleInfo)) {
        LOG_ERROR("read ruleInfo failed");
        return ERR_FAILED;
    }
    SetRuleInfo(ruleInfo);
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceStub::OnExemptExperienceResult(MessageParcel &data, MessageParcel &reply)
{
    if (!VerifyCallingPermission(PERMISSION_MANAGE_ECOLOGICAL_RULE)) {
        LOG_ERROR("verify permission failed");
        return ERR_FAILED;
    }
    std::string targetBundleName;
    if (!data.ReadString(targetBundleName)) {
        LOG_ERROR("read targetBundleName failed");
        return ERR_FAILED;
    }

    int32_t rule;
    if (!data.ReadInt32(rule)) {
        LOG_ERROR("read rule failed");
        return ERR_FAILED;
    }

    int32_t timestamp;
    if (!data.ReadInt32(timestamp)) {
        LOG_ERROR("read timestamp failed");
        return ERR_FAILED;
    }

    ExemptExperience(targetBundleName, rule, timestamp);
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceStub::OnGetVersionResult(MessageParcel &data, MessageParcel &reply)
{
    if (!VerifyCallingPermission(PERMISSION_MANAGE_ECOLOGICAL_RULE)) {
        LOG_ERROR("verify permission failed");
        return ERR_FAILED;
    }
    std::string versionJson;
    GetVersion(versionJson);
    reply.WriteString(versionJson);
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceStub::OnSetSceneExperienceResult(MessageParcel &data, MessageParcel &reply)
{
    if (!VerifyCallingPermission(PERMISSION_MANAGE_ECOLOGICAL_RULE)) {
        LOG_ERROR("verify permission failed");
        return ERR_FAILED;
    }
    std::string ruleConfig;
    if (!data.ReadString(ruleConfig)) {
        LOG_ERROR("read ruleConfig failed");
        return ERR_FAILED;
    }

    std::string sceneExperience;
    if (!data.ReadString(sceneExperience)) {
        LOG_ERROR("read sceneExperiences failed");
        return ERR_FAILED;
    }

    SetSceneExperience(ruleConfig, sceneExperience);
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceStub::OnGetSceneCodeResult(MessageParcel &data, MessageParcel &reply)
{
    if (!VerifyCallingPermission(PERMISSION_GET_SCENE_CODE)) {
        LOG_ERROR("verify permission failed");
        return ERR_FAILED;
    }
    std::string bundleName;
    if (!data.ReadString(bundleName)) {
        LOG_ERROR("read bundleNamee failed");
        return ERR_FAILED;
    }
    std::string sceneCode;
    GetSceneCode(bundleName, sceneCode);

    reply.WriteString(sceneCode);
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceStub::OnGetInitialSceneCodeResult(MessageParcel &data, MessageParcel &reply)
{
    if (!VerifyCallingPermission(PERMISSION_GET_SCENE_CODE)) {
        LOG_ERROR("verify permission failed");
        return ERR_FAILED;
    }
    std::string bundleName;
    if (!data.ReadString(bundleName)) {
        LOG_ERROR("read bundleNamee failed");
        return ERR_FAILED;
    }
    std::string originalSceneCode;
    GetInitialSceneCode(bundleName, originalSceneCode);

    reply.WriteString(originalSceneCode);
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceStub::OnGetSelfSceneCodeResult(MessageParcel &data, MessageParcel &reply)
{
    std::string sceneCode;
    GetSelfSceneCode(sceneCode);
    reply.WriteString(sceneCode);
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceStub::OnGetAdsVerificationVersionResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t version;
    GetAdsVerificationVersion(version);
    reply.WriteInt32(version);
    return ERR_OK;
}

bool EcologicalRuleMgrServiceStub::EnforceInterceToken(MessageParcel &data)
{
    std::u16string interfaceToken = data.ReadInterfaceToken();
    return interfaceToken == ERMS_INTERFACE_TOKEN;
}

bool EcologicalRuleMgrServiceStub::VerifyCallingPermission(const std::string &permissionName)
{
    LOG_INFO("VerifyCallingPermission permission %{public}s", permissionName.c_str());
    OHOS::Security::AccessToken::AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    OHOS::Security::AccessToken::ATokenTypeEnum tokenType =
        OHOS::Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType == OHOS::Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        LOG_INFO("caller tokenType is native, verify success");
        return true;
    }
    int32_t ret = OHOS::Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken, permissionName);
    if (ret == OHOS::Security::AccessToken::PermissionState::PERMISSION_DENIED) {
        LOG_INFO("permission %{public}s: PERMISSION_DENIED", permissionName.c_str());
        return false;
    }
    LOG_INFO("verify permission success");
    return true;
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