/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ecological_rule_mgr_service_proxy.h"
#include "iremote_broker.h"
#include "ecological_rule_mgr_service_logger.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
#define TAG "erms_proxy"
static inline const std::u16string ERMS_INTERFACE_TOKEN =
    u"ohos.cloud.ecologicalrulemgrservice.IEcologicalRuleMgrService";

EcologicalRuleMgrServiceProxy::EcologicalRuleMgrServiceProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IEcologicalRuleMgrService>(object)
{}

int32_t EcologicalRuleMgrServiceProxy::QueryFreeInstallExperience(const Want &want, const CallerInfo &callerInfo,
    ExperienceRule &rule)
{
    LOG_INFO("QueryFreeInstallExperience called");
    MessageParcel data;

    if (!data.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        LOG_ERROR("write token failed");
        return ERR_FAILED;
    }

    if (!data.WriteParcelable(&want)) {
        LOG_ERROR("write want failed");
        return ERR_FAILED;
    }

    if (!data.WriteParcelable(&callerInfo)) {
        LOG_ERROR("write callerInfo failed");
        return ERR_FAILED;
    }

    MessageOption option = { MessageOption::TF_SYNC };
    MessageParcel reply;

    auto remote = Remote();
    if (remote == nullptr) {
        LOG_ERROR("get Remote failed");
        return ERR_FAILED;
    }

    int32_t ret = remote->SendRequest(QUERY_FREE_INSTALL_EXPERIENCE_CMD, data, reply, option);
    if (ret != ERR_NONE) {
        LOG_ERROR("SendRequest error, ret = %{public}d", ret);
        return ERR_FAILED;
    }

    sptr<ExperienceRule> sptrRule = reply.ReadParcelable<ExperienceRule>();
    if (sptrRule == nullptr) {
        LOG_ERROR("readParcelable sptrRule error");
        return ERR_FAILED;
    }

    rule = *sptrRule;
    LOG_INFO("QueryFreeInstallExperience end");
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceProxy::EvaluateResolveInfos(const Want &want, const CallerInfo &callerInfo,
    int32_t type, std::vector<AbilityInfo> &abilityInfos)
{
    LOG_INFO("EvaluateResolveInfos called");
    MessageParcel data;

    if (!data.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        LOG_ERROR("write token failed");
        return ERR_FAILED;
    }

    if (!data.WriteParcelable(&want)) {
        LOG_ERROR("write want failed");
        return ERR_FAILED;
    }

    if (!data.WriteParcelable(&callerInfo)) {
        LOG_ERROR("write callerInfo failed");
        return ERR_FAILED;
    }

    if (!data.WriteInt32(type)) {
        LOG_ERROR("write type failed");
        return ERR_FAILED;
    }

    if (!data.WriteInt32(abilityInfos.size())) {
        LOG_ERROR("write abilityInfos size failed");
        return ERR_FAILED;
    }

    for (auto &abilityInfo : abilityInfos) {
        if (!data.WriteParcelable(&abilityInfo)) {
            LOG_ERROR("write abilityInfo failed");
            return ERR_FAILED;
        }
    }

    MessageOption option = { MessageOption::TF_SYNC };
    MessageParcel reply;

    auto remote = Remote();
    if (remote == nullptr) {
        LOG_ERROR("get Remote failed.");
        return ERR_FAILED;
    }

    int32_t ret = remote->SendRequest(EVALUATE_RESOLVE_INFO_CMD, data, reply, option);
    if (ret != ERR_NONE) {
        LOG_ERROR("SendRequest error, ret = %{public}d", ret);
        return ERR_FAILED;
    }

    if (!ReadParcelableVector(abilityInfos, reply)) {
        LOG_ERROR("GetParcelableInfos fail");
    }
    LOG_INFO("EvaluateResolveInfos end");
    return ERR_OK;
}

template <typename T>
bool EcologicalRuleMgrServiceProxy::ReadParcelableVector(std::vector<T> &parcelableVector, MessageParcel &reply)
{
    int32_t infoSize = reply.ReadInt32();
    parcelableVector.clear();
    for (int32_t i = 0; i < infoSize; i++) {
        sptr<T> info = reply.ReadParcelable<T>();
        if (info == nullptr) {
            LOG_ERROR("read Parcelable infos failed");
            return false;
        }
        parcelableVector.emplace_back(*info);
    }
    return true;
}

int32_t EcologicalRuleMgrServiceProxy::QueryStartExperience(const Want &want, const CallerInfo &callerInfo,
    ExperienceRule &rule)
{
    LOG_INFO("QueryStartExperience called");
    MessageParcel data;

    if (!data.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        LOG_ERROR("write token failed");
        return ERR_FAILED;
    }

    if (!data.WriteParcelable(&want)) {
        LOG_ERROR("write want failed");
        return ERR_FAILED;
    }

    if (!data.WriteParcelable(&callerInfo)) {
        LOG_ERROR("write callerInfo failed");
        return ERR_FAILED;
    }

    MessageOption option = { MessageOption::TF_SYNC };
    MessageParcel reply;

    auto remote = Remote();
    if (remote == nullptr) {
        LOG_ERROR("get Remote failed");
        return ERR_FAILED;
    }

    int32_t ret = remote->SendRequest(QUERY_START_EXPERIENCE_CMD, data, reply, option);
    if (ret != ERR_NONE) {
        LOG_ERROR("SendRequest error, ret = %{public}d", ret);
        return ERR_FAILED;
    }

    sptr<ExperienceRule> sptrRule = reply.ReadParcelable<ExperienceRule>();
    if (sptrRule == nullptr) {
        LOG_ERROR("ReadParcelable sptrRule error");
        return ERR_FAILED;
    }

    rule = *sptrRule;
    LOG_INFO("QueryStartExperience end");
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceProxy::IsSupportPublishForm(const std::vector<Want> &wants,
    const CallerInfo &callerInfo, bool &bSupport)
{
    LOG_INFO("IsSupportPublishForm called");
    MessageParcel data;

    if (!data.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        LOG_ERROR("write token failed");
        return ERR_FAILED;
    }

    if (!data.WriteInt32(wants.size())) {
        LOG_ERROR("write wants size failed");
        return ERR_FAILED;
    }

    for (auto &want : wants) {
        if (!data.WriteParcelable(&want)) {
            LOG_ERROR("write want failed");
            return ERR_FAILED;
        }
    }

    if (!data.WriteParcelable(&callerInfo)) {
        LOG_ERROR("write callerInfo failed");
        return ERR_FAILED;
    }

    MessageOption option = { MessageOption::TF_SYNC };
    MessageParcel reply;

    auto remote = Remote();
    if (remote == nullptr) {
        LOG_ERROR("get Remote failed");
        return ERR_FAILED;
    }

    int32_t ret = remote->SendRequest(IS_SUPPORT_PUBLISH_FORM_CMD, data, reply, option);
    if (ret != ERR_NONE) {
        LOG_ERROR("SendRequest error, ret = %{public}d", ret);
        return ERR_FAILED;
    }

    bSupport = reply.ReadBool();

    LOG_INFO("IsSupportPublishForm end, bSupport=%{public}d", bSupport);
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceProxy::SetAppEnhancedData(const int32_t &operType, const std::string &appData)
{
    LOG_INFO("SetAppEnhancedData called");
    MessageParcel data;

    if (!data.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        LOG_ERROR("write token failed");
        return ERR_FAILED;
    }

    if (!data.WriteInt32(operType)) {
        LOG_ERROR("write operType failed");
        return ERR_FAILED;
    }

    if (!data.WriteString(appData)) {
        LOG_ERROR("write appData failed");
        return ERR_FAILED;
    }

    MessageOption option = { MessageOption::TF_SYNC };
    MessageParcel reply;

    auto remote = Remote();
    if (remote == nullptr) {
        LOG_ERROR("get Remote failed");
        return ERR_FAILED;
    }

    int32_t ret = remote->SendRequest(SET_APP_ENHANCED_DATA_CMD, data, reply, option);
    if (ret != ERR_NONE) {
        LOG_ERROR("SendRequest error, ret = %{public}d", ret);
        return ERR_FAILED;
    }
    LOG_INFO("SetAppEnhancedData end");
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceProxy::SetRuleInfo(const std::string &ruleInfo)
{
    LOG_INFO("SetRuleInfo called");
    MessageParcel data;

    if (!data.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        LOG_ERROR("write token failed");
        return ERR_FAILED;
    }

    if (!data.WriteString(ruleInfo)) {
        LOG_ERROR("write ruleInfo failed");
        return ERR_FAILED;
    }

    MessageOption option = { MessageOption::TF_SYNC };
    MessageParcel reply;

    auto remote = Remote();
    if (remote == nullptr) {
        LOG_ERROR("get Remote failed");
        return ERR_FAILED;
    }

    int32_t ret = remote->SendRequest(SET_RULE_INFO_CMD, data, reply, option);
    if (ret != ERR_NONE) {
        LOG_ERROR("SendRequest error, ret = %{public}d", ret);
        return ERR_FAILED;
    }
    LOG_INFO("SetRuleInfo end");
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceProxy::ExemptExperience(const std::string &targetBundleName, const int32_t &rule,
    const int32_t &timestamp)
{
    LOG_INFO("ExemptExperience called");
    MessageParcel data;

    if (!data.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        LOG_ERROR("write token failed");
        return ERR_FAILED;
    }

    if (!data.WriteString(targetBundleName)) {
        LOG_ERROR("write targetBundleName failed");
        return ERR_FAILED;
    }

    if (!data.WriteInt32(rule)) {
        LOG_ERROR("write rule failed");
        return ERR_FAILED;
    }

    if (!data.WriteInt32(timestamp)) {
        LOG_ERROR("write timestamp failed");
        return ERR_FAILED;
    }

    MessageOption option = { MessageOption::TF_SYNC };
    MessageParcel reply;

    auto remote = Remote();
    if (remote == nullptr) {
        LOG_ERROR("get Remote failed");
        return ERR_FAILED;
    }

    int32_t ret = remote->SendRequest(SET_EXEMPT_EXPERIENCE_CMD, data, reply, option);
    if (ret != ERR_NONE) {
        LOG_ERROR("SendRequest error, ret = %{public}d", ret);
        return ERR_FAILED;
    }
    LOG_INFO("ExemptExperience end");
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceProxy::GetVersion(std::string &versionJson)
{
    LOG_INFO("GetVersion called");
    MessageParcel data;

    if (!data.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        LOG_ERROR("write token failed");
        return ERR_FAILED;
    }

    MessageOption option = { MessageOption::TF_SYNC };
    MessageParcel reply;

    auto remote = Remote();
    if (remote == nullptr) {
        LOG_ERROR("get Remote failed");
        return ERR_FAILED;
    }

    int32_t ret = remote->SendRequest(GET_ENGINE_VERSION_CMD, data, reply, option);
    if (ret != ERR_NONE) {
        LOG_ERROR("SendRequest error, ret = %{public}d", ret);
        return ERR_FAILED;
    }

    versionJson = reply.ReadString();
    LOG_INFO("GetVersion end");
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceProxy::SetSceneExperience(std::string &ruleConfig, std::string &sceneExperience)
{
    LOG_INFO("SetSceneExperience called");
    MessageParcel data;

    if (!data.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        LOG_ERROR("write token failed");
        return ERR_FAILED;
    }

    if (!data.WriteString(ruleConfig)) {
        LOG_ERROR("write ruleConfig failed");
        return ERR_FAILED;
    }

    if (!data.WriteString(sceneExperience)) {
        LOG_ERROR("write sceneExperiences failed");
        return ERR_FAILED;
    }

    MessageOption option = { MessageOption::TF_SYNC };
    MessageParcel reply;

    auto remote = Remote();
    if (remote == nullptr) {
        LOG_ERROR("get Remote failed");
        return ERR_FAILED;
    }

    int32_t ret = remote->SendRequest(SET_SCENE_EXPERIENCE_CMD, data, reply, option);
    if (ret != ERR_NONE) {
        LOG_ERROR("SendRequest error, ret = %{public}d", ret);
        return ERR_FAILED;
    }
    LOG_INFO("SetSceneExperience end");
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceProxy::GetSceneCode(const std::string &bundleName, std::string &sceneCode)
{
    LOG_INFO("GetSceneCode called");
    MessageParcel data;

    if (!data.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        LOG_ERROR("write token failed");
        return ERR_FAILED;
    }

    if (!data.WriteString(bundleName)) {
        LOG_ERROR("write bundleName failed");
        return ERR_FAILED;
    }

    MessageOption option = { MessageOption::TF_SYNC };
    MessageParcel reply;

    auto remote = Remote();
    if (remote == nullptr) {
        LOG_ERROR("get Remote failed");
        return ERR_FAILED;
    }

    int32_t ret = remote->SendRequest(GET_SCENE_CODE_CMD, data, reply, option);
    if (ret != ERR_NONE) {
        LOG_ERROR("SendRequest error, ret = %{public}d", ret);
        return ERR_FAILED;
    }

    sceneCode = reply.ReadString();
    LOG_INFO("GetSceneCode end");
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceProxy::GetInitialSceneCode(const std::string &bundleName,
    std::string &originalSceneCode)
{
    LOG_INFO("GetInitialSceneCode called");
    MessageParcel data;

    if (!data.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        LOG_ERROR("write token failed");
        return ERR_FAILED;
    }

    if (!data.WriteString(bundleName)) {
        LOG_ERROR("write bundleName failed");
        return ERR_FAILED;
    }

    MessageOption option = { MessageOption::TF_SYNC };
    MessageParcel reply;

    auto remote = Remote();
    if (remote == nullptr) {
        LOG_ERROR("get Remote failed");
        return ERR_FAILED;
    }

    int32_t ret = remote->SendRequest(GET_ORIGINAL_SCENE_CODE_CMD, data, reply, option);
    if (ret != ERR_NONE) {
        LOG_ERROR("SendRequest error, ret = %{public}d", ret);
        return ERR_FAILED;
    }

    originalSceneCode = reply.ReadString();
    LOG_INFO("GetInitialSceneCode end");
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceProxy::GetSelfSceneCode(std::string &sceneCode)
{
    LOG_INFO("GetSelfSceneCode called");
    MessageParcel data;

    if (!data.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        LOG_ERROR("write token failed");
        return ERR_FAILED;
    }

    MessageOption option = { MessageOption::TF_SYNC };
    MessageParcel reply;

    auto remote = Remote();
    if (remote == nullptr) {
        LOG_ERROR("get Remote failed");
        return ERR_FAILED;
    }

    int32_t ret = remote->SendRequest(GET_SELF_SCENE_CODE_CMD, data, reply, option);
    if (ret != ERR_NONE) {
        LOG_ERROR("SendRequest error, ret = %{public}d", ret);
        return ERR_FAILED;
    }

    sceneCode = reply.ReadString();
    LOG_INFO("GetSelfSceneCode end");
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceProxy::GetAdsVerificationVersion(int32_t &version)
{
    LOG_INFO("GetAdsVerificationVersion called");
    MessageParcel data;

    if (!data.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        LOG_ERROR("write token failed");
        return ERR_FAILED;
    }

    MessageOption option = { MessageOption::TF_SYNC };
    MessageParcel reply;

    auto remote = Remote();
    if (remote == nullptr) {
        LOG_ERROR("get Remote failed");
        return ERR_FAILED;
    }

    int32_t ret = remote->SendRequest(GET_ADS_VERIFICATION_VERSION_CMD, data, reply, option);
    if (ret != ERR_NONE) {
        LOG_ERROR("SendRequest error, ret = %{public}d", ret);
        return ERR_FAILED;
    }

    version = reply.ReadInt32();
    LOG_INFO("GetAdsVerificationVersion end");
    return ERR_OK;
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS
