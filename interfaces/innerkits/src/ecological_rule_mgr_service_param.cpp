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

#include "ecological_rule_mgr_service_param.h"
#include "iremote_broker.h"
#include "iremote_object.h"

#include "ecological_rule_mgr_service_logger.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
#define TAG "ERMS_PARAM"

ExperienceRule *ExperienceRule::Unmarshalling(Parcel &in)
{
    return nullptr;
}

bool ExperienceRule::Marshalling(Parcel &parcel) const
{
    return true;
}

AmsExperienceRule *AmsExperienceRule::Unmarshalling(Parcel &in)
{
    auto *rule = new (std::nothrow) AmsExperienceRule();
    if (rule == nullptr) {
        return nullptr;
    }
    if (!in.ReadInt32(rule->resultCode)) {
        delete rule;
        return nullptr;
    }
    rule->replaceWant = in.ReadParcelable<Want>();
    return rule;
}

bool AmsExperienceRule::Marshalling(Parcel &parcel) const
{
    bool res = parcel.WriteInt32(resultCode) && parcel.WriteParcelable(replaceWant);
    if (!res) {
        LOG_ERROR("AmsExperienceRule Marshalling failed");
        return false;
    }
    return true;
}

BmsExperienceRule *BmsExperienceRule::Unmarshalling(Parcel &in)
{
    auto *rule = new (std::nothrow) BmsExperienceRule();
    if (rule == nullptr) {
        return nullptr;
    }
    if (!in.ReadBool(rule->isAllow)) {
        delete rule;
        return nullptr;
    }
    rule->replaceWant = in.ReadParcelable<Want>();
    return rule;
}

bool BmsExperienceRule::Marshalling(Parcel &parcel) const
{
    bool res = parcel.WriteBool(isAllow) && parcel.WriteParcelable(replaceWant);
    if (!res) {
        LOG_ERROR("BmsExperienceRule Marshalling failed");
        return false;
    }
    return true;
}

bool CallerInfo::ReadFromParcel(Parcel &parcel)
{
    LOG_INFO("read from parcel");
    return true;
}

CallerInfo *CallerInfo::Unmarshalling(Parcel &in)
{
    auto *info = new (std::nothrow) CallerInfo();
    if (info == nullptr) {
        LOG_ERROR("new callerInfo failed, return nullptr");
        return nullptr;
    }
    info->packageName = in.ReadString();
    LOG_INFO("read packageName result: %{public}s", info->packageName.c_str());
    bool res = in.ReadInt32(info->uid) && in.ReadInt32(info->pid) && in.ReadInt32(info->callerAppType) &&
        in.ReadInt32(info->targetAppType);
    if (!in.ReadInt32(info->callerModeType)) {
        LOG_DEBUG("read callerModeType failed");
        info->callerModeType = 0;
    } else {
        LOG_DEBUG("read callerModeType is %{public}d", info->callerModeType);
    }
    info->targetAppDistType = in.ReadString();
    info->targetLinkFeature = in.ReadString();
    if (!in.ReadInt32(info->targetLinkType)) {
        LOG_DEBUG("read targetLinkType failed");
        info->targetLinkType = 0;
    } else {
        LOG_DEBUG("read targetLinkType is %{public}d", info->targetLinkType);
    }
    int callerAbilityType = 0;
    if (in.ReadInt32(callerAbilityType)) {
        info->callerAbilityType = static_cast<AppExecFwk::AbilityType>(callerAbilityType);
    }
    if (!in.ReadInt32(info->embedded)) {
        LOG_DEBUG("read embedded failed");
        info->embedded = 0;
    } else {
        LOG_DEBUG("read embedded is %{public}d", info->embedded);
    }
    info->callerAppProvisionType = in.ReadString();
    info->targetAppProvisionType = in.ReadString();
    int32_t callerExtensionAbilityType = 0;
    if (in.ReadInt32(callerExtensionAbilityType)) {
        info->callerExtensionAbilityType = static_cast<AppExecFwk::ExtensionAbilityType>(callerExtensionAbilityType);
    }
    info->targetAbilityType = static_cast<AppExecFwk::AbilityType>(in.readInt32());
    info->targetExtensionAbilityType = static_cast<AppExecFwk::ExtensionAbilityType>(in.readInt32());
    if (!res) {
        LOG_ERROR("read callerInfo information failed");
        delete info;
        return nullptr;
    }
    return info;
}

bool CallerInfo::Marshalling(Parcel &parcel) const
{
    bool res = parcel.WriteString(packageName) && parcel.WriteInt32(uid) && parcel.WriteInt32(pid) &&
        parcel.WriteInt32(callerAppType) && parcel.WriteInt32(targetAppType);
    if (!res) {
        LOG_ERROR("write CallerInfo failed");
        return false;
    }
    return true;
}

std::string CallerInfo::ToString() const
{
    std::string str = "CallerInfo{packageName:" + packageName + ",uid:" + std::to_string(uid) +
        ",pid:" + std::to_string(pid) + ",callerAppType:" + std::to_string(callerAppType) +
        ",targetAppType:" + std::to_string(targetAppType) + ",callerModeType:" + std::to_string(callerModeType) +
        ",targetAppDistType:" + targetAppDistType + ",targetLinkFeature:" + targetLinkFeature + ",targetLinkType:" +
        std::to_string(targetLinkType) + ",callerAbilityType:" +
        std::to_string(static_cast<int32_t>(callerAbilityType)) + ",callerExtensionAbilityType:" +
        std::to_string(static_cast<int32_t>(callerExtensionAbilityType)) + ",embedded:" +
        std::to_string(embedded) + ",callerAppProvisionType:" + callerAppProvisionType +
        ",targetAppProvisionType:" + targetAppProvisionType + ",targetAbilityType:" + 
        std::to_string(static_cast<int32_t>(targetAbilityType)) + ",targetExtensionAbilityType:" +
        std::to_string(static_cast<int32_t>(targetExtensionAbilityType)) + "}";
    return str;
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS
